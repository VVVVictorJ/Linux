//
// Created by root on 18-11-26.
//

#ifndef FTP_V_CHECK_USERNAME_H
#define FTP_V_CHECK_USERNAME_H
#define _XOPEN_SOURCE

#include <pwd.h>
#include <stddef.h>
#include <string.h>
#include <shadow.h>
#include <stdio.h>
#include <unistd.h>


int v_check_n_p(const char *username, const char *passwd);


#endif //FTP_V_CHECK_USERNAME_H
