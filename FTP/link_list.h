//
// Created by root on 18-11-26.
//

#ifndef FTP_LINK_LIST_H
#define FTP_LINK_LIST_H

#include <malloc.h>

typedef struct LNode {
    int character;//store hashed address
    int subscript;//subscript
    struct LNode *next;//next node addr
} LNode, *LinkList;

static LNode head;

int create_list(LinkList L, int c);

int append_ele(LinkList L, int c);

int ele_count(LinkList L);

int find_ele(LinkList L, int c);

int del_ele(LinkList L, int c);

int prin_ele(LinkList L);

#endif //FTP_LINK_LIST_H
