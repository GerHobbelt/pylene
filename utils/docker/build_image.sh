#!/bin/bash

scp ../../conanfile.py .
docker build -t carlinet/buildimage .
