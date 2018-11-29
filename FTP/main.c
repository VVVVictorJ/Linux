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
#include "server_control.h"
#include "v_check_username.h"
#include "link_list.h"


#pragma mark                                                                        状态码
#define LOGIN           1

#define UPLOAD          002
#define DOWNLOAD        010
#define MKDIR           011
#define RMDIR           100
#define PWD             101
#define CD              110
#define LS              111
#define BYE             88

#pragma mark                                                                        端口号
#define PORT            8888
#define MAX             30
#define MMAX            1024

#pragma mark                                                                        mode
#define ASCII           1
#define BIT             2


//#pragma mark  - semaphore
//static int sema = 0;

#pragma mark                                                                        协议 头
typedef struct NYN {
    int status;                         //状态码s         // 1-----登录//002-----上传//010-----下载//011-----创建dir
    // 100-----删除dir//  101-----显示当前路径    //110-----切换目录
    // 111-----查看当前所有文件
    int mode;                           //上传下载选定模式//1-----ansic              //2-----bit  //3-----非上传下载
} CW;
typedef struct MCM {                    //s为1时选用
    char username[10];                  //right return 999
    char password[10];                  //fail  return 777
} MN;
typedef struct SQS {                    //s为2是选用
    char content[MMAX];//文件
} BX;
#pragma mark                                                                        协议 体
typedef struct NLM {
    CW _n;                               //协议头
    union {
        MN _m;
        BX _b;
    };
} BZ;

//#pragma mark init_sema() -初始化semaphore
//void init_sema() {
//    sema = 1;
//}

/*******************************************************************\
 *                                                                 *
 *  author: victor                                                 *
 *                                                                 *
 *  CopyRight(c) 2018-11-17-23:04                                  *
 *                                                                 *
 *  PurPose: FTP-Sever                                             *
 *                                                                 *
 \******************************************************************/

#pragma mark - init_socket()                                                        初始化 套接字-返回 监听套接字            //Done

int init_socket() {
    int listen_socket = socket(AF_INET, SOCK_STREAM, 0);//1、创建socket
    if (listen_socket == -1) {
        perror("socket");
        return -1;
    }
    struct sockaddr_in addr;                            // 2、命名套接字，绑定本地的ip地址和端口
    memset(&addr, 0, sizeof(addr));                     //格式化内存
    addr.sin_family = AF_INET;                          // 设置地址族
    addr.sin_port = htons(PORT);                        // 设置本地端口
    addr.sin_addr.s_addr = htonl(INADDR_ANY);           // 使用本地的任意IP地址

    int ret = bind(listen_socket, (struct sockaddr *) &addr, sizeof(addr));
    if (ret == -1) {
        perror("bind");
        return -1;
    }
    ret = listen(listen_socket, 5);                     // 3、监听本地套接字
    if (ret == -1) {
        perror("listen");
        return -1;
    }
    printf("等待客户端连接.......\n");
    return listen_socket;
}

#pragma mark - MyAccept()                                                           记录 套接字状态                        //Done

int MyAccept(int listen_socket) {                       //返回套接字状态
    struct sockaddr_in client_addr;
    int len = sizeof(client_addr);
    int client_socket = accept(listen_socket, \
                              (struct sockaddr *) &client_addr, &len);
    if (client_socket == -1) {
        perror("accept");
        return 1;
    }
    printf("成功接收一个客户端: %s\n", inet_ntoa(client_addr.sin_addr));
    return client_socket;
}

#pragma mark - Login()                                                              登录-校验                             //Done

void Login(int client_socket, struct NLM *nlm) {
    countt++;
    struct NLM tt;
    bzero(&tt, sizeof(BZ));

    if (NULL == head.next) {
        if (0 != create_list(&head, client_socket))//need to fix
            return;
    } else {
        if (0 != append_ele(&head, client_socket))
            return;
    }

    if (0 == v_check_n_p(nlm->_m.username, nlm->_m.password)) {
        tt._n.mode = 999;
        if (write(client_socket, &tt, sizeof(tt)) < 0) {
            perror("wirte error");
            return;
        }
    } else {
        tt._n.mode = 777;
        if (write(client_socket, &tt, sizeof(tt)) < 0) {
            perror("wirte error");
            return;
        }
    }
}

#pragma mark - Upload()                                                             上传                                 //Done

void Upload(int client_socket, struct NLM *nlm) {
    const char *a = "can't open";

    FILE *fd;
    int nbyte;
    struct NLM tt;
    bzero(&tt, sizeof(BZ));

    printf("get filename : [ %s ]\n", nlm->_b.content);

    if (ASCII == nlm->_n.mode) {
        if ((fd = fopen(nlm->_b.content, "r")) < 0) {                                  //ACSII---error
            printf("Open file Error!\n");
            strcpy(tt._b.content, a);
            if (write(client_socket, &tt, sizeof(tt)) < 0) {
                printf("Write Error!\n");
                return;
            }
        }
        if (-1 == chmod(nlm->_b.content, S_IRUSR | S_IWUSR | S_IROTH)) {
            printf("can't set.");
        };
        while ((nbyte = (int) read(client_socket, &tt, sizeof(tt))) > 0) {
            if (fwrite(tt._b.content, MMAX, 1, fd) < 0) {
                printf("Write Error!\n");
                fclose(fd);
                return;
            }
        }
    } else if (BIT == nlm->_n.mode) {
        if ((fd = fopen(nlm->_b.content, "rb")) <
            0) {                                                                  //ACSII---error
            printf("Open file Error!\n");
            strcpy(tt._b.content, a);
            if (write(client_socket, &tt, sizeof(tt)) < 0) {
                printf("Write Error!\n");
                return;
            }
        }
        if (-1 == chmod(nlm->_b.content, S_IRUSR | S_IWUSR | S_IROTH)) {
            printf("can't set.");
        };
        while ((nbyte = (int) read(client_socket, &tt, sizeof(tt))) > 0) {
            if (fwrite(tt._b.content, MMAX, 1, fd) < 0) {
                printf("Write Error!\n");
                fclose(fd);
                return;
            }
        }
    }
}

#pragma mark - Download()                                                           下载                                 //Done

void Download(int client_socket, struct NLM *nlm) {

    const char *a = "can't open";

    FILE *fd;
    int nbyte;
    struct NLM tt;
    bzero(&tt, sizeof(BZ));

    printf("get filename : [ %s ]\n", nlm->_b.content);

    if (ASCII == nlm->_n.mode) {
        if ((fd = fopen(nlm->_b.content, "r")) < 0) {                                  //ACSII---error
            printf("Open file Error!\n");
            strcpy(tt._b.content, a);
            if (write(client_socket, &tt, sizeof(tt)) < 0) {
                printf("Write Error!\n");
                return;
            }
        }
        while (0 < (nbyte = (int) fread(tt._b.content, MMAX, 1, fd))) {
            if (write(client_socket, tt._b.content, MMAX) < 0) {
                printf("Write Error! At commd_put 1!\n");
                fclose(fd);
                return;
            }
        }
    } else if (BIT == nlm->_n.mode) {
        if ((fd = fopen(nlm->_b.content, "rb")) < 0) {                                  //BIT---error
            printf("Open file Error!\n");
            strcpy(tt._b.content, a);
            if (write(client_socket, &tt, sizeof(tt)) < 0) {
                printf("Write Error!\n");
                return;
            }
        }
        while (0 < (nbyte = (int) fread(tt._b.content, MMAX, 1, fd))) {
            if (write(client_socket, tt._b.content, MMAX) < 0) {
                printf("Write Error! At commd_put 1!\n");
                fclose(fd);
                return;
            }
        }
    }


}

#pragma mark - Mkdir()                                                              创建 DIR                             //Done

void Mkdir(int client_socket, struct NLM *nlm) {
    const char *b = "ok!";
    struct NLM tt;
    bzero(&tt, sizeof(BZ));
    char DirName[256];
    strcpy(DirName, nlm->_b.content);
    int i, length = (int) strlen(DirName);
    if (DirName[length - 1] != '/')
        strcat(DirName, "/");
    length = (int) strlen(DirName);
    for (i = 1; i < length; i++) {
        if (DirName[i] == '/') {
            DirName[i] = 0;
            if (access(DirName, F_OK) != 0)                   //bucunzai
            {
                if (mkdir(DirName, 0777) == -1) {
                    return;
                }
                int a = access(DirName, F_OK);
                if (a == -1) {
                    mkdir(DirName, 0777);
                    return;
                } else {
                    strcpy(tt._b.content, b);
                    write(client_socket, &tt, sizeof(tt));
                    return;
                }

            }
        }
    }
}

#pragma remove_dir()                                                                删除DIR                              //Done

int remove_dir(const char *dir) {
    char cur_dir[] = ".";
    char up_dir[] = "..";
    char dir_name[128];
    DIR *dirp;
    struct dirent *dp;
    struct stat dir_stat;

    //参数传递进来的目录不存在，直接返回
    if (0 != access(dir, F_OK)) {
        return 0;
    }

    //获取目录属性失败，返回错误
    if (0 > stat(dir, &dir_stat)) {
        perror("get directory stat error");
        return -1;
    }

    if (S_ISREG(dir_stat.st_mode)) {   //普通文件直接删除
        remove(dir);
    } else if (S_ISDIR(dir_stat.st_mode)) {    //目录文件，递归删除目录中内容
        dirp = opendir(dir);
        while ((dp = readdir(dirp)) != NULL) {
            //忽略 . 和 ..
            if ((0 == strcmp(cur_dir, dp->d_name)) || (0 == strcmp(up_dir, dp->d_name))) {
                continue;
            }

            sprintf(dir_name, "%s%s", dir, dp->d_name);
            remove_dir(dir_name);    //递归调用
        }
        closedir(dirp);

        rmdir(dir);      //删除空目录
    } else {
        perror("unknow file type!");
    }
    return 0;
}

#pragma mark - Rmdir()                                                              删除 DIR                             //Done

void Rmdir(int client_socket, struct NLM *nlm) {
    const char *a = "successful remove.";
    struct NLM tt;
    bzero(&tt, sizeof(BZ));
    if (0 == remove_dir(nlm->_b.content)) {
        write(client_socket, &tt, sizeof(tt));
        return;
    }
}

#pragma mark - Pwd()                                                                显示 当前路径                         //Done

void Pwd(int client_socket, struct NLM *nlm) {
    struct NLM tt;
    bzero(&tt, sizeof(BZ));

    if (NULL == getcwd(tt._b.content, MMAX)) {
        perror("Get cerrent working directory fail.\n");
        return;
    }
    tt._n.status = PWD;
    write(client_socket, &tt, sizeof(tt));
    return;
}

#pragma mark - Ls()                                                                 显示 当前文件夹                        //Done

void Ls(int client_socket, struct NLM *nlm) {
    DIR *my_dir = NULL;
    struct dirent *structdirent = NULL;
    struct stat statbuf;
    struct NLM tt;
    bzero(&tt, sizeof(BZ));

    if (NULL == (my_dir = opendir(nlm->_b.content))) {                              //open dir
        fprintf(stderr, "can not open directory: %s\n", nlm->_b.content);
        return;
    }

    chdir(nlm->_b.content);
    while (NULL != (structdirent = readdir(my_dir))) {
        lstat(structdirent->d_name, &statbuf);
        if (S_ISDIR(statbuf.st_mode)) {                                          //judge DIR or not
            if (0 == strcmp(".", structdirent->d_name) ||
                0 == strcmp("..", structdirent->d_name))
                continue;
            if (0 > (sprintf(tt._b.content, "%c%s\n", '\\', structdirent->d_name))) {
                printf("Sprintf Error!\n");
                return;
            }
            tt._n.status = LS;                                                    //set status
            if (write(client_socket, &tt, sizeof(tt)) < 0) {
                printf("Write Error!\n");
                return;
            }
        }
    }
    chdir("..");
    closedir(my_dir);
}

#pragma mark - Cd()                                                                 切换 目录                             //Done

void Cd(int client_socket, struct NLM *nlm) {
    DIR *my_dir = NULL;
    struct NLM tt;

    bzero(&tt, sizeof(BZ));
    if (NULL == (my_dir = opendir(nlm->_b.content))) {
        fprintf(stderr, "cannot open directory: %s\n", nlm->_b.content);
        return;
    }
    chdir(nlm->_b.content);
    strcpy(tt._b.content, "dir has changed.");
    tt._n.status = CD;
    write(client_socket, &tt, sizeof(tt));
}

#pragma mark - Bye()                                                                退出                                 //Done

void Bye(int client_socket, struct NLM *nlm) {
    if (88 == nlm->_n.status) {
        countt--;
        del_ele(&head, client_socket);
        close(client_socket);
    }
}

#pragma mark - Handle_Thread()                                                      线程 调用函数处理用户请求                //Done

void *Handle_Thread(void *pVoid) {
    int client_socket = (int) pVoid;
    struct NLM tmp;
    while (1) {
        int ret = (int) read(client_socket, &tmp, sizeof(tmp));
        if (-1 == ret) {
            perror("read");
            break;
        }
        switch (tmp._n.status) {
            case LOGIN:
                Login(client_socket, &tmp);
                break;
            case UPLOAD:
                Upload(client_socket, &tmp);
                break;
            case DOWNLOAD:
                Download(client_socket, &tmp);
                break;
            case MKDIR:
                Mkdir(client_socket, &tmp);
                break;
            case RMDIR:
                Rmdir(client_socket, &tmp);
                break;
            case PWD:
                Pwd(client_socket, &tmp);
                break;
            case LS:
                Ls(client_socket, &tmp);
                break;
            case CD:
                Cd(client_socket, &tmp);
                break;
            case BYE:
                Bye(client_socket, &tmp);
                break;
            default:
                break;
        }
    }
}

#pragma mark - Handle_Myserver()                                                    本地 线程                             //Done

void *Handle_Myserver() {
    const char *w = "\t\twelcome!";
    printf("%s\n", w);
    char buff[MAX];
    printf("\t\tyou can use this command.\n"
           "\t\t1.count\n\t\t2.count_history\n\t\t3.list_name_now\n\t\t4.kill_someone\n\t\t5.quit\n"
           "p.s:just letter don't number!\n");
    fflush(stdin);
    while (1) {
        scanf("%s", &buff);
        while (getchar() != '\n');
        //system("clear");
        if (0 == strcmp(buff, "count")) {
            count();
        } else if (0 == strcmp(buff, "count_history")) {
            count_history();
        } else if (0 == strcmp(buff, "list_name_now")) {
            list_name_now();
        } else if (0 == strcmp(buff, "kill_someone")) {
            kill_someone(buff);
        } else if (0 == strcmp(buff, "quit")) {
            quit();
        }
    }
}

#pragma mark - main()                                                                                                   //入口函数

int main(int argc, char **argv) {
    pthread_t listen;                                                                                                   //用于监听线程与主线程通信
    pthread_create(&listen, NULL, Handle_Myserver,
                   NULL);                                                               //服务端命令行监听线程
    pthread_detach(listen);
    printf("主线程");
    int listen_socket = init_socket();//初始化
    printf("%i", listen_socket);
    while (1) {
        //sleep(1000);
        int client_socket = MyAccept(listen_socket);//获取客户端套接字
        //创建线程处理连接
        pthread_t id;
        pthread_create(&id, NULL, Handle_Thread, (void *) (long) client_socket);
        pthread_detach(
                id);                                                                                             //线程分离
    }
    close(listen_socket);
    printf("Bye--");
    return 0;
}
/*
    wchar_t a[] = L"叶长青你妈死了";
    wchar_t b[] = L"欢迎";
    setlocale(LC_ALL, "zh_CN.UTF-8");
    if (
            argv[1][0] != 'f' || \
            argv[1][1] != 't' || \
            argv[1][2] != 'p' || \
            argv[1][3] != '/' || \
            argv[1][4] != '/') {
        wprintf(L"%ls ", a);
        return -1;
    }
    wprintf(L"%ls ", b);
    init_sema();
*/
/*
//  *
//#pragma mark quit()                                                                 退出                                 //Done
//
//void quit() {
//    system("clear");
//    exit(0);
//}
//
//#pragma mark kill_someone()                                                         与某个用户断 连
//
//void kill_someone(char *c) {
//    //处理字符串
//}
//
//#pragma mark list_name_now()                                                        当前 在线用户名
//
//void list_name_now() {
//
//    //for处理
//}
//
//#pragma mark - count_history()                                                      总 访问数                             //Done
//
//void count_history() {
//    printf("Historical number of vistors:%i\n", countt_history);
//}
//
//#pragma mark - count()                                                              获取 当前人数                          //Done
//
//void count() {
//    printf("Number of active user:%i\n", countt);
//}
//*/