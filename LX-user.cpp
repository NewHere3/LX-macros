#include "headers/LX-messages.hpp"
#include "headers/LX-constants.hpp"
#include "headers/LX-utilities.hpp"

#include <string>
#include <thread>
#include <mutex>
#include <atomic>
#include <iostream>

#include <stdio.h>

std::atomic<int> unfinished_threadcount;


void UserCommand(Message&& message, FILE* file)
{
    ++unfinished_threadcount;
    system(message.contents);
    if (message.wait)
    {
        Message return_message("finished", FINISHED_COMMAND, 1, message.ptr_wait);
        return_message.send(file);
    }
    --unfinished_threadcount;
}

int LXCommand(std::string command)
{
    if (command == "close")
    {
        return 1;
    }
    else if (command == "exit")
    {
        exit(0);
    }
    else
    {
        std::cout << "error: " << command << std::endl;
        return 0;
    }
}

int main()
{
    FILE* root_pipe = fopen(ROOT_PIPE, "rb");
    std::cout << "root_pipe opened!" << std::endl;
    FILE* user_pipe = fopen(USER_PIPE, "wb");
    std::cout << "user_pipe opened!" << std::endl;

    while(1)
    {
        Message message;
        message.read(root_pipe);
        std::cout << "EXE" << std::endl;
        if(message.type == USER_COMMAND)
        {
            std::thread t1(UserCommand, std::move(message), user_pipe);
            t1.detach();
        }
        else if (message.type == LX_MESSAGE)
        {
            if(LXCommand(message.contents))
            {
                break;
            }
        }
        else
        {
            std::cout << "message not recognised!:" << std::endl;
            message.print();
        }
    }

    while(unfinished_threadcount>0)
    {
        std::cout << "Waiting for all threads to finish (" << unfinished_threadcount << " left)" << std::endl;
        msleep(100);
    }

    fclose(root_pipe);
    Message M("closed", LX_MESSAGE);
    M.send(user_pipe);
    fclose(user_pipe);
    return 0;
}



 
