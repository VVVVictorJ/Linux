//
// Created by root on 18-11-26.
//

#ifndef LINUX_SERVER_CONTROL_H
#define LINUX_SERVER_CONTROL_H

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <stdio.h>
#include<stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <wchar.h>
#include <locale.h>
#include <semaphore.h>
#include <dirent.h>
#include <fcntl.h>
#include "link_list.h"

#pragma mark                                                                        总访问数
static int countt = 0;
static int countt_history = 0;

void quit();

void kill_someone();

void list_name_now();

void count_history();

void count();

#endif //LINUX_SERVER_CONTROL_H
