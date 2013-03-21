#include "Message.h"

using namespace std;

namespace ramcore
{
    Message::Message() : msg_type(Message::TYPE_INVALID) {}
    
    template<typename T>
    Message::Message(const T &obj) : msg_type(Message::TYPE_RAW)
    {
        msg = zmq::message_t(sizeof(T));
        memcpy(msg.data(), &obj, msg.size());
        gettimeofday(&msg_time, NULL);
    }

    Message::Message(const std::string &str) : msg_type(Message::TYPE_STRING)
    {
        msg = zmq::message_t(str.size());
        memcpy(msg.data(), str.data(), msg.size());
        gettimeofday(&msg_time, NULL);
    }

    Message::Message(const Json::Value &val) : msg_type(Message::TYPE_JSON)
    {
        Json::FastWriter writer;
        std::string str = writer.write(val);

        msg = zmq::message_t(str.size());
        memcpy(msg.data(), str.data(), msg.size());
        gettimeofday(&msg_time, NULL);
    }

    //////////////////////////////////////////////////

    template<typename T>
    bool Message::getObject(T &obj)
    {
        if(msg_type != TYPE_RAW || msg.size() != sizeof(T))
            return false;

        memcpy(&obj, msg.data(), msg.size());
        return true;
    }

    bool Message::getString(std::string &str)
    {
        if(msg_type != TYPE_STRING)
            return false;

        str = std::string((char *)msg.data(), msg.size());
        return true;
    }

    bool Message::getJSON(Json::Value &val)
    {
        if(msg_type != TYPE_JSON)
            return false;

        Json::Reader reader;
        return reader.parse((char *)msg.data(), (char *)msg.data() + msg.size() - 1, val, false);
    }
    
    //////////////////////////////////////////////////

    struct timeval Message::getTimestamp(){return msg_time;}
    int Message::getType(){return msg_type;}
    size_t Message::getSize(){return msg.size();}
    
};
