#include "log.hh"
#include "action.hh"

action::action (const std::string &name_)
: name (name_)
, interrupted (false)
, waiting (false)
{
    init ();
}

action::~action ()
{
}

void action::init ()
{
    // initialize the barrier for two threads: a producer and a consumer
    LOG (name << ": Initializing");
    // pthread_barrier_init (&barrier, NULL, 2);
    pthread_mutex_init (&action_mutex, NULL);
    pthread_mutex_init (&cond_mutex, NULL);
    pthread_cond_init (&cond, NULL);
    interrupted = false;
    waiting = false;
}

bool action::wait ()
{
    pthread_mutex_lock (&action_mutex);

    if (waiting) {
        /* wake up the waiting thread and return */
        waiting = false;
        LOG (name << ": Resuming from sleep");
        pthread_mutex_unlock (&action_mutex);
        pthread_cond_signal (&cond);
    } else {
        /* sleep current thread for a subsequent wait */
        LOG (name << ": Going to sleep");
        waiting = true;
        pthread_mutex_unlock (&action_mutex);
        while (waiting) {
            pthread_mutex_lock (&cond_mutex);
            pthread_cond_wait (&cond, &cond_mutex);
            pthread_mutex_unlock (&cond_mutex);
        }
    }

    return !interrupted;
}

void action::interrupt ()
{
    LOG (name << ": Marking action interrupted");
    // interrupted = true;
    wait ();
}

void action::reset ()
{
    LOG (name << ": Resetting");
    // pthread_barrier_destroy (&barrier);
    // pthread_mutex_destroy (&mutex);
    // pthread_mutex_destroy
    // pthread_cond_destroy (&cond);
    init ();
}

std::string action::named () const
{
    return name;
}

