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

namespace ramcore{

    class Subsystem{
    private:
        int interval = 0;
        Json::Value config;
    public:
        void setUpdateInterval(int milliseconds);
        void run();

        virtual void onSetup(){}
        virtual void onUpdate(){}
        virtual void onShutdown(){}
    };

    extern volatile bool proc_shutdown;
    extern Json::Value proc_config;

    extern vector<Subsystem *> proc_subsystems;

    extern shared_ptr<zmq::context_t> proc_context;

    bool isShutdown();
    void triggerShutdown(int param = 0);

    string getProcessName();
    string getTopDirectory();
    string getProgramDirectory();

    Json::Value getConfiguration();

    shared_ptr<zmq::context_t> getZMQContext();

    void initSubprocess(int zmq_threads = 1);
    void runSubsystem(Subsystem *subsys);

    void addSubsystem(Subsystem *subsys);
    void runSubsystems();
};
