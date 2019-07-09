//
// Created by root on 18-11-26.
//

#include "v_check_username.h"

int v_check_n_p(const char *username, const char *passwd) {
    if (geteuid() != 0)                                                                                                 //sure authority is root
    {

        fprintf(stderr, "must be setuid root");

        return -1;

    }
    struct spwd *shd;                                                                                                   //get encoded passwd
    if (NULL == (shd = getspnam(username))) {
        perror("no_user_exist");
        return -1;
    }
    char salt[13];                                                                                                      //salt

    static char crypt_char[80];                                                                                         //load encoded series

    int i = 0, j = 0;

    while (shd->sp_pwdp[i] !=
           '\0')                                                                                        //the series must have three '$'
    {                                                                                                                   //cut it and store in the salt[]
        salt[i] = shd->sp_pwdp[i];
        if (salt[i] == '$') {
            j++;
            if (j == 3) {

                salt[i + 1] = '\0';

                break;

            }
        }
        i++;
    }

    if (j <
        3)                                                                                                             //risk control
        perror("file error or user cannot use.");

    if (0 == strcmp(shd->sp_pwdp, crypt(passwd, salt))) {                                                                    //successful access
        return 0;
    } else {
        return -1;
    }
}