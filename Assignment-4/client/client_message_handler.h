/**
 * @file client_message_handler.h
 * @author Adnan Omar (JUST ID: 123423)
 * @brief Messages handler header (all from scratch) for NES416/HW4
 * @date 2021-05-02
 * @version 0.1
 * 
 * @copyright Copyright (c) 2021
 * 
 */

/* Enumeration values to make the operation more understandable */
enum operations {ERR=-1, ENC=1, DEC=2, EXIT=3, LIST=4};

/**
 * @brief subfunction from cipher() to handle the operation per
 * character level
 * 
 * @param org original character from message
 * @param k matching key for this character position in message
 * @param op user choi
 * @return chaice (ENC: Enryption or DEC: Decryption)
 */
char char_move_around(char org, char k, int op)   {
    int v1=((int)org)-97, v2=((int)k)-97, r;
    char res='\0';
    
    if (op == ENC)      res = (char)(((v1 + v2)%26) + 97);
    else if (op == DEC) {
        /*  In case of negative numbers we should make sure it's a
        *   true modulo not just a remaider */
        r = (((v1 - v2) % 26) + 26) % 26;
        res = ((char)r) + 97;
    }

    return res;
}

/**
 * @brief Get the key object
 * 
 * @param key 
 * @return void 
 */
void get_key(char *key)   {
    while (1)   {
        memset(key, 0, 7);
        /* get key user input */

        printf("Enter the Key: ");
        scanf("%s", key);

        /* check the new key length */
        if (strlen(key) != 7)  {
            fprintf(stderr, "invlalid key: should be 7 small case \
characters.\n");
            continue;
        }

        /* check if all small case characters or not */
        for (int i = 0; i < 7; i++) {
            if (key[i] < 'a' && key[i] > 'z')  {
                fprintf(stderr, "invlalid key: should be 7 small \
case characters.\n");
                continue;
            }
        }

        break;
    }
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
void cipher(char *msg, int msglen, char *out, int op)    {
    char key[7];
    get_key(key);

    for (int i = 0; i < msglen; i++)    {
        out[i] = char_move_around(msg[i], key[i%7], op);
    }
}
