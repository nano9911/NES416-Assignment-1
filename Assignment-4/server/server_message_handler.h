/**
 * @file server_message_handler.h
 * @author Adnan Omar (JUST ID: 123423)
 * @brief Messages handler header (all from scratch) for NES416/HW4
 * @date 2021-05-02
 * @version 0.1
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#include <dirent.h>

/* Enumeration values to make the operation more understandable */
enum operations {ERR=-1, DEC=1, ENC=2, EXIT=3, LIST=4};

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
int decode_msg(char *msg, int *len, char *text)  {
    int op = ((int)msg[0]) - 48;

    if (op == ENC || op == DEC) {
        for (int i = 1; i < *len; i++)
        {text[i-1] = msg[i];}
        *len = strlen(text);
    }
    else if (op != LIST && op != EXIT) op = ERR;
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
char char_move_around(char org, char k, int op)   {
    int v1=((int)org)-97, v2=((int)k)-97, r=0;
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
void cipher(char *msg, int msglen, int op)    {
    char key[7];
    get_key(key);

    for (int i = 0; i < msglen; i++)    {
        msg[i] = char_move_around(msg[i], key[i%7], op);
    }
}

/**
 * @brief List current directory files
 * 
 * @param out buffer to fill data in
 */
void list_directory(char *out)   {
    DIR *folder;
    struct dirent *entry;
    int files = 0;
    char filename[265], chr='\n';

    folder = opendir(".");
    if(folder == NULL)  {
        perror("Unable to read directory");
        return;
    }

    while( (entry=readdir(folder)) )
    {
        files++;
        memset(filename, 0, sizeof filename);
//        sprintf(filename, "File %3d: %s\n", files, entry->d_name);
        strncat(out, entry->d_name, strlen(entry->d_name));
        strncat(out, &chr, 1);
    }

    closedir(folder);
}
