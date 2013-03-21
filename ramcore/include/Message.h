#pragma once

#include <string>
#include <zmq.hpp>
#include <sys/time.h>
#include <json/json.h>

using namespace std;

namespace ramcore
{
    class Message
    {
    public:
        enum {TYPE_INVALID = 0, TYPE_RAW = 1, TYPE_STRING = 2, TYPE_JSON = 3};

    private:
        struct timeval msg_time;    
        int msg_type;
        zmq::message_t msg;

    public:
        Message();

        template<typename T>
        Message(const T &obj);
        Message(const std::string &str);
        Message(const Json::Value &val);

        template<typename T>
        bool getObject(T &obj);
        bool getString(std::string &str);
        bool getJSON(Json::Value &val);

        struct timeval getTimestamp();
        int getType();
        size_t getSize();
    };
};
