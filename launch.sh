#!/bin/bash
# A sample Bash script, by Ryan
#alacritty -e sleep 10 && echo hello
mkfifo "root >> user"
mkfifo "root << user"
make all
./LX-user &
sudo ./LX-macros

#x-terminal-emulator -e ./pipe
#rm "root >> user"
echo "script end!"
