#include "Message.h"

using namespace std;

namespace ramcore
{
    Message::Message() : msg_type(Message::TYPE_INVALID) {}
    
    template<typename T>
    Message::Message(const T &obj) : msg_type(Message::TYPE_RAW)
    {
        //Create a new ZMQ message with the correct amount of bytes, then copy the given object to the message.
        msg = zmq::message_t(sizeof(T));
        memcpy(msg.data(), &obj, msg.size());
        gettimeofday(&msg_time, NULL);  //Set the timestamp, as always.
    }

    Message::Message(const std::string &str) : msg_type(Message::TYPE_STRING)
    {
        //Create a new ZMQ message, then copy over the raw string data.
        msg = zmq::message_t(str.size());
        memcpy(msg.data(), str.data(), msg.size());
        gettimeofday(&msg_time, NULL);
    }

    Message::Message(const Json::Value &val) : msg_type(Message::TYPE_JSON)
    {
        //Write the JSON value to a string using a compact format, so it's faster to parse.
        Json::FastWriter writer;
        std::string str = writer.write(val);

        //Create a new ZMQ message, and copy over the raw data for the created string.
        msg = zmq::message_t(str.size());
        memcpy(msg.data(), str.data(), msg.size());
        gettimeofday(&msg_time, NULL);
    }

    //////////////////////////////////////////////////

    template<typename T>
    bool Message::getObject(T &obj)
    {
        //If the message is the wrong type, or is the wrong size, abort.
        if(msg_type != TYPE_RAW || msg.size() != sizeof(T))
            return false;

        //Copy the raw data into the object.
        memcpy(&obj, msg.data(), msg.size());
        return true;
    }

    bool Message::getString(std::string &str)
    {
        //If the message is the wrong type, abort.
        if(msg_type != TYPE_STRING)
            return false;

        //Copy the raw data into the string.
        str = std::string((char *)msg.data(), msg.size());
        return true;
    }

    bool Message::getJSON(Json::Value &val)
    {
        //If the message is the wrong type, abort.
        if(msg_type != TYPE_JSON)
            return false;

        //Parse the raw data into the given JSON value.
        Json::Reader reader;
        return reader.parse((char *)msg.data(), (char *)msg.data() + msg.size() - 1, val, false);
    }
    
    //////////////////////////////////////////////////

    struct timeval Message::getTimestamp(){return msg_time;}
    int Message::getType(){return msg_type;}
    size_t Message::getSize(){return msg.size();}
    
};
