#pragma once

void get_mouse_coordinates(int& x, int& y, Window window = root_window);

void pass_event_on(input_event event);


//inline void key_hold(int key, int m_sec);

inline void mouse_move(int x, int y);

inline void mouse_move(int x, int y, int speed);

void run_and_wait(std::string command);

void run_as_root(std::string command, bool wait);

void run_as_user(std::string command, bool wait);

void run(std::string command, bool root = 0, bool wait = 0);

void notify(std::string text, int timeout = 1000);