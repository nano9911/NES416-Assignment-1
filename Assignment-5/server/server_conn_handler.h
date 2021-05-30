/**
 * @file server_msg_handler.h
 * @author Adnan Omar (JUST ID: 123423)
 * @brief server message and threads handler file, will be linked with server.c
 * @version 0.1
 * @date 2021-05-29
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#pragma once

#ifndef WRAPPER_INCLUDED
#include "server_wrappers.h"
#endif /*ifndef WRAPPER_INCLUDED*/

#define MAX_THREAD 3
#define CONN_HANDLER_DEFINED

extern void *msg_thread(void *argv);

int test_primality(long num, time_t *t)    {
    time_t      sec = time(NULL);
    long        from=0,
                to=(long)sqrt(num) / MAX_THREAD;
    void        *rt;
    int         rv=1;
    pthread_t   msgid[MAX_THREAD];

    /*  create multiple threads of count MAX_THREAD, each will
        check the range given to it if the remainder of num
        and the numbers in the range is ZERO    */
    for (int i = 1; i <= MAX_THREAD; i++)   {
        struct msg_thread_arguments *msgargs = 
            (struct msg_thread_arguments *)malloc(sizeof(struct msg_thread_arguments));

        msgargs->num = num;
        msgargs->from = from;
        msgargs->to = to;

        if (pthread_create(&(msgid[i]), NULL, msg_thread,
            (void *)msgargs) != 0)    {
            perror("test_primality: pthread_create");
            break;
        }

        from = to + 1;
        /*  to make sure we check from zero to sqrt(num) exactly
            we put the last thread range to sqrt(num) by hand   */
        to = i == MAX_THREAD ? (long)sqrt(num) : to * i;
    }

    /*  wait for created threads, and check returned values, if it's
        not -1 it means that num isn't prime    */
    for (int i = 0; i < MAX_THREAD; i++)    {
        pthread_join(msgid[i], &rt);
        if (*(int *)rt !=-1)    rv = -1;
    }

    sec -= time(NULL);
    *t = sec;
    return rv;
}
