//
//  Message.hpp
//  Banking_system
//
//  Created by Pietro Lenti on 23/11/2015.
//  Copyright © 2015 Pietro Lenti. All rights reserved.
//

#ifndef Message_hpp
#define Message_hpp

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>

//This class wraps the messages that nodes exchange. Each message is composed of an header and a core. The header is encrypted
//before the message is sent and decryptes when it is received. The core contains the content of the message.
class Message
{
public:
    enum { header_length = 4 };
    enum { max_body_length = 512 };
    
    Message()
    : body_length_(0)
    {
    }
    
    const char* data() const
    {
        return data_;
    }
    
    char* data()
    {
        return data_;
    }
    
    std::size_t length() const
    {
        return header_length + body_length_;
    }
    
    const char* body() const
    {
        return data_ + header_length;
    }
    
    char* body()
    {
        return data_ + header_length;
    }
    
    std::size_t body_length() const
    {
        return body_length_;
    }
    
    void body_length(std::size_t new_length)
    {
        body_length_ = new_length;
        if (body_length_ > max_body_length)
            body_length_ = max_body_length;
    }
    
    bool decode_header()
    {
        char header[header_length + 1] = "";
        std::strncat(header, data_, header_length);
        body_length_ = std::atoi(header);
        if (body_length_ > max_body_length)
        {
            body_length_ = 0;
            return false;
        }
        return true;
    }
    
    std::string to_string()
    {
        std::string line(data_ + header_length, body_length_);
        return line;
    }
    void encode_header()
    {
        char header[header_length + 1] = "";
        std::sprintf(header, "%4lu", body_length_);
        std::memcpy(data_, header, header_length);
    }
    
private:
    char data_[header_length + max_body_length];
    std::size_t body_length_;
};

#endif /* Message_hpp */
