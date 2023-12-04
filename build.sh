#!/bin/bash

set -xe

cc main_client.c -o bin_client -lm
cc main_server.c -o bin_server
