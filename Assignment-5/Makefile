CC=gcc -Wall -Wextra -pthread
OPTIONS=-Wall -Wextra -pthread
PORT=5987
HOST=nes416_prime_server
IP=

run-server:
	${CC} ${OPTIONS} server/server.c -o server/server -lm
	server/./server ${PORT}

run-client:
	${CC} client/client.c -o client/client
	client/./client ${HOST} ${PORT}

add-host:
	chmod 0755 check_host_name.sh
	sudo ./check_host_name.sh
