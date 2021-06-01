#!/bin/bash
g++ -g -c main.cpp
g++ -g main.o -o double_pendulum -lsfml-graphics -lsfml-window -lsfml-system
./double_pendulum

