 /**
 * @file message_handler.h
 * @author Adnan Omar (JUST ID: 123423)
 * @brief Messages handler header (all from scratch) for NES416/HW4
 * @date 2021-05-02
 * @version 0.1
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#define crypt(which,var1,var2)  ((which) == (0) ? (var1) : (var2))

/* Enumeration values to make the operation more understandable */
enum operations {ERR=-1, ENC, DEC, EXIT, LIST};

int decode_msg(char *msg, size_t len, char *text, int *textlen)  {
    int op=ERR, ptr=2;

    op = (int)msg[0] - 48;

    if (op == ENC || op == DEC) {
        *textlen = (int)msg[1];
        if ((*textlen)+2 > len)
            return -2;

        for (int i = 0; ptr < ((*textlen)+2); ptr++, i++)
        {text[i] = msg[ptr];}
    }

    if (op != LIST || op != EXIT) op = ERR;

    return op;
}


int cipher(char *msg, int msglen, int op)    {
    int rv;
    char key[7];

    if (fgets(key, 7, stdin) == -1)   {
        perror("cipher: fgets");
        return -1;
    }

    return 0;
}
