//
// Created by root on 18-11-21.
//

#include <unistd.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
int create_dir(const char *dir_name)
{
    char DirName[256];
    strcpy(DirName,dir_name);
    int i,length = strlen(DirName);
    if(DirName[length-1]!='/')
        strcat(DirName,"/");
    length = strlen(DirName);
    for(i=1;i<length;i++)
    {
        if(DirName[i]=='/')
        {
            DirName[i] = 0;
            if(access(DirName,NULL) != 0)
            {
                if(mkdir(DirName,0777) == -1)
                {
                    return 0;
                }
                int a = access(DirName, F_OK);
                if(a ==-1)
                {
                    printf("%c",2);
                    mkdir(DirName,0777);
                    return 1;
                }
                else{
                    printf("%c",1);
                    return 0;
                }
                DirName[i] = '/';
                return 1;
            }
        }
    }
    return 0;
}
int main(int argc,char**argv)
{
    char a[20]={"failed creating"};
    if(create_dir(argv[1])==1){printf("%s\n",&a);}
    return 0;
}