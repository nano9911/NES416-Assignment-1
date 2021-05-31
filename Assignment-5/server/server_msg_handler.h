/**
 * @file server_msg_handler.h
 * @author Adnan Omar (JUST ID: 123423)
 * @brief server message handler file,
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

#define MSG_HANDLER_DEFINED

void msg_handler_thread_cleanup(void *argv)  {
    struct msg_handler_thread_arguments *args = 
        (struct msg_handler_thread_arguments *)argv;
    free(args);
//    return NULL;
}

/**
 * @brief message handler thread, which will check the primality of a given
 * number to a range of number
 * 
 * @param argv 
 * @return void* 
 */
void *msg_handler_thread(void *argv)    {
    struct msg_handler_thread_arguments *args = 
        (struct msg_handler_thread_arguments *)argv;

    pthread_cleanup_push(msg_handler_thread_cleanup, args);

    for (long i = args->from; i <= args->to; i++)    {
        if (args->num % i == 0) {
            *(args->rv) = i;
            break;
        }
    }

    pthread_exit(NULL);
    pthread_cleanup_pop(1);

    return NULL;
}

/**
 * @brief test primality of a given number, and return the
 * first occurence (if there's one)
 * 
 * @param num 
 * @param t 
 * @return int 
 */
int test_primality(long num, time_t *t,
                    pthread_t   *msgid)    {

    time_t      sec = time(NULL);
    long        from=0,
                to=(long)sqrt(num) / MAX_THREAD,
                thread_rv[MAX_THREAD];
    int         rv=1;

    /**
     * @brief msgargs is the pointer to arguments passed to the newly 
     * created thread  */
    struct msg_handler_thread_arguments *msgargs;

    /**  
     * @brief create multiple threads of count MAX_THREAD, each will
     * check the range given to it if the remainder of num
     * and the numbers in the range is ZERO */
    for (int i = 1; i <= MAX_THREAD; i++)   {
        msgargs = (struct msg_handler_thread_arguments *)
                    malloc(sizeof(struct msg_handler_thread_arguments));

        msgargs->num = num;
        msgargs->from = from;
        msgargs->to = to;
        msgargs->rv = &(thread_rv[i]);

        if (pthread_create(&(msgid[i]), NULL, msg_handler_thread,
            (void *)msgargs) != 0)    {
            perror("test_primality: pthread_create");
            break;
        }

        from = to + 1;
        /**
         * @brief to make sure we check from zero to sqrt(num) exactly
         * we put the last thread range to sqrt(num) by hand   */
        to = i == MAX_THREAD ? (long)sqrt(num) : to * i;
    }

    /**
     * @brief wait for created threads, and check returned values, if it's
     * not -1 it means that num isn't prime    */
    for (int i = 0; i < MAX_THREAD; i++)    {
        pthread_join(msgid[i], NULL);
    }

    /**
     * @brief get the minimum returned factor of num    */
    rv = thread_rv[0];
    for (long i = 0; i < MAX_THREAD; i++)   {
        rv = rv > thread_rv[i] ? thread_rv[i] : rv;
    }

    sec -= time(NULL);
    *t = sec;
    return rv;
}
