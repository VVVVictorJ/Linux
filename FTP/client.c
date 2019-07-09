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


#pragma mark                                                                        协议 头
typedef struct NYN {
    int status;                         //状态码s        // 1-----登录//001-----上传//010-----下载//011-----创建dir
    //100-----删除dir//101-----显示当前路径//110-----切换目录
    //111-----查看当前所有文件
    int mode;                           //上传下载选定模式//1-----ansic//2-----bit//3-----非上传下载
}CW;
typedef struct MCM {                    //s为1时选用
    char username[10];
    char password[10];
} MN;
typedef struct SQS {                    //s为2是选用
    char content[1024];//文件
} BX;
#pragma mark                                                                        协议 体
typedef struct NLM {
    CW _n;                               //协议头
    union {
        MN _m;
        BX _b;
    };
} BZ;


#pragma mark - write_server                                                         写线程
void write_server(int client_socket)
{
    int a ;
    while (1)
    {

        while (1) {
            printf("\n");
            printf("\n");
            printf("\n");
            interface();
            sleep(1);
            printf("\t\t输入功能:\n");
            scanf("%d", &a);
            printf("\n");
            while (getchar() != '\n');
            switch (a) {
                case 1 :
                    preupload(client_socket);
                    break;
                case 2 :
                    predownload(client_socket);
                    break;
                case 3 :
                    ctdir(client_socket);
                    break;
                case 4 :
                    deldir(client_socket);
                    break;
                case 5 :
                    pwd(client_socket);
                    break;
                case 6 :
                    ls(client_socket);
                    break;
                case 7 :
                    cd(client_socket);
                    break;
                case 8 :
                    logout();
            }
//            system("clear");
        }

    }

}


#pragma mark -interface                                                             主界面
void interface()
{
    printf ("\t\t 1、上传文件\n");
    printf ("\t\t 2、下载文件\n");
    printf ("\t\t 3、创建目录\n");
    printf ("\t\t 4、删除目录\n");
    printf ("\t\t 5、显示当前路径\n");
    printf ("\t\t 6、显示当前文件夹\n");
    printf ("\t\t 7、切换目录\n");
    printf ("\t\t 8、退出\n");
}


#pragma mark - Upload()                                                             上传
void preupload(int client_socket){
    struct NLM tname;
    tname._n.status=UPLOAD;
    char name[1024];
    int m;
    printf("文件名：");
    scanf("%s",&name);
    printf("传输模式:\n1.ASCII\n2.BIT\n");
    scanf("%i",&m);
    switch (m)
    {
        case 1:tname._n.mode=ASCII;
            break;
        case 2:tname._n.mode=BIT;
            break;
        default:printf("erro");
            break;
    }
    printf("%i",tname._n.mode);
    strcpy(tname._b.content,name);
    write(client_socket, &tname, sizeof(tname));

    Upload(client_socket,&tname);
}

void Upload(int client_socket,struct NLM *nlm){
    const char *a = "can't open";

    FILE *fd;
    size_t nbyte;
    long file_len = 0;
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
        fseek(fd, 0L, SEEK_END);
        file_len = ftell(fd);
        fseek(fd, 0L, SEEK_SET);

        while (!feof(fd))                //判断文件是否结束
        {
            fread(tt._b.content, MMAX, 1, fd);
            if (MMAX > file_len)                //缓冲区长度大于文件长度
                write(client_socket, tt._b.content, MMAX);
            else {
                write(client_socket, tt._b.content, MMAX);
                file_len -= MMAX;
            }
            bzero(&tt, sizeof(BZ));         //清零缓冲区
        }

        fclose(fd);


//        while (nbyte = fread(&tt._b.content, sizeof(char), MMAX, fd)) {
//
//            if (write(client_socket, tt._b.content, MMAX) < 0) {
//                perror("Write Error! At commd_put 1!");
//                fclose(fd);
//                return;
//            }
//        }
    } else if (BIT == nlm->_n.mode) {
        if ((fd = fopen(nlm->_b.content, "rb")) < 0) {                                  //BIT---error
            perror("Open file Error!");
            strcpy(tt._b.content, a);
            if (write(client_socket, &tt, sizeof(tt)) < 0) {
                printf("Write Error!\n");
                return;
            }
        }
        fseek(fd, 0L, SEEK_END);
        file_len = ftell(fd);
        fseek(fd, 0L, SEEK_SET);

        while (!feof(fd))                //判断文件是否结束
        {
            fread(tt._b.content, MMAX, 1, fd);
            if (MMAX > file_len)                //缓冲区长度大于文件长度
                write(client_socket, tt._b.content, MMAX);
            else {
                write(client_socket, tt._b.content, MMAX);
                file_len -= MMAX;
            }
            bzero(&tt, sizeof(BZ));         //清零缓冲区
        }

        fclose(fd);
        return;
//        while (0 < (nbyte = (int) fread(tt._b.content, sizeof(char), MMAX, fd))) {
//            if (write(client_socket, tt._b.content, MMAX) < 0) {
//                printf("Write Error! At commd_put 1!\n");
//                fclose(fd);
//                return;
//            }
//        }
    }
}

#pragma mark - Download()                                                             下载
void predownload(int client_socket){
    struct NLM tname;
    tname._n.status=UPLOAD;
    char name[1024];
    int m;
    printf("文件名：");
    scanf("%s",&name);
    printf("传输模式:\n1.ASCII\n2.BIT\n");
    scanf("%i",&tname._n.mode);
    strcpy(tname._b.content,name);
    write(client_socket, &tname, sizeof(tname));

    Download(client_socket,&tname);
}
void Download(int client_socket,struct NLM *nlm) {

    const char *a = "can't open";

    FILE *fd;
    ssize_t nbyte;
    size_t wbyte;
    struct NLM tt;
    bzero(&tt, sizeof(BZ));
    char *ptr;

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
        while (nbyte = read(client_socket, &tt, sizeof(tt))) {
            ptr = tt._b.content;
            if (nbyte > 0) {
                while (wbyte = fwrite(ptr, sizeof(char), MMAX, fd)) {
                    if (wbyte == nbyte) {
                        break;
                    } else if (wbyte > 0) {
                        ptr += wbyte;
                        nbyte -= wbyte;
                    }
                }
                if (0 == wbyte) {
                    break;
                }
            }
        }
        close(fd);
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
        while (nbyte = read(client_socket, &tt, sizeof(tt))) {
            ptr = tt._b.content;
            if (nbyte > 0) {
                while (wbyte = fwrite(ptr, sizeof(char), MMAX, fd)) {
                    if (wbyte == nbyte) {
                        break;
                    } else if (wbyte > 0) {
                        ptr += wbyte;
                        nbyte -= wbyte;
                    }
                }
                if (0 == wbyte) {
                    break;
                }
            }
        }
        close(fd);
    }
}


#pragma mark - mkdir()                                                               创建目录
void ctdir(int client_socket) {
        struct NLM wri;
        struct NLM rea;
        wri._n.status = MKDIR;
        printf("输入要创建的目录:\n");
        scanf("%s", wri._b.content);
        write(client_socket, &wri, sizeof(wri));
        read(client_socket, &rea, sizeof(rea));
        printf("%s\n", rea._b.content);
}


#pragma mark - rmdir()                                                               删除目录
void deldir(int client_socket) {
        struct NLM wri;
        struct NLM rea;
        wri._n.status = RMDIR;
        printf("输入要删除的目录:\n");
        scanf("%s", wri._b.content);
        write(client_socket, &wri, sizeof(wri));
        read(client_socket, &rea, sizeof(rea));
        printf("%s\n", rea._b.content);
}


#pragma mark - pwd()                                                                 显示当前路径
void pwd(int client_socket) {
        struct NLM wri;
        struct NLM rea;
        wri._n.status = PWD;
        write(client_socket, &wri, sizeof(wri));
        read(client_socket, &rea, sizeof(rea));
        printf("%s", rea._b.content);
}


#pragma mark - ls()                                                                  显示当前文件夹
void ls(int client_socket) {
        struct NLM wri;
        struct NLM rea;
        wri._n.status = LS;

        printf("输入文件夹:\n");
        scanf("%s", wri._b.content);
        write(client_socket, &wri, sizeof(wri));
        read(client_socket, &rea, sizeof(rea));
        printf("%s\n", rea._b.content);
}


#pragma mark - cd()                                                                  切换目录
void cd(int client_socket) {
        struct NLM wri;
        struct NLM rea;
        wri._n.status = CD;
        printf("输入要切换的目录:\n");
        scanf("%s", wri._b.content);
        write(client_socket, &wri, sizeof(wri));
        read(client_socket, &rea, sizeof(rea));
        printf("%s\n", rea._b.content);
}


#pragma mark - logout                                                               退出
void logout() {
        system("clear");
        exit(0);
}


#pragma mark - main()                                                               入口函数
int main(int argc, char **argv) {
        // 创建与服务器通信的套接字
        int client_socket = socket(AF_INET, SOCK_STREAM, 0);
        if (client_socket == -1) {
            perror("client_socket");
            return -1;
        }

        // 连接服务器
        struct sockaddr_in addr;
        memset(&addr, 0, sizeof(addr));
        addr.sin_family = AF_INET;     // 设置地址族
        addr.sin_port = htons(PORT); // 设置本地端口
        inet_aton("127.0.0.1", &(addr.sin_addr));


        // 连接服务器，如果成功，返回0，如果失败，返回-1
        // 成功的情况下，可以通过socketfd与服务器进行通信
        int ret = connect(client_socket, (struct sockaddr *) &addr, sizeof(addr));
        if (ret == -1) {
            perror("connect");
            return -1;
        }

        printf("成功连上服务器\n");

        //开始进行操作；
        struct NLM psw;
        char t[256];
        char username[10];
        char password[10];
        char port[10];
        int i;
        int count1;
        int count2;
        while (1) {                                             //登录
        printf("%s\n",psw._b.content);
        printf("%i\n",psw._n.mode);
//        if(psw._n.status==LOGIN) {
            if (psw._n.mode == 999)
                break;                                         //判断是否已经登录成功
            if (psw._n.mode == 777) {
                printf("用户名密码不正确\n");
                logout();
            }
//        }
            printf("正在验证用户名和密码\n");
            strcpy(t, argv[1]);
            int length = strlen(t);
            if (t[0] == 'f' && t[1] == 't' && t[2] == 'p' && t[3] == ':' && t[4] == '/' && t[5] == '/') {
                printf("ftp头格式正确\n");
                if (t[6] == '@') { printf("缺少用户名\n"); }
                count1 = 0;
                count2 = 0;
                for (i = 6; i < length; i++) {
                    if (t[i] == '@')break;
                    username[i - 6] = t[i];                   //获取用户名
                    count1++;
                }
                for (i = 7 + count1; i < length; i++) {
                    if (t[i] == ':')break;
                    password[i - (7 + count1)] = t[i];        //获取密码
                    count2++;
                }
                for (i = 8 + count1 + count2; i < length; i++) {
                    port[i - (8 + count1 + count2)] = t[i];   //获取端口号
                }
//            printf("%i\n",strcmp(username,"sdzhj"));
//            printf("%i\n",strcmp(password,"123"));
            } else {
                printf("指令格式错误\n");
                logout();
            }
            psw._n.status = LOGIN;
            strcpy(psw._m.username, username);
            strcpy(psw._m.password, password);
            printf("1");
            write(client_socket, &psw, sizeof(psw));     //消息写过去，等待回复，例如什么用户不存在，密码错误
            read(client_socket, &psw, sizeof(psw));      //读取来自服务器的返回的消息，进行客户端屏幕打印
        }
        printf("恭喜你登录成功\n");
        write_server(client_socket);
        // 关闭套接字
        close(client_socket);
        return 0;
}