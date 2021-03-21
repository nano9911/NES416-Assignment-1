/**
 * @file message_handler.h
 * @author Adnan Omar (JUST ID: 123423)
 * @brief Messages handler header (all from scratch) for NES416/HW1
 * @date 2021-03-18
 *
 */


/* Macro to make the function of get values from client message */
#define var(which,var1,var2)  ((which) == (0) ? (var1) : (var2))

/* Enumeration values to make the operation more understandable */
enum operations {ERR=0, ADD=1, MINUS=2, MUL=3, DIV=4};

/*
 *  Just To make the code clearer I separated this part from the message handler.
 *  It's responssible to determine if this character represents an operation ( + , - , * , / ).
 */
enum operations decode_operation(char op)   {
    enum operations ret = ERR;
    switch (op) {
        case '+':
            ret = ADD;
            break;
        case '-':
            ret = MINUS;
            break;
        case '*':
            ret = MUL;
            break;
        case '/':
            ret = DIV;
            break;
        default:
            ret = ERR;
    }
    return ret;
}

/*
 *  This function is isolated from main() to keep the networking part clearer. It's
 *  responsible to handle the function of understanding the client's message, and
 *  detemine if it's valid or not.
 *
 *  If the message is valid it will return 0, and the result will be in passes to 
 *  the integer result.
 */
int handle_msg(char msg[], int len, int *result)
{
    int x = 0, y = 0, res = 0;
    enum operations op = ERR;

    /*
     *  If a == 0:
     *      we are filling variable x
     *  
     *  else if a == 1:
     *      we are filling variable y
     *
     *  else:
     *      We have more than one oprator, so return error.
     */
    unsigned char a = 0;
    char temp;

    for (int i = 0; i < len; i++)   {
        temp = msg[i];
        if (temp == ' ')    {continue;}

        /* If it's a number add it as digit to the current varibale (x or y) */
        if (temp >= '0' && temp <= '9')   {
            /* var() -> is a macro determine which variable we are filling (x or y) */
            *(var(a, &x, &y)) += (int)temp - 48;
            *(var(a, &x, &y)) *= 10;
        }

        else  {
            /* if it's an operator, we should turn a to > 1, so for the next iterations
                we could fill the y as the second parametr instead of x. */
            a++;
            /* If there's more than one operator return an error. */
            if (a > 1)  {return -1;}
            /* If the operator is the last or fisrt character in string, this means that we had 
                only one value and an operator, so return error. */
            else if (i == len-1 || i == 0)    {return -1;}

            op = decode_operation(temp);
            if (op == ERR)    {return -2;}
        }
    }

    x /= 10; y /= 10;
    switch (op) {
        case ADD:
            res = x + y;
            break;
        case MINUS:
            res = x - y;
            break;
        case MUL:
            res = x * y;
            break;
        case DIV:
            res = x / y;
            break;
        default:
            return -1;
    }

    *result = res;
}
