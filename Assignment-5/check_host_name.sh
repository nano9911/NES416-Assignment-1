#!/bin/bash

host="nes416_prime_server"
ip=""

echo "Enter the target (server) ip address:\b"
read ip

if grep -xq "$ip"$'*[\t]*'"$host" /etc/hosts
then
    echo "($ip) exists with hostname:($host) in /etc/hosts"

else
    if [[ $(id -u) -ne 0 ]];
    then
        echo "Please run as root"
        echo "Failed to append to /etc/hosts"
    else
        echo "" >> /etc/hosts
        echo "$ip	$host" >> /etc/hosts
        echo "($ip) with hostname:($host) added to /etc/hosts"
    fi
fi

exit 0
