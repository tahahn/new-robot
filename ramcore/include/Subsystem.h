#pragma once

#include <memory>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <thread>
#include <signal.h>
#include <zmq.hpp>
#include <sys/time.h>
#include <json/json.h>

using namespace std;

namespace ramcore
{
    class Subsystem
    {
    private:
        /* Keeps track of how long the subsystem should wait between updates, in milliseconds. */
        int interval = 0;

    public:
        /* Sets the interval at which the subsystem should run updates. */
        void setUpdateInterval(int milliseconds);

        /* Enters the subprocesses main loop. */
        void run();

        /* Inheritable function for performing subsystem specific code at startup. */
        virtual void onSetup(){}
        /* Inheritable function for performing subsystem specific code on an update */
        virtual void onUpdate(){}
        /* Inheritable function for performing subsystem specific code at shutdown. */
        virtual void onShutdown(){}
    };

    /* A flag for detecting when the process is shutting down. */
    /* We use volatile here to indicate that this might be modified by a different thread (i.e. the signal handler) */
    extern volatile bool proc_shutdown;

    /* Stores the configration retrieved from stdin */
    extern Json::Value proc_config;

    /* When specified with multiple subsystems for the process, we use this vector to store all the subsystems. */
    extern vector<Subsystem *> proc_subsystems;

    /* A ZMQ context used with the Event/RPC system */
    extern shared_ptr<zmq::context_t> proc_context;

    /* Returns true if the process is shutting down, false otherwise. */
    /* Use this in a main loop to shutdown when necessary. */
    bool isShutdown();
    /* Trigger a shutdown of the process.  Also used when the signal SIGINT is recieved. */
    void triggerShutdown(int param = 0);

    /* Returns the title of the process.  
        NOTE: this is only the name specified by the config file, and is separate from the OS name. */
    string getProcessName();
    /* Returns the top level directory of the R@M environment. */
    string getTopDirectory();
    /* Returns the directory where the executable for this current process is. */
    string getProgramDirectory();

    /* Get the configuration valuse for this specific process. 
        NOTE: The value returned does not include the "subprocess_data" section or the configuration for any other subprocess. */
    Json::Value getConfiguration();

    /* Returns the context for this specific process.*/
    shared_ptr<zmq::context_t> getZMQContext();

    /* Initializes the subprocess, incluing loading the configuration file and setting up the ZMQ context.
        zmq_threads is the number of threads ZMQ should use to handle message IO. */
    void initSubprocess(int zmq_threads = 1);
    /* Runs the main loop for a single subsystem. */
    void runSubsystem(Subsystem *subsys);

    /* Adds a subsystem for this process to run. */
    void addSubsystem(Subsystem *subsys);
    /* Runs the main loop for each of the subsystems aded by addSubsystem()
        NOTE: Each subsystem runs in a separate thread, so use messages to avoid locking/data races. */
    void runSubsystems();
};
