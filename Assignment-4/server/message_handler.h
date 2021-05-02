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

/* Enumeration values to make the operation more understandable */
enum operations {ERR=-1, ENC, DEC, EXIT, LIST};

/**
 * @brief message decoder: extract client choice and msg (in case 
 * of Encryption and Decryption)
 * 
 * @param msg recived message from client
 * @param len message length (return value from recv or recvfrom)
 * @param text will contain the extracted message in case of Encryption
 * or Decryption
 * @param textlen returned message length in 'text'
 * @return int -> user choice
 */
static int decode_msg(char *msg, size_t len, char *text, int *textlen)  {
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

/**
 * @brief subfunction from cipher() to handle the operation per
 * character level
 * 
 * @param org original character from message
 * @param k matching key for this character position in message
 * @param op user choi
 * @return chaice (ENC: Enryption or DEC: Decryption)
 */
static char char_move_around(char org, char k, int op)   {
    int v1=(int)org-97, v2=(int)k-97;
    char res='\0';
    
    if (op == ENC)  res = (char)((v1 + v2)%26);
    if (op == DEC)  res = (char)((v1 - v2)%26);

    return res;
}

/**
 * @brief Encryption or Decryption function, which will key for cipher
 * from user input, then encrypt or decrypt the message depending on
 * user choice.
 * 
 * @param msg client user message
 * @param msglen client user message length
 * @param op client user choice
 * @return int: -1 in case of fgets failed, 0 if succeeded
 */
static int cipher(char *msg, int msglen, int op)    {
    int rv;
    char key[7];

    while (1)   {
        memset(key, 0, 7);
        /* get key user input */
        if (fgets(key, 7, stdin) == -1)   {
            perror("cipher: fgets");
            return -1;
        }
        /* check if new key length */
        else if (strlen(key) != 7)  {
            printf(stderr, "invlalid key: should be 7 small case \
characters.\n");
            continue;
        }

        for (int i = 0; i < 7; i++) {
            if (!(key[i] >= 'a' && key[i] <= 'z'))  {
                printf(stderr, "invlalid key: should be 7 small \
case characters.\n");
                continue;
            }
        }
    }

    for (int i = 0; i < msglen; i++)    {
        msg[i] = char_move_around(msg[i], key[i%7], op);
    }

    return 0;
}
