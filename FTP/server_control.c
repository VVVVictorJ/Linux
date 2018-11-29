//
// Created by root on 18-11-26.
//

#include "server_control.h"
#include "link_list.h"

void quit() {
    system("clear");
    exit(0);
}                                                                                                       //Done

void kill_someone() {
    int buf[30];
    printf("input the name who you want to kill:");
    scanf("%s", &buf);
    del_ele(&head, buf);
    return;
}                                                                                               //Done

void list_name_now() {
    if (0 == prin_ele(&head))
        return;
}                                                                                              //Done

void count_history() {
    printf("Historical number of vistors:%i\n", countt_history);
}                                                                                              //Done

void count() {
    printf("Number of active user:%i\n", countt);
}                                                                                                      //Done