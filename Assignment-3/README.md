# NES 416 Assignment #2
>[Link to project](https://github.com/nano9911/NES416-Assignments/tree/main/Assignment-3)

**By:**
- Adnan Omar [@nano9911](https://github.com/nano9911) (JUST ID:123423)
- Roua Kofahi [@rouakofahi](https://github.com/rouakofahi) (JUST ID: 126474)

This is the source code for our NES416 / Network Programming course Assignment #3.

## Overview
Console program that create a socket and print the default values of the socket options. Furthermore, it
should take two integers from user and assign them to SO_SNDLOWAT and SO_RCVLOWAT options.

## Requirments
Attach screenshots of running program output. User input in screenshots should make sense.

## Important Notes
### Methods Used to Make a Socket Non-Blocking
I have written 3 methods to put a socket in Non-Blocking mode, the socket is created to change it's SO_SNDLOWAT and SO_RCVLOWAT options.
The 3 methods are:
#### 1. Using SOCK_NONBLOCK type with SOCK_STREAM in socket() function.
#### 2. Using fcntl() with F_SETFL function
There's two cases here for the option:
- Linux and FreeBSD, using O_NONBLOCK.
- Solaris, using FNDELAY.
#### 3. Using ioctl() function
There's two cases here for the option to be setted to 1:
- Linux and FreeBSD, using FIONBIO.
- Solaris, using \_IONBF.

### SO_SNDLOWAT & SO_RCVLOWAT
#### Linux
SO_SNDLOWAT is defined in linux but isn't changeable by setsockopt() and will return an error with errno 92 "Protocol not available".
SO_RCVLoWAT is defined and changeable on linux from kernel 2.4 and later. [Source here] (https://linux.die.net/man/7/socket)
#### FreeBSD
Both options are defined and changeable, check the screenshots from the proof of work file.
#### Solaris
I didn't find any refrence to confirm that both options aren't defined on Solaris 11, but from my test runs, it always return error "Options not supported by protocol".

## Quoting from Dr.Raed documet:
> The output of your program should first display the name of your operating system, release and version.
> 
> Then, you need to display the name of the option, and its default value for all options.
> 
> Furthermore, if the option is not supported, you need to indicate that also.
> 
> At the end you should ask users to enter values for the receive low-water mark and the send low-water mark
> 
> for a TCP socket, modify them, and display the new values on the screen. An example showing only the option
> 
> setting might look like:

> * Prompt$ Please enter new values for the receive low-water mark and the send low-water mark for TCP: *
>
> * xx yy (where xx yy are user input such that xx is the send low-water mark and yy is the receive low-water mark) *
>
> * Modified send low-water mark (TCP): xx *
>
> * Modified receive low-water mark (TCP): yy *
