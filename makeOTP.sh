#!/bin/bash
FILE="otp.key"

if [ ! -f $FILE ]; then
    head -c 1G </dev/urandom >otp.key
fi
