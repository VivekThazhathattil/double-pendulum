#!/bin/bash
g++ -c main.cpp
g++ main.o -o double_pendulum -lsfml-graphics -lsfml-window -lsfml-system
./double_pendulum

