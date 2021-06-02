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

    long *rv = (long *)malloc(sizeof(long));
    *rv = -1;

    for (long i = args->from; i <= args->to; i++)    {
        if ((args->num % i) == 0) {
            *rv = i;
            break;
        }
    }

    return rv;
}

/**
 * @brief test primality of a given number, and return the
 * first occurence (if there's one)
 * 
 * @param num 
 * @param t 
 * @return int 
 */
int test_primality(long num, pthread_t   *msgid)    {
    long            from=2,
                    block=((long)sqrt((double)num)) / MAX_THREAD,
                    to=block,
                    thread_rv[MAX_THREAD];

    long            rv=-1;
    void            *rt;

    /**
     * @brief msgargs is the pointer to arguments passed to the newly 
     * created thread  */
    struct msg_handler_thread_arguments msgargs[MAX_THREAD];

    /**  
     * @brief create multiple threads of count MAX_THREAD, each will
     * check the range given to it if the remainder of num
     * and one of the numbers in the range is ZERO */
    for (int i = 0; i < MAX_THREAD; i++)   {
        msgargs[i].num = num;
        msgargs[i].from = from;
        msgargs[i].to = to;

        if (pthread_create(&(msgid[i]), NULL, msg_handler_thread,
                                    (void *)&(msgargs[i])) != 0)    {
            perror("test_primality: pthread_create");
            break;
        }

        from = to + 1;
        /**
         * @brief to make sure we check from zero to sqrt(num) exactly
         * we put the last thread range to sqrt(num) by hand   */
        to = i == (MAX_THREAD-1) ? (long)sqrt((double)num) : block * (i+1);
    }

    rv = -1;
    /**
     * @brief wait for created threads, and check returned values, if it's
     * not -1 it means that num isn't prime    */
    for (int i = 0; i < MAX_THREAD; i++)    {
        pthread_join(msgid[i], &rt);
        thread_rv[i] = *((long *)rt);
        free(rt);

        if (thread_rv[i] > rv)
            rv = thread_rv[i];
    }

    /**
     * @brief get the minimum returned factor of num    */
    for (int i = MAX_THREAD; i >= 0; i--)   {
        if (thread_rv[i] < rv && thread_rv[i] > 1)
            rv = thread_rv[i];
    }

    return rv;
}
