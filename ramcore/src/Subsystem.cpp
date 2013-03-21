#include "Subsystem.h"

#include <chrono>

using namespace std;

namespace ramcore{

    void Subsystem::setUpdateInterval(int milliseconds){interval = milliseconds;}

    void Subsystem::run()
    {
        onSetup();

        chrono::system_clock::time_point last;

        while(!isShutdown())
        {
            //Get the current time, update, then wait until the next specified time.
            last = chrono::system_clock::now();
            onUpdate();
            this_thread::sleep_until(last + std::chrono::milliseconds(interval));
        }

        onShutdown();
    }

    //////////////////////////////////////////////////

    volatile bool proc_shutdown = false;
    Json::Value proc_config;

    vector<Subsystem *> proc_subsystems;

    shared_ptr<zmq::context_t> proc_context;

    //////////////////////////////////////////////////

    bool isShutdown(){return proc_shutdown;}
    void triggerShutdown(int param){proc_shutdown = true;}

    string getProcessName(){return proc_config["subprocess_data"]["name"].asString();}
    string getTopDirectory(){return proc_config["subprocess_data"]["top_dir"].asString();}
    string getProgramDirectory(){return proc_config["subprocess_data"]["prog_dir"].asString();}

    Json::Value getConfiguration(){return proc_config["subprocesses"][getProcessName()];}

    shared_ptr<zmq::context_t> getZMQContext(){return proc_context;}

    //////////////////////////////////////////////////

    void initSubprocess(int zmq_threads)
    {
        string str;
        Json::Reader reader;

        //Set the interrupt signal to tell subsystems to shutdown, rather than just end the program.
        //We want to give subsystems a chance to end any IO they're doing.
        signal(SIGINT, triggerShutdown);

        //Set the context, and initialize an empty JSON value.
        proc_context = shared_ptr<zmq::context_t>(new zmq::context_t(zmq_threads));

        //Retrieve the configuration info from stdin, and parse it to a JSON value.
        cin >> str;
        reader.parse(str, proc_config, false);
    };

    void runSubsystem(Subsystem *subsys){
        subsys->run();
    }

    void addSubsystem(Subsystem *subsys){
        proc_subsystems.push_back(subsys);
    }

    void runSubsystems()
    {
        vector<thread> threads;

        //Start a new subsystem thread for every subsystem.
        for(Subsystem *subsys : proc_subsystems)
            threads.push_back(thread(runSubsystem, subsys));

        //Join the threads once they have finished.
        for(thread &t : threads)
            t.join();
    };
};
