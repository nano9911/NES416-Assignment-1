# NES 416
## Assignment 1
### Adnan Omar @nano9911 (JUST ID:123423) and Roua Kofahi @rouakofahi (JUST ID: 126474)

This is the source code for our NES416 / Network Programming course Assignment 1.

It's about a client server model, where the server is iteravtive, and receive messages of
a simple mathmatic equations check it, and return the result, the connection ends when the client
sends "exit". The client program check the input given from user, then send it to server waiting
fo the result, and finnaly present it to user.

The Client read the command line arguments **(argv)** which shoud be **server's ip and port number**.
In addition the client should use **bind()** to port **55abc**, where **abc** is the least
significant 3 digits of our students ID.

The server also read the command line arguments **(argv)** to indicate the port it will listen to.

Finally, both server and client once receive a message from the other it should print
the **sender ip and port number**.



### Quoting from Dr.Raed documet:

> Output Sample:
> 
> For example, running the code should produce something similar to this output:

>   Client_side> please enter your expression:

>   1 + 2 (input from a user)

>   Client_side > 1+2 = 3 (server IP , Port#), where 3 is the answer received from the server

>   Client_side > please enter your expression:

>   exit (input from a user)

>   Client_side > exiting (then the client side exits, and also the server side)

>   Server_side > waiting for client messages.

>   Sever_side > received "1 + 2" from the client (IP address, port #)

>   Server_side > Sending " 3 " to the clientServer_side > waiting for client messages

>   Server_side > received "exit" from the client (IP address, port #)

>   Server_side > exiting ( then the server exits)
