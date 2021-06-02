# NES 416 Assignment #5
>[Link to project](https://github.com/nano9911/NES416-Assignments/tree/main/Assignment-5)

**By:**
- Adnan Omar [@nano9911](https://github.com/nano9911) (JUST ID:123423)

This is the source code for NES416 / Network Programming course Assignment #5.

## Overview
A multithreaded tcp client server model using pthread in POSIX systems.
The server is for primality testing, where it will recive requests with only a number in it,
create a thread to handle it, and it's required to reponse back to the client with the
result (prime or not) and the time it took it to decide. The client will take user's input
(which should be a positive integer) and send two requests to the server, each request on
a different thread, one request contains user's input, the other contains user input incremented
by 1.

## Compiling and Running
To run the server just use:
> make run-server

For the client part it will use nes416_prime_server hostname as a traget and argument for client binary, so to make sure everything works fine run:
> make add-host

which should run the ***check_host_name.sh*** script which will check if the required hostname is present in the /etc/hosts file with the appropriate ip user inputed, and if not it will be added properly

then to run the client you could use:
> make run-client

for compiling then running

## Notes
I tried hardly to make the code well organised and weel documented,
so you find two folders (server and client):
### Server
Quoting from Dr.Raed Document:
> The server receives a request containing some integernumberthat need to be checked. For each
> number, the server creates a core thread. However, instead of doing all calculations and checking
> in this core thread, and in order to enhance the performance, the functionality is distributed
> multiple threads. That is, the core threadshould create MAX_THREADnumber of threads and divide the
> work amongst those threads. Simply, you can divide the work equally such that each thread gets 
> sqrt(n)/ MAX_THREADS number of potential factors to check against. Each of the threads should either
> return the first factor or -1 if no factor is found. In additionto waiting for the threads to finish,
> the core thread should calculates/measure how much time was elapsed until a decision was made whether
> a given number is prime or not, and reportthe decision and the time to the client.after that, all
> associated threads with the client request terminates, but the server continue running waiting for more requests.

The "server" directory contains 5 Files:
- **server.c**:
  - main()
  - add_thread()
- **server_threads.h**:
  - client_handler_thread()
  - client_handler_thread_cleanup()
- **server_msg_handler.h**:
  - test_primality()
  - msg_handler_thread()
- **server_sock_handler.h**:
  - get_tcp_socket()
- **server_wrappers.h**:
  - Send()
  - Recv()
  - signal_handler()
  - Contains the defnition of 3 important structs:
    - *client_handler_thread_arguments*: to pass the required arguments from the main() to the client_handler_thread() function thread
    - *msg_handler_thread_arguments*: to pass the required arguments from test_primality() to msg_handler_thread()
    - *threads*: to organise a doubly linked list of running threads, which is added by add_thread(), and deleted at client_handler_thread_cleanup() for each terminated thread

### Client
Quoting from Dr.Raed Document:
> your client is supposed to be multithreaded. The client’s main thread will be responsible for reading
> user input, and for each entered number, two threads are created: the first one will ask the server to
> check the same entered number, and the second one will ask the server to check the entered number plus
> one.  For example, if the user entered the number 100, the first thread will send the server the
> number 100, and the second thread will send the number 101 for checking. The client waits for both replies
> before displaying the results and exits.

The "client" directory contains 4 Files:
- **client.c**:
  - main()
- **client_threads.h**:
  - client_thread()
  - thread_cleanup()
- **client_sock_handler.h**:
  - get_socket()
- **client_wrappers.h**:
  - Send()
  - Recv()
  - signal_handler()
  - Contains the defnition of 2 important structs:
    - *thread_arguments*: to pass the required arguments from the main() to the client_thread() function thread
    - *thread_cleanup_arguments*: to pass the required arguments from client_thread() thread to the cleanup routine thread_cleanup()

### Primality and other options
Quoting from Dr.Raed Document:
> Conceptually, to check if a number “n” is prime (or to find a factor of a number), you have to see if all numbers
> between 2 and sqrt(n)are a factor of n. Furthermore,your code should measure the time the detection process takes.

