# NES 416 Assignment #1
>[Link to project](https://github.com/nano9911/NES416-Assignments)

**By:**
- Adnan Omar [@nano9911](https://github.com/nano9911) (JUST ID:123423)
- Roua Kofahi [@rouakofahi](https://github.com/rouakofahi) (JUST ID: 126474)

This is the source code for our NES416 / Network Programming course Assignment #2.

## Overview
It's about a client server model, where the server is concurrent (fork() model), and receive messages of
a simple mathmatic equations check it, and return the result, the connection ends when the client
choose "exit". The client program check the input given from user, then send it to server waiting
for the result, and finnaly present it to user.

## Requirments
### Client
The Client reads the command line arguments **(argv)** which shoud be **server's ip and port number**.
Then connect to server and receive the menu of operations the server offers, and show to the user
in an appropriate way to let him choose. The user should firstly input his choice, then th expression
he had. If the **"Compute GPA"** is his choice, he should provide a list of each mark with it's hours
separated with commas.
Finally the client put send the choice at the first byte of the message then the expression (without
anything but spaces btween them)
### Server
The server also read the command line arguments **(argv)** to indicate the port it will listen to.
Firstly, when the parent accepts a connection it should starts the child process, and loop to accept().
The child immediately sends the menu to the client, and wit for the message of (choice + expression) to
extract it.

### Finally, both server and client once receive a message from the other, it should print the **sender ip and port number** with it.


## Quoting from Dr.Raed documet:
>
> The functionality starts by the child server sending the client a menu which contains function to
> choose from. The menu should look like the following
>   1) Add
>   2) Subtract
>   3) Divide
>   4) Multiply
>   5) Compute GPA
>   6) Exit
>
> After receiving the menu, the client displays it on the screen waiting for user input (selection).
>
> For example, if the user selects 1 (first choice in the menu), it means add two integers. The user has
> to enter the two integers as done in HW#1, i.e. 3 + 5, and ignoring any number of extra spaces. The
> child server should perform the required operation and sends result back to the client, which is
> displayed there. Furthermore, the child server sends the menu again to be displayed at the client side
> for the user to enter his/her choice again, until 6 is selected at which time both the client and child
> server exits appropriately, where the parent prints the process ID of the terminated child.
>
>
> For the GPA (Grade Point Average) calculation, you need to send the server a comma separated
> list containing the course grade (out of 100) and number of credit hours for that course.
> For example, if you send: 
> 80,3,90,1,65,3,70,2, -1 
> this means that you need to compute the GPA as the following, where -1 indicates the end of the list
> GPA= (80*3 + 90*1 + 65*3 + 70*2)/9 = 73.88
>
> Note that in all cases, the parent server is kept running.
>
