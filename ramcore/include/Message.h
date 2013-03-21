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
        /* Specifies the type of data encoded in the message. */
        enum {TYPE_INVALID = 0, TYPE_RAW = 1, TYPE_STRING = 2, TYPE_JSON = 3};

    private:
        /* Timestamp for when the message was created. */
        struct timeval msg_time;
        /* A number representing the type of the message, see the enum above. */
        int msg_type;
        /* The ZMQ message to send over the socket; contains all the data. */
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
