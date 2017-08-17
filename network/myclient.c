/*************************************************************************
	> File Name: myclient.c
	> Author: 
	> Mail: 
	> Created Time: 2017年08月11日 星期五 08时16分59秒
 ************************************************************************/
#include<stdio.h>
#include<termios.h>
#include<unistd.h>
#include<assert.h>
#include<string.h>
#include<string.h>
#include<stdlib.h>
#include<netinet/in.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<cJSON.h>
#include<pthread.h>

#define mouth 4507

//向上移动光标 cursor_up(3); 代表向上移动3.
#define cursor_up(n) \
    fprintf(stderr, "\033[%dA", (n))
//向下移动光标
#define cursor_down(n) \
    fprintf(stde
//向右移动光标
#define cursor_right(n) \
    fprintf(stderr, "\033[%dC", (n))
#define START "\t************************************************************************************\n"

int getch();
void log_in(int client_fd);
void log_up(int client_fd);
void my_error(char *string,int line);
void log_in_ui(void);
void find_passwd(int client_fd);
void log_after_ui();
int log_after_friend(char *name);
void search_online_friend(char *name);//查找在线好友
void add_friend(char *name);//添加好友
void friend_ask(char *name);//好友请求
void search_allfriend(char *name);//搜索好友
void delete_friend(char *name);//删除好友
void chat(char *name);//私聊
void search_group(char *name);//搜索群
void read_message(char *name);
void search_chat(char *name);
void chat_withfriend(char *name);
int log_after_group(char *name);
void create_group(char *name);
void dissolve_group(char *name);
void manage_group(char *name);
void group_ask(char *name);
void chat_group(char *name);
void read_group(char *name);
int client_fd;//全局变量  
int main()
{
    struct sockaddr_in addr;
    memset(&addr,0,sizeof(addr));
    char buf[400];
    memset(buf,0,sizeof(buf));
    addr.sin_family=AF_INET;
    addr.sin_port=htons(mouth);
    addr.sin_addr.s_addr=inet_addr("127.0.0.1");//服务器地址

    if((client_fd=socket(AF_INET,SOCK_STREAM,0))<0)//创建客户端套接字
    {
        my_error("client socket",__LINE__);
    }
    if(connect(client_fd,(struct sockaddr *)&addr,sizeof(struct sockaddr))<0)//将套接字绑定到服务器地址
    {
       my_error("connect",__LINE__);
    }

    int choice;
    while(1)
    {
        char buf[20];
        log_in_ui();
        memset(buf,0,sizeof(buf));
        //fprintf(stderr,"\033[20C");
        printf("\t\t请输入你的选项:");
        scanf(" %d%*c",&choice);
        log_in_ui();
        switch(choice)
        {
            case 1:
                log_in(client_fd);
                break;
            case 2:
                log_up(client_fd);
                break;
            case 3:
                find_passwd(client_fd);
                break;
        }
    }
    close(client_fd);
    printf("thanks for using!\n");
    return 0;
}

void log_in_ui()
{   
    system("clear");
    printf(START);
    printf("\t\t\t\t\t欢迎来到聊天室\n");
    printf("\n\n\t\t\t 请输入您的选项：\n");
    printf("\t\t\t\t\t 1、登录\n");
    printf("\t\t\t\t\t 2、注册\n");
    printf("\t\t\t\t\t 3、找回密码\n");
    printf(START);
}

void log_in(int client_fd)
{
    int i=0;
    char passwd[20];
    int count=0;
    char name[100];
    while(1)
    {
        if(i!=0)
            log_in_ui();
        printf("\t\t\t用户名:\n\t\t\t密码:\033[1A\033[3C");
        fgets(name,100,stdin);
        if(strcmp(name,"\n")==0)
        {
            printf("\n\t\t\t\t用户名非法，请重新输入!\n");
            sleep(2);
            system("clear");
            i=1;
            continue;
        }
        name[strlen(name)-1]='\0';
        fprintf(stderr,"\033[29C");
        break;
    }
    i=0;
    while(1)
    {
        if(i!=0)
            printf("\t\t\t\t请再次输入密码:");
        for(i=0;;)
        {
            char t=getch();
            if(t=='\n'||t=='\r')
            {
                passwd[i]='\0';
                break;
            }
            if(t==127&&i>=1)
            {
                printf("\b \b");
                passwd[--i]='\0';
            }
            if (t != 127)
            {
                printf("*");
                passwd[i++] = t;
            }
            if(i<0)
            {
                passwd[0]='\0';
                i=0;
            } 
        }
        cJSON * root;
        root=cJSON_CreateObject();
        cJSON_AddStringToObject(root,"name",name);
        cJSON_AddStringToObject(root,"password",passwd);
        cJSON_AddNumberToObject(root,"type",0);
        char *out=cJSON_Print(root);
         if(send(client_fd,out,strlen(out),0)<0)
        {
            my_error("send failed",__LINE__);
        }
        char buf[20];
        if(recv(client_fd,buf,sizeof(buf),0)<0)
        {
            my_error("recv failed",__LINE__);
        }
        if(strcmp(buf,"登录成功!")==0)
            {
                printf("\n\n\t\t\t\t\t%s\n",buf);
                sleep(2);
            //system("sl");
                system("clear");
                break;
            }
        else
            count++;
        if(count!=3)
            printf("\t\t\t\t您已输错%d次,还有%d次机会!\n",count,3-count);
        if(count==3)
        {
            printf("\t\t\t\t您已输错3次！\n");
            exit(1);
        }
    }
    //登录之后的界面
    log_after_ui(name);
}

void log_after_ui(char *name)//登录之后的界面
{
    while(1)
    {
        int flag=0;
        system("clear");
        printf(START);
        printf("\t\t\t\t1、好友信息\n");
        printf("\t\t\t\t2、群信息\n");
        printf("\t\t\t\t3、传文件\n");
        printf("\t\t\t\t4、退出\n");
        printf(START);
        printf("\n\t       请输入您的选项:\n");
        int choice;
        fprintf(stderr,"\033[30C");
        scanf(" %d",&choice);
        switch(choice)
        {
            case 1:
                while(1)
                {
                    system("clear");
                    printf(START);
                    printf("\t\t\t\t1、查看在线好友\n");
                    printf("\t\t\t\t2、添加好友\n");
                    printf("\t\t\t\t3、查看好友请求\n");
                    printf("\t\t\t\t4、显示全部好友\n");
                    printf("\t\t\t\t5、删除好友\n");
                    printf("\t\t\t\t6、私聊\n");
                    printf("\t\t\t\t0、返回上一级\n");
                    printf(START);
                    read_message(name);
                    //未读消息
                    printf("\n\n\t\t\t\t请输入你的选项:");
                    int flag=log_after_friend(name);
                    if(flag==0)
                        break;
                }
               break;
            case 2:
                while(1)
                {
                    system("clear");
                    printf(START);
                    printf("\t\t\t\t1、创建群\n");
                    printf("\t\t\t\t2、解散群\n");
                    printf("\t\t\t\t3、管理群\n");
                    printf("\t\t\t\t4、群邀请\n");
                   // printf("\t\t\t\t5、群聊天\n");
                    printf("\t\t\t\t0、返回上一级\n");
                    printf(START);
                    int flag=log_after_group(name);
                    if(flag==0)
                        break;
                }
                break;
                /*
            case 3:
                break;*/
            case 4:
            {
                flag=1;
                cJSON *root=cJSON_CreateObject();
                cJSON_AddNumberToObject(root,"type",-1);
                cJSON_AddStringToObject(root,"name",name);
                char *out=cJSON_Print(root);
                send(client_fd,out,strlen(out),0);
            }
                break;
        }
        if(flag)
        {
            system("clear");
            printf(START);
            printf("\n\n               \t\t\t\t欢迎下次使用!\n\n");
            printf(START);
            close(client_fd);
            exit(0);
        }
    }
}

int log_after_friend(char *name)
{
    int choice;
    scanf("%d",&choice);
    switch(choice)
    {
        case 1:
            search_online_friend(name);
            break;
        case 2:
            add_friend(name);
            break;
        case 3:
            friend_ask(name);
            break;
        case 4:
            search_allfriend(name);
            break;
        case 5:
            delete_friend(name);
            break;
        case 6:
            chat(name);
            break;
        default:
            break;
    }
    return choice;

}

void search_online_friend(char *name)
{
    cJSON *root=cJSON_CreateObject();
    cJSON_AddStringToObject(root,"name",name);
    cJSON_AddNumberToObject(root,"type",3);
    char *out=cJSON_Print(root);
    if(send(client_fd,out,strlen(out),0)<0)
    {
        my_error("send search_online",__LINE__);
    }
    char buf[500];
    if(recv(client_fd,buf,sizeof(buf),0)<0)
    {
        my_error("recv search_online",__LINE__);
    }
    if(strcmp(buf,"没有好友在线")==0)
       {
            printf("\n\n\t\t\t\t%s!\n",buf);
            sleep(3);
       }
    else
    {
        root=cJSON_Parse(buf);
        cJSON * list=cJSON_GetObjectItem(root,"list");
        int size=cJSON_GetArraySize(list);
        cJSON *arr,*final;
        for(int i=0;i<size;i++)
        {
            arr=cJSON_GetArrayItem(list,i);
            final=cJSON_GetObjectItem(arr,"name");
            printf("\n\t\t%d、姓名:%s\n",i+1,final->valuestring);
        }
        fflush(stdout);
        putchar('\n');
        sleep(3);
    }
}

void add_friend(char *name)
{
    while(1)
    {
        system("clear");
        printf("\t\t\t\t请输入您要添加好友的姓名:");
        char toname[100];
        scanf(" %s",toname);
        cJSON * root=cJSON_CreateObject();
        cJSON_AddStringToObject(root,"toname",toname);
        cJSON_AddStringToObject(root,"fromname",name);
        cJSON_AddNumberToObject(root,"type",4);
        char *out =cJSON_Print(root);
        send(client_fd,out,strlen(out),0);
        memset(toname,0,sizeof(toname));
        if(recv(client_fd,toname,sizeof(toname),0)<0)
        {
            my_error("recv failed",__LINE__);
        }
        if(strcmp(toname,"发送成功")==0)
        {
            printf("\t\t\t\t%s\n!",toname);
            printf("\n\t\t是否返回上一级(yes/no):");
            char yes[20];
            scanf(" %s",yes);
            if(strcmp(yes,"yes")==0)
                break;
        }
        else 
        {
            printf("\t\t\t\t%s,请重新输入!\n",toname);
            sleep(2);
        }
    }
}

void friend_ask(char *name)
{

    while(1)//放在while循环会出现段错误
    {
        cJSON *root=cJSON_CreateObject();
        cJSON_AddStringToObject(root,"name",name);
        cJSON_AddNumberToObject(root,"type",5);
    
        cJSON *list;
        char *out;
        cJSON *arr;
        int size;

        system("clear");
        int choice;
        char result[400];
        memset(result,0,sizeof(result));
        printf("\t\t请输入您的选项:\n");
        printf("\t\t\t\t1、您发送的好友请求\n");
        printf("\t\t\t\t2、您收到的请求\n");
        printf("\t\t\t\t3、返回上一层\n");
        scanf(" %d",&choice);
        if(choice==3)
            break;
        switch(choice)
        {
        case 1:
            cJSON_AddNumberToObject(root,"casenum",1);
            cJSON_AddNumberToObject(root,"type",5);
            out=cJSON_Print(root);
            send(client_fd,out,strlen(out),0);
            memset(result,0,sizeof(result));
            if(recv(client_fd,result,sizeof(result),0)<0)
            {
                my_error("recv case 1",__LINE__);
            }
            if(strcmp(result,"您没有发送好友请求")==0)
            {
                system("clear");
                printf("\n\n\t\t\t\t%s!\n",result);
                sleep(3);
                break;
            }
            fprintf(stderr,"%s",result);
            root=cJSON_Parse(result);
            list=cJSON_GetObjectItem(root,"list");
            size=cJSON_GetArraySize(list);
            while(1)
            {
                system("clear");
                printf("\n\t\t您申请的好友请求:\n");
                for(int i=0;i<size;i++)
                {
                    arr=cJSON_GetArrayItem(list,i);
                    printf("\t\t\t\t%d、%s\n",i+1,arr->valuestring);
                }
                fflush(stdout);
                printf("\n\t\t\t\t是否返回上一级(yes/no):");
                char yes[20];
                scanf(" %s",yes);
                getchar();
                if(strcmp(yes,"yes")==0)
                    break;
            }
            break;
        case 2:
            cJSON_AddNumberToObject(root,"casenum",2);
            cJSON_AddNumberToObject(root,"type",5);
            out=cJSON_Print(root);
            printf("443 %s",out);
            send(client_fd,out,strlen(out),0);
            memset(result,0,sizeof(result));
            if(recv(client_fd,result,sizeof(result),0)<0)
            {
                my_error("recv case 1",__LINE__);
            }
            printf("450 %s\n",result);////
            root=cJSON_Parse(result);
            list=cJSON_GetObjectItem(root,"list");
            size=cJSON_GetArraySize(list);
            cJSON *arr1,*final1;
            system("clear");
            if(size==0)
            {
                printf("\n\n\t\t您没有收到好友请求\n");
                sleep(3);
            }
            else 
            {
                for(int i=0;i<size;i++)
                {
                    arr=cJSON_GetArrayItem(list,i);
                    printf("\t\t您收到的好友请求:\n");
                    printf("\t\t\t\t%d、姓名%s\n",i+1,arr->valuestring);
                }
                fflush(stdout);
                putchar('\n');
                while(1)
                {
                    char agree[20];
                    memset(agree,0,sizeof(agree));
                    printf("\n\n\t\t请输入您想添加好友的名字(返回上一层请输入quit):\n");
                   // fprintf(stderr,"\033[30C");
                    scanf(" %s",agree);
                    cJSON *root1=cJSON_CreateObject();
                    cJSON_AddStringToObject(root1,"name",name);
                    cJSON_AddNumberToObject(root1,"type",5);
                    cJSON_AddStringToObject(root1,"agree",agree);
                    if(strcmp(agree,"quit")==0)
                    {
                        break;
                    }
                    char *out=cJSON_Print(root1);
                    printf("\t\t\t\t添加好友成功!");
                    send(client_fd,out,strlen(out),0);
                }
            }
            break;
        }
    }
}

void search_allfriend(char *name)
{
    char result[400];
    cJSON * sendout=cJSON_CreateObject();
    cJSON_AddStringToObject(sendout,"name",name);
    cJSON_AddNumberToObject(sendout,"type",6);
    char * sendtosever=cJSON_Print(sendout);
    send(client_fd,sendtosever,strlen(sendtosever),0);
    memset(result,0,sizeof(result));
    if(recv(client_fd,result,sizeof(result),0)<0)
    {
        my_error("search_allfriend",__LINE__);
    }
    while(1)
    {
        system("clear");
        if(strcmp(result,"您没有好友")==0)
            printf("\n\t\t\t\t%s",result);
        else
        {
            cJSON *root=cJSON_Parse(result);
            cJSON *list=cJSON_GetObjectItem(root,"list");
            cJSON * arr; 
            int   size=cJSON_GetArraySize(list);
            for(int i=0;i<size;i++)
            {
                arr=cJSON_GetArrayItem(list,i);
                printf("\t\t\t\t%d、姓名:%s\n",i+1,arr->valuestring);
            }
            putchar('\n');
        }
        printf("\n\t\t\t\t是否返回上一层:(yes/no):");
        char yes[20];
        scanf(" %s",yes);
        if(strcmp(yes,"yes")==0)
            break;
    }
}

void delete_friend(char *name)
{
    while(1)
    {
        printf("\t\t\t\t请输入你要删除的好友");
        char reduce[50];
        fprintf(stderr,"\033[10C");
        scanf(" %s",reduce);
        cJSON *root=cJSON_CreateObject();
        cJSON_AddStringToObject(root,"selfname",name);
        cJSON_AddStringToObject(root,"reducename",reduce);
        cJSON_AddNumberToObject(root,"type",7);
        char *out=cJSON_Print(root);
        send(client_fd,out,strlen(out),0);

        char result[400];
        if(recv(client_fd,result,sizeof(result),0)<0)
        {
            my_error("delte failed",__LINE__);
        }
        printf("\t\t\t\t%s!\n",result);
        printf("\n\n\t\t是否返回上一级\n(yes/no):");
        char yes[20];
        scanf(" %s",yes);
        getchar();
        if(strcmp(yes,"yes")==0)
            break;
    }
    
}
void read_message(char *name)
{
    cJSON *root=cJSON_CreateObject();
    cJSON_AddStringToObject(root,"fromname",name);
    cJSON_AddNumberToObject(root,"type",9);
    char *out=cJSON_Print(root);
    send(client_fd,out,strlen(out),0);
    char result[400];
    memset(result,0,sizeof(result));
    if(recv(client_fd,result,sizeof(result),0)<0)
    {
        my_error("read_message",__LINE__);
    }
    if(strcmp(result,"没有消息")==0)
        printf("\t\t您没有好友消息\n");
    else 
    {
        cJSON * recvin=cJSON_Parse(result);
        cJSON *list=cJSON_GetObjectItem(recvin,"list");
        cJSON *arr;
        int size=cJSON_GetArraySize(list);
        printf("你收到以下好友消息:\n");
        for(int i=0;i<size;i++)
        {
            arr=cJSON_GetArrayItem(list,i);
            printf("\t\t\t%d、姓名%s\n",i+1,arr->valuestring);
        }
    }

}
void chat(char *name)
{
    while(1)
    {
        system("clear");
        read_message(name);
        //消息列表
        putchar('\n');
        printf("\t\t\t\t1、查看聊天记录\n");
        printf("\t\t\t\t2、给好友发送信息\n");
        printf("\t\t\t\t3、返回上一层\n");
        int choice;
        scanf(" %d",&choice);
        if(choice==3)
            break;
        switch(choice)
        {
            case 1:
                search_chat(name);
                break;
            case 2:
                chat_withfriend(name);
                break;
        }
    }
   
}

void search_chat(char *name)
{
    printf("\t\t\t\t请输入您想查看和谁的聊天记录:");
    char result[10000];
    scanf(" %s",result);
    cJSON *case1=cJSON_CreateObject();
    cJSON_AddStringToObject(case1,"toname",result);
    cJSON_AddStringToObject(case1,"fromname",name);
    cJSON_AddNumberToObject(case1,"type",8);
    cJSON_AddNumberToObject(case1,"casenum",1);
    char *sendout1=cJSON_Print(case1);
   // printf("640%s \n",sendout1);
    send(client_fd,sendout1,strlen(sendout1),0);
    memset(result,0,sizeof(result));
    if(recv(client_fd,result,sizeof(result),0)<0)
    {
        my_error("recv case 1",__LINE__);
    }
    if(strcmp(result,"没有聊天记录")==0)
    {
        printf("\t\t\t\t没有聊天记录\n");
        sleep(2);
    }
    else 
    {
        while(1)
        {
            system("clear");
            cJSON * chatres=cJSON_Parse(result);
            cJSON *list = cJSON_GetObjectItem(chatres,"list");
            int size=cJSON_GetArraySize(list);
            for(int i=0;i<size;i++)//最重要的!!!!
            {
                cJSON *arr=cJSON_GetArrayItem(list,i);
                cJSON *chattime=cJSON_GetObjectItem(arr,"chattime");
                printf("%s :",chattime->valuestring);
                cJSON *fromname=cJSON_GetObjectItem(arr,"fromname");
                printf("%s\n",fromname->valuestring);
                cJSON *chat=cJSON_GetObjectItem(arr,"chat");
                printf("%s\n",chat->valuestring);
                putchar('\n');
            }
            printf("\n\t\t\t\t是否返回上一级(yes/no):");
            char yes[20];
            scanf(" %s",yes);
            getchar();
            if(strcmp(yes,"yes")==0)
                break;
        }
    }
}
void chat_withfriend(char *name)
{
    printf("\t\t\t\t请输入您要发送信息的好友:");
    char result[1000];//数组开大才可以接受
    char words[10000];
    memset(words,0,sizeof(words));
    scanf(" %s",result);
    getchar();
    while(1)
    {
        system("clear");
        cJSON *case1=cJSON_CreateObject();
        cJSON_AddStringToObject(case1,"toname",result);
        cJSON_AddStringToObject(case1,"fromname",name);
        cJSON_AddNumberToObject(case1,"type",8);
        cJSON_AddNumberToObject(case1,"casenum",1);//先查询聊天记录
        char *sendout1=cJSON_Print(case1);
        send(client_fd,sendout1,strlen(sendout1),0);
        memset(words,0,sizeof(words));
        if(recv(client_fd,words,sizeof(words),0)<0)
        {
            my_error("recv case 1",__LINE__);
        }
        if(strcmp(words,"没有聊天记录")==0)
        {
            printf("\t\t\t\t没有聊天记录\n");
            printf(START);
        }
        else 
        {
            cJSON * chatres=cJSON_Parse(words);
            cJSON *list = cJSON_GetObjectItem(chatres,"list");
            int size=cJSON_GetArraySize(list);
            for(int i=0;i<size;i++)//最重要的!!!!
            {
                cJSON *arr=cJSON_GetArrayItem(list,i);
                cJSON *chattime=cJSON_GetObjectItem(arr,"chattime");
                printf("%s :",chattime->valuestring);
                cJSON *fromname=cJSON_GetObjectItem(arr,"fromname");
                printf("%s\n",fromname->valuestring);
                cJSON *chat=cJSON_GetObjectItem(arr,"chat");
                printf("%s\n",chat->valuestring);
                putchar('\n');
            }
            fflush(stdin);
            printf(START);
        }

        memset(words,0,sizeof(words));
        printf("\n\t\t\t\t请输入你想发送的内容(输入quit结束):");
        fgets(words,1000,stdin);
        words[strlen(words)-1]='\0';
        if(strcmp(words,"quit")==0)
        {
            // send(client_fd,"quit",10,0);//！！！！如果send的话，服务器recv一直在阻塞
            break;
        }
        cJSON *root=cJSON_CreateObject();
        cJSON_AddStringToObject(root,"toname",result);
        cJSON_AddStringToObject(root,"fromname",name);
        cJSON_AddNumberToObject(root,"type",8);
        cJSON_AddNumberToObject(root,"casenum",2);
        cJSON_AddStringToObject(root,"chat",words);
        char *sendout=cJSON_Print(root);
        send(client_fd,sendout,strlen(sendout),0);
    }
}

int  log_after_group(char *name)
{
    int choice;
    printf("\t\t\t\t请输入你的选项:");
    scanf(" %d",&choice);
    switch(choice)
    {
        case 1:
            create_group(name);
            break;
        case 2:
             dissolve_group(name);
             break;
        case 3:
            manage_group(name);
            break;
        case 4:
            group_ask(name);
            break;
        //   case 5:
        //         chat_group(name);
        //         break;
        // default
        //     break;
    }
    return choice;

}

void create_group(char *name)
{
    char num[20];
    while(1)
    {
        printf("请输入群号(长度为6，必须含有字母):");
        int flag=0;
        scanf(" %s%*c",num);
        for(int i=0;i<strlen(num);i++)
        {
            if((num[i]>='a'&&num[i]<='z')||(num[i]>='A'&&num[i]<='Z'))
            {
                flag=1;
                break;
            }
        }
        if(strlen(num)!=6||flag!=1)
            printf("群号不符合要求");
        if(flag&&strlen(num)==6)
            break;
    }
    char result[400];
    memset(result,0,sizeof(result));
    sprintf(result,"create table %s (chattime timestamp,name varchar(20),"\
    "chat text,PRIMARY KEY(chattime))DEFAULT CHARSET=utf8;",num);
    cJSON *root=cJSON_CreateObject();
    cJSON_AddStringToObject(root,"table",result);
    cJSON_AddNumberToObject(root,"type",10);
    cJSON_AddStringToObject(root,"group",num);
    cJSON_AddStringToObject(root,"name",name);
    char *out=cJSON_Print(root);
    send(client_fd,out,strlen(out),0);
    printf("\t\t\t\t创建成功!\n");
    sleep(2);
}

void dissolve_group(char *name)
{
    printf("请输入你要解散的群:\n");
    char result[100];
    scanf(" %s%*c",result);
    cJSON *root=cJSON_CreateObject();
    cJSON_AddStringToObject(root,"group",result);
    cJSON_AddNumberToObject(root,"type",11);
    cJSON_AddStringToObject(root,"name",name);
    char *sendout=cJSON_Print(root);
    send(client_fd,sendout,strlen(sendout),0);
    memset(result,0,sizeof(result));
    if(recv(client_fd,result,sizeof(result),0)<0)
    {
        my_error("dissolve",__LINE__);
    }
    printf("%s",result);
}

void manage_group(char *name)
{
    while(1)
    {
        system("clear");
        printf("\t\t\t\t1、踢出群\n");
        printf("\t\t\t\t2、邀请加入群\n");
        printf("\t\t\t\t3、返回上一层\n");
        int choice;
        scanf(" %d%*c",&choice);
        if(choice==3)
            break;
        switch(choice)
        {
            case 1:
                {
                    while(1)
                    {
                        system("clear");
                        char people[500];
                        char group[100];
                        printf("请输入你要踢出的人:");
                        scanf(" %s%*c",people);
                        printf("请输入要踢出的群:");
                        scanf(" %s%*c",group);
                        cJSON *root=cJSON_CreateObject();
                        cJSON_AddStringToObject(root,"name",people);
                        cJSON_AddStringToObject(root,"group",group);
                        cJSON_AddNumberToObject(root,"type",12);
                        cJSON_AddNumberToObject(root,"casenum",1);
                        char *out=cJSON_Print(root);
                        send(client_fd,out,strlen(out),0);
                        memset(people,0,sizeof(people));
                        if(recv(client_fd,people,sizeof(people),0)<0)
                        {
                            my_error("manage case 1",__LINE__);
                        }
                        printf(" %s",people);
                        printf("是否退出(yes/no):");
                        char yes[100];
                        scanf(" %s%*c",yes);
                        if(strcmp(yes,"yes")==0)
                            break;
                    }
                }
                break;
            case 2:
            {
                while(1)
                {
                    system("clear");
                    char people[100];
                    char group[100];
                    printf("请输入你要邀请的人:");
                    scanf(" %s%*c",people);
                    printf("请输入你要邀请的群:");
                    scanf(" %s%*c",group);
                    cJSON *root=cJSON_CreateObject();
                    cJSON_AddStringToObject(root,"toname",people);
                    cJSON_AddStringToObject(root,"fromname",name);
                    cJSON_AddStringToObject(root,"group",group);
                    cJSON_AddNumberToObject(root,"type",12);
                    cJSON_AddNumberToObject(root,"casenum",2);
                    char *out=cJSON_Print(root);
                    send(client_fd,out,strlen(out),0);
                    memset(people,0,sizeof(people));
                    if(recv(client_fd,people,sizeof(people),0)<0)
                    {
                        my_error("manage case 2",__LINE__);
                    }
                    printf("%s\n",people);
                    memset(people,0,sizeof(people));
                    printf("是否返回上一层(yes/no):");
                    scanf(" %s%*c",people);
                    if(strcmp(people,"yes")==0)
                        break;
                }
            }
                break;
        }
    }

}

void group_ask(char *name)
{
    while(1)
    {
        system("clear");
        printf("1、您发送的群请求\n");
        printf("2、您收到的好友请求\n");
        printf("3、返回上一层\n");
        int choice;
        scanf(" %d%*c",&choice);
        if(choice==3)
            break;
        switch(choice)
        {
            case 1:
                {
                    while(1)
                    {
                        char result[10000];
                        cJSON *root=cJSON_CreateObject();
                        cJSON_AddStringToObject(root,"fromname",name);
                        cJSON_AddNumberToObject(root,"type",13);
                        cJSON_AddNumberToObject(root,"casenum",1);
                        char *out=cJSON_Print(root);
                        send(client_fd,out,strlen(out),0);
                        memset(result,0,sizeof(result));
                        if(recv(client_fd,result,sizeof(result),0)<0)
                        {
                            my_error("ask case 1",__LINE__);
                        }
                        fprintf(stderr,"\033[32m$#%s#$\033[0m", result);  
                        if(strcmp(result,"没有发送群邀请")==0)
                            printf("\t\t\t\t%s\n",result);
                        else
                        {
                            cJSON *res=cJSON_Parse(result);
                            cJSON *list=cJSON_GetObjectItem(res,"list");
                            int size=cJSON_GetArraySize(list);
                            cJSON *arr,*final;
                            for(int i=0;i<size;i++)
                            {
                                arr=cJSON_GetArrayItem(list,i);
                                cJSON *toname=cJSON_GetObjectItem(arr,"toname");
                                cJSON *group=cJSON_GetObjectItem(arr,"group");
                                printf("%d、 你邀请%s加入群:%s\n",i+1,toname->valuestring,group->valuestring);

                            }
                        }

                        printf("是否退出(yes/no):");
                        char yes[100];
                        scanf(" %s%*c",yes);
                        if(strcmp(yes,"yes")==0)
                            break;
                    }
                    
                }
                break;
            case 2:
            {
                char result[10000];
                cJSON *root=cJSON_CreateObject();
                cJSON_AddStringToObject(root,"tomname",name);
                cJSON_AddNumberToObject(root,"type",13);
                cJSON_AddNumberToObject(root,"casenum",2);
                char *out=cJSON_Print(root);
                send(client_fd,out,strlen(out),0);
                memset(result,0,sizeof(result));
                if(recv(client_fd,result,sizeof(result),0)<0)
                {
                    my_error("ask case 2",__LINE__);
                }
                if(strcmp(result,"没有收到群邀请")==0)
                {
                    printf("\t\t\t\t%s\n",result);
                }
                else
                {
                    cJSON *res=cJSON_Parse(result);
                    cJSON *list=cJSON_GetObjectItem(res,"list");
                    int size=cJSON_GetArraySize(list);
                    cJSON *arr,*final;
                    for(int i=0;i<size;i++)
                    {
                        arr=cJSON_GetArrayItem(list,i);
                        cJSON *fromname=cJSON_GetObjectItem(arr,"fromname");
                        cJSON *group=cJSON_GetObjectItem(arr,"group");
                        printf("%d、 %s邀请你加入群:%s",i+1,fromname->valuestring,group->valuestring);

                    }
                }
                while(1)
                {
                    printf("请输入你想加入的群号码(quit退出):");
                    char agree[100];
                    scanf(" %s%*c",yes);
                    if(strcmp(yes,"yes")==0)
                        break;
                }
                
            }
                break;
        }
    }

}

/*void read_group(char *name)
{

}*/
void log_up(int client_fd)//注册
{
    char passwd1[20],passwd2[20],name[100];
    int i=0,count=0;
    while(1)
    {
        while(1)
        {
            if(i!=0)
                log_in_ui();
            printf("\t\t\t请输入注册账户(不要包含空格):\n \t\t\t请输入8位密码(数字字母均可):\033[1A\033[1C");
            fgets(name,100,stdin);
            if(strcmp(name,"\n")==0)
                {
                    printf("\n\t\t\t\t用户名非法，请重新输入!\n");
                    sleep(2);
                    system("clear");
                    i=1;
                    continue;
                }
            name[strlen(name)-1]='\0';
            fprintf(stderr,"\033[52C");
            break;
        }
        i=0;
            while(1)//第一次输入密码
        {
            if(i!=0)
                printf("\t\t\t\t请重新输入8位密码:");
            for(i=0;;)
            {
                char t=getch();
                if(t=='\n'||t=='\r')
                {
                    if(i!=0)
                    {
                        passwd1[i]='\0';
                        break;
                    }
                    else 
                        continue;
                }
                if(t==127&&i>=1)
                {
                    printf("\b \b");
                    passwd1[--i]='\0';
                }
                if (t != 127)
                {
                    printf("*");
                    passwd1[i++] = t;
                }
                if(i<0)
                {
                    passwd1[0]='\0';
                    i=0;
                } 
            }
            if(i==8)
                break;
            if(i!=8)
                printf("\n\t\t\t\t输入不是8位，请重新输入!\n");
        }
        i=0;
        printf("\n\t\t\t请验证密码:");
        while(1)//第二次輸入密碼
        {
            if(i!=0)
                printf("请再次输入密码:");
            for(i=0;;)
            {
                char t=getch();
                if(t=='\n'||t=='\r')
                {
                    passwd2[i]='\0';
                    break;
                }
                if(t==127&&i>=1)
                {
                    printf("\b \b");
                    passwd2[--i]='\0';
                }
                if (t != 127)
                {
                    printf("*");
                    passwd2[i++] = t;
                }
                if(i<0)
                {
                    passwd2[0]='\0';
                    i=0;
                } 
            }
            if(i==8&&strcmp(passwd2,passwd1)==0)//比較兩次輸入一致不一致
                break;
            else 
                printf("\n与第一次输入密码不符合，请重新输入!\n");
        }
            char question[100];
            char answer[100];
            printf("\n\t\t\t请输入密保问题(不要包含空格):");
            fgets(question,100,stdin);
            question[strlen(question)-1]='\0';
            printf("\t\t\t请输入密保答案(不要包含空格):");
            fgets(answer,100,stdin);
            answer[strlen(answer)-1]='\0';
            cJSON * root;
            root=cJSON_CreateObject();
            cJSON_AddStringToObject(root,"name",name);
            cJSON_AddStringToObject(root,"password",passwd1);
            cJSON_AddStringToObject(root,"question",question);
            cJSON_AddStringToObject(root,"answer",answer);
            cJSON_AddNumberToObject(root,"type",1);
            char *out=cJSON_Print(root);
            if(send(client_fd,out,strlen(out),0)<0)
            {
                my_error("send failed",__LINE__);
            }
            char buf[20];
            if(recv(client_fd,buf,sizeof(buf),0)<0)
            {
                my_error("recv failed!",__LINE__);
            }
            if(strcmp(buf,"注册成功!")==0)
            {
                printf("\n\t\t\t\t%s\n",buf);
                sleep(3);
                break;//退出大循环
            }
            else 
            {
                printf("\n                \t\t注册失败，请重新输入\n");
                sleep(2);
                i=1;
            }
    }
}

int getch()//getch实现
{
    int c=0;
    struct termios org_opts, new_opts;
    int res=0;
    //-----  store old settings -----------
     res=tcgetattr(STDIN_FILENO, &org_opts);
     assert(res==0);
    //---- set new terminal parms --------
    memcpy(&new_opts, &org_opts, sizeof(new_opts));
    new_opts.c_lflag &= ~(ICANON | ECHO | ECHOE | ECHOK | ECHONL | ECHOPRT | ECHOKE | ICRNL);
    tcsetattr(STDIN_FILENO, TCSANOW, &new_opts);
    c=getchar();
    //------  restore old settings ---------
    res=tcsetattr(STDIN_FILENO, TCSANOW, &org_opts);
    assert(res==0);
    return c;
}

void find_passwd(int client_fd)//找回密码
{
    char name[20];
    char question[100];
    char answer[100];
    char passwd[20];
    int i=0;
    while(1)
    {
        while(1)
        {
            if(i!=0)
                log_in_ui();
            printf("\t\t\t\t请输入你要找回的用户名:");
            fgets(name,100,stdin);
            if(strcmp(name,"\n")==0)
            {
                printf("\t\t\t\t用户名非法,请重新输入!\n");
                i=1;
                sleep(2);
                continue;
            }
            name[strlen(name)-1]='\0';
            printf("\t\t\t\t请输入密保问题");
            fgets(question,100,stdin);
            if(strcmp(question,"\n")==0)
            {
                printf("\t\t\t\t密保问题非法,请重新输入！\n");
                i=1;
                sleep(2);
                continue;
            }
            question[strlen(question)-1]='\0';
            printf("\t\t\t\t请输入密保答案:");
            if(strcmp(answer,"\n")==0)
            {
                printf("\t\t\t\t密保答案非法,请重新输入!\n");
                i=1;
                sleep(2);
                continue;
            }
            fgets(answer,100,stdin);
            answer[strlen(answer)-1]='\0';
        }
        printf("\t\t\t\t请输入修改后的密码:");
        i=0;
        while(1)
        {
            if(i!=0)
                printf("\t\t\t\t\n请再次输入密码:");
            for(i=0;;)
            {
                char t=getch();
                if(t=='\n'||t=='\r')
                {
                    passwd[i]='\0';
                    break;
                }
                if(t==127&&i>=1)
                {
                    printf("\b \b");
                    passwd[--i]='\0';
                }
                if (t != 127)
                {
                    printf("*");
                    passwd[i++] = t;
                }
                if(i<0)
                {
                    passwd[0]='\0';
                    i=0;
                } 
            }
            if(i!=8)
                printf("\t\t\t\t输入密码长度不足！\n");
            if(i==8)
                break;
        }

        cJSON * root;
        root=cJSON_CreateObject();
        cJSON_AddStringToObject(root,"name",name);
        cJSON_AddStringToObject(root,"password",passwd);
        cJSON_AddStringToObject(root,"question",question);
        cJSON_AddStringToObject(root,"answer",answer);
        cJSON_AddNumberToObject(root,"type",2);
        char *out=cJSON_Print(root);
        if(send(client_fd,out,strlen(out),0)<0)
        {
            my_error("send failed",__LINE__);
        }
        char buf[100];
        memset(buf,0,sizeof(buf));
        if(recv(client_fd,buf,sizeof(buf),0)<0)
        {
            my_error("recv failed",__LINE__);
        }
        if(strcmp(buf,"输入信息错误")==0)
            printf("\t\t\t\t\n您输入的信息错误，请重新输入!\n");
        else
        { 
            printf("\n\n\n%20s\t\t\t%s\n"," ",buf);
            sleep(2);
            break;
        }
    }
}

void my_error(char *string,int line)
{
    fprintf(stderr,"line:%d ",line);
    perror(string);
    exit(1);
}
