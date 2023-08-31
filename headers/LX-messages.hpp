#pragma once
#include "LX-constants.hpp"

#include <string>

struct Message
{
    char date[MESSAGE_DATE_SIZE] = {0}; //2023-07-21 07:53:39 881485 -- date, time, microseconds
    size_t number = 0; // i'd like to make it atomic static , need to check if reading and writing doesn't break
    char type = 0;
    bool wait = false;
    bool* ptr_wait = NULL;
    size_t size = 0;
    char* contents = NULL;
    Message(std::string txt = "", char category = 0, bool pause = 0, bool* ptr_pause = NULL);
    Message(Message& M);
    Message(Message&& M);
    ~Message();
    void send(FILE* file);
    void read(FILE* file);
    void print(FILE* file = stdout);
    std::string time();
};