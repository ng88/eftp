#!/bin/bash

[ -z "$1" ] || touch "$1" > /dev/null 2>&1

if [ -z "$1" -o ! -w "$1" ]
then
    echo "usage: $0 user_file" >&2
    exit 2
fi

read -p "Login: " login

stty -echo
read -p "Password: " pass; echo
stty echo

echo :$login:$(echo -n $pass | md5sum | cut -f1 -d" "):: >> $1

echo Added.
