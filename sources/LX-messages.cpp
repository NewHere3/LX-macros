#include "../headers/LX-messages.hpp"

#include <string.h>
#include <sstream>
#include <string>
#include <sys/time.h>
#include <iostream>
#include <iomanip>
#include <mutex>

Message::Message(std::string txt, char category, bool pause, bool* ptr_pause)
{
    strcpy(date, time().c_str());
    type = category;
    wait = pause;
    ptr_wait = ptr_pause;
    size = txt.length()+1; // +1 for null termination
    contents = (char*) malloc(size);
    memcpy(contents, txt.c_str(), size);
}

void Message::send(FILE* file)
{
    static std::mutex message_lock;
    static size_t num = 0;
    number = num;
    message_lock.lock();
    fwrite(this,sizeof(*this),1,file);
    fwrite(contents,size,1,file);
    fflush(file);
    ++num;
    message_lock.unlock();
}

void Message::read(FILE* file)
{
    fread(this, sizeof(*this), 1, file);
    contents = (char*) malloc(size);
    fread(contents, size, 1, file);
}

std::string Message::time()
{
    std::stringstream line;
    struct timeval tv;
    gettimeofday(&tv,NULL);
    struct tm* tm = localtime(&tv.tv_sec);
    char date[64] = {0};
    strftime(date, sizeof(date), "%Y-%m-%d %X ", tm);
    line << date << std::setw(6) << std::to_string(tv.tv_usec);
    return line.str();
}

void Message::print(FILE* file)
{
    fprintf(file, "|date:%s|number:%d|type:%d|wait:%d|ptr_wait:%d|size:%d|contents:%s|\n",
    date, number, type, wait, ptr_wait, size, contents);
    fflush(file);
}

Message::~Message()
{
    free(contents);
}

Message::Message(Message& M)
{
    strcpy(date, M.date);
    type = M.type;
    wait = M.wait;
    ptr_wait = M.ptr_wait;
    size = M.size; // +1 for null termination
    contents = (char*) malloc(size);
    memcpy(contents, M.contents, size);
}

Message::Message(Message&& M)
{
    strcpy(date, M.date);
    type = M.type;
    wait = M.wait;
    ptr_wait = M.ptr_wait;
    size = M.size; // +1 for null termination
    contents = M.contents;
    M.contents = NULL;
}