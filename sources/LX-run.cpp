#include "../headers/LX-run.hpp"
#include "../headers/LX-utilities.hpp"
#include "../headers/LX-constants.hpp"
#include "../headers/LX-messages.hpp"

#include <string>
#include <thread>
#include <iostream>


void execute(std::string command, GlobalVariables* Gvar)
{
    ++Gvar->unfinished_threadcount;
    system(command.c_str());
    --Gvar->unfinished_threadcount;
}

void run_as_root(std::string command, GlobalVariables* Gvar, bool wait_to_finish)
{
    if(wait_to_finish)
    {
        system(command.c_str()); 
        return;
    } else
    {
        std::thread t1(execute, command, Gvar);
        t1.detach();
        return;
    }
}

void run(std::string command, GlobalVariables* Gvar, bool wait_to_finish)
{
    //static FILE* file = fopen(ROOT_PIPE, "wb");
    if(wait_to_finish)
    {
        bool waiting = 1;
        Message message(command, USER_COMMAND, 1, &waiting);
        message.send(Gvar->root_pipe);
        while(waiting)
        {
            msleep(POLLING_RATE);
            std::cout << "waiting for:|"  << command << "|:" << std::endl;
        }
        std::cout << "finished:|"  << command << "|:" << std::endl;
    } else
    {
        Message message(command, USER_COMMAND);
        message.send(Gvar->root_pipe);
    }
}

void catch_message_return(GlobalVariables* Gvar)
{
    while (1)
    {
        //fread(&ptr, sizeof(ptr), 1, Gvar->user_pipe);
        //*ptr = 0;
        Message message;
        message.read(Gvar->user_pipe);
        std::cout << "READreturn!" << std::endl;
        if (message.type == LX_MESSAGE)
        {
            if(strcmp(message.contents, "closed")==0)
            {
                fclose(Gvar->user_pipe);
                std::cout << "LX-user closed!:" << std::endl;
                std::cout << "user_pipe closed!:" << std::endl;
                break;
            }
        } else if (message.type == FINISHED_COMMAND)
        {
            if(strcmp(message.contents, "finished")==0)
            {
                *message.ptr_wait = 0;
                std::cout << " message finised!:" << std::endl;
            }
        }
        else
        {
            std::cout << " return message not recognised!:" << std::endl;
            message.print();
        }
    }
}