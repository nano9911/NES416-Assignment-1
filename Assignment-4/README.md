# NES 416 Assignment #4
>[Link to project](https://github.com/nano9911/NES416-Assignments/tree/main/Assignment-4)

**By:**
- Adnan Omar [@nano9911](https://github.com/nano9911) (JUST ID:123423)

This is the source code for NES416 / Network Programming course Assignment #4.

## Overview
Client and Server with substitute cipher/cryptosystem service.
The server should implement both:
- TCP MultiProcessing.
- UDP Concurrent.

And listen on both of them (by multiplexing).

The client should also offer both options (TCP or UDP) for user to use when communicating with server. And offer this menu to user:
- Encrypt a message.
- Decrypt a message.
- Exit.
- Listing files in the current directory on the server.

## Notes
I tried hardly to make the code well organised, so you find two folders (server and client):
### Server
To run server you can use the Makefile by this command:
>   make run-server

Where the code will be compiled with full check for errors and warnings by:
> 	gcc -Wall -Wextra -g server/server.c -o server/server
Then will be executed by:
>	server/./server nes416_port

- server.c is the main file:
    1. main().
    2. signal_handler().

- server_conn_handler.h is responsible to handle coming connections and direct them to the appropriate function:
    1. handle_message().
    2. udp_conn().
    3. tcp_conn().
    4. tcp_client_handler().

- server_socket_handler.h is responsibe for socket creation, so it won't be used but once at the startup:
    1. get_socket().
    2. create_socket().

- server_message_handler.h is responsilbe for decoding client messages and pass them to the write operator:
    1. decode_msg().
    2. char_move_around().
    3. get_key().
    4. cipher().
    5. list_directory().

- server_wrappers.h is just a colection of all public variables, wrapper functions and libraries:
    1. wait_children_and_exit().
    2. Recv().
    3. Send().
    4. Recvfrom().
    5. Sendto().
    6. Accept().
    7. Signal().

### Client
To run client in TCP mode you can use the Makefile by this command:
>   make run-client-tcp
Where the code will be compiled with full check for errors and warnings by:
>	gcc -Wall -Wextra -g client/client.c -o client/client
Then will be executed by:
>	client/./client nes416_crypto_server nes416_port tcp

or by UDP mode:
>   make run-client-udp
Where the code will be compiled with full check for errors and warnings by:
>	gcc -Wall -Wextra -g client/client.c -o client/client
Then will be executed by:
>	client/./client nes416_crypto_server nes416_port udp


- client.c is the main file, which handles the send and receive loop, and encode user choice and message:
    1. signal_handler().
    2. main().
    3. get_user_choice().
    4. create_message().

- client_socket_hanlder.h is used to create the appropriate socket depending on user choice:
    1. get_socket().
    2. create_socket().

- client_message_handler.h is responsible for getting user message and chloce to process to it and pass it back for the main file for encoding before sending:
    1. char_move_around().
    2. get_key().
    3. cipher().

- client_wrappers.g is just a colection of all public variables, wrapper functions and libraries:
    1. check_input_and_return_choice().
    2. Send().
    3. Recv().

### Crypto system
#### Quoting from Dr.Raed Document:
> The cipher replaces one character to another based on some key. The input message (plaintext) can be of any size
> but composed only of small-letter (abcdefghijklmnopqrstuvwxyz). The output (ciphertext) has the same size
> and composed also of small-letter characters. The key is limited to 7 characters (also small-letter). You need to
> treat each digit as a number according to its position in the alphabet. For example, a=0, b=1, …,z=26. To
> encrypt a message, you add a plaintext character with the corresponding key character (mod 26). The key
> might be repeated to match the length of the plaintext message. In the decryption, you subtract the
> corresponding key character from the ciphertext character (mod 26). Again the key might be repeated as
> necessary.

### Encryption option
#### Quoting from Dr.Raed Document:
> If the user selects 1, the encryption must be implemented by the client. The client asks the user to enter a
> plaintext message (NULL terminated) and a key. Then it encrypts the message and sends it to the server. The
> server, which implements the decryption, reads the encrypted message, asks the user to enter the key, then
> sends the plaintext message to the client. Then, the client prints the received message on standard output
> together with the IP address and port number of the server.

### Decryption option
#### Quoting from Dr.Raed Document:
> If the user selects 2, the decryption must be implemented by the client. The client asks the user to enter a
> ciphertext message (NULL terminated) and a key. Then it decrypts the message and sends it to the server. The
> server, which implements the encryption in this case, reads the decrypted message, asks the user to enter the
> key, then sends the corresponding ciphertext message to the client. Then, the client prints the received message
> on standard output together with the IP address and port number of the server.

### Force Exit for server from client side
#### Quoting from Dr.Raed Document:
> The connection between your client and server should stay open, so that the client can repeat the operation
> again. You should also add a logout or stop type of a command so that the client can tell the server that it is
> finished and the session should end. For example, if the user enters --, the client and server program terminates.
> Note that the server displays the IP address and port number of each client request
