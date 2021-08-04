#include <sys/types.h>
#include <sys/socket.h>
#include <iostream>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <utility>
#include <thread>
#include <chrono>
#include <functional>
#include <atomic>
#define MYPORT  8000
#define BUFFER_SIZE 2048
#define TAG "Admin"
#define INET_ADDR "127.0.0.1"
int main(int argc,char *argv[]){
    std::string username="";
    std::cout<<"Set your user name:";
    std::cin>>username;
    username=username+"@"+TAG+"~# ";
    std::fflush(stdin);
    int sock_cli;
    fd_set rfds;
    struct timeval tv;
    int retval, maxfd;
    sock_cli = socket(AF_INET,SOCK_STREAM, 0);
    struct sockaddr_in servaddr;
    memset(&servaddr, 0,sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(MYPORT);
    servaddr.sin_addr.s_addr = inet_addr(INET_ADDR);
 
    if (connect(sock_cli, (struct sockaddr *)&servaddr,sizeof(servaddr)) < 0){
        perror("connect");
        exit(1);
    }
 
    while(1){
        FD_ZERO(&rfds);
        FD_SET(0, &rfds);
        maxfd = 0;
        FD_SET(sock_cli, &rfds);
        if(maxfd < sock_cli){
            maxfd = sock_cli;
        }
        tv.tv_sec = 10;
        tv.tv_usec = 0;
        retval = select(maxfd+1, &rfds, NULL, NULL, &tv);
        if(retval == -1){
            std::cerr<<"select error, service has been shutdown"<<std::endl;
            break;
        }else if(retval == 0){
            continue;
        }else{
            if(FD_ISSET(sock_cli,&rfds)){
                char recvbuf[BUFFER_SIZE];
                recv(sock_cli, recvbuf,sizeof(recvbuf),0);
                std::cout<< recvbuf;
                memset(recvbuf, 0,sizeof(recvbuf));
            }
            if(FD_ISSET(0, &rfds)){
                char sendbuf[BUFFER_SIZE];
                char getbuff[BUFFER_SIZE-sizeof(username)];
                fgets(getbuff,sizeof(getbuff), stdin);
                if (strcmp(getbuff, "-Q")==0) {
                    break;
                }
                strcpy(sendbuf, username.data());
                strcat(sendbuf, getbuff);
                send(sock_cli, sendbuf,strlen(sendbuf),0);
                memset(sendbuf, ' ',sizeof(sendbuf));
            }
        }
    }
 
    return 0;
}
