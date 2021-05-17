run-server:
	gcc -Wall -Wextra -g server/server.c -o server/server
	server/./server nes416_port


run-client-tcp:
	gcc -Wall -Wextra -g client/client.c -o client/client
	client/./client nes416_crypto_server nes416_port tcp

run-client-udp:
	gcc -Wall -Wextra -g client/client.c -o client/client
	client/./client nes416_crypto_server nes416_port udp
