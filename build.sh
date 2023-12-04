#!/bin/bash

set -xe

#g++ main.cpp -o proc `pkg-config --cflags opencv4` `pkg-config --libs opencv4`
cc main_client.c -o bin_client -lm
cc main_server.c -o bin_server
