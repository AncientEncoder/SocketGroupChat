#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <iostream>
#include <thread>
#include <list>

#define PORT 8000
#define INET_ADDR "127.0.0.1"
#define BUFFER_SIZE 2048
#define Passwd "1234567890"
int s;
struct sockaddr_in servaddr;
socklen_t len;
std::list<int> li;

void getConn(){
    while(true){
        int conn = accept(s, (struct sockaddr*)&servaddr, &len);
        li.push_back(conn);
        std::cout<<"New Client has been connected"<<std::endl;
    }
}

void getMsg(){
    struct timeval tv;
    tv.tv_sec = 2;
    tv.tv_usec = 0;
    while(true){
        std::list<int>::iterator it;
        for(it=li.begin(); it!=li.end(); ++it){
            fd_set rfds;
            FD_ZERO(&rfds);
            int maxfd = 0;
            int retval = 0;
            FD_SET(*it, &rfds);
            if(maxfd < *it){
                maxfd = *it;
            }
            retval = select(maxfd+1, &rfds, NULL, NULL, &tv);
            if(retval == -1){
                std::cerr<<"select error\n";
            }
            else if(retval == 0){
                continue;
            }else{
                char buf[BUFFER_SIZE];
                memset(buf, 0 ,sizeof(buf));
                recv(*it, buf,sizeof(buf), 0);
                std::list<int>::iterator reit;
                for (reit=li.begin(); reit!=li.end(); reit++) {
                    if (reit!=it) {
                        send(*reit, buf, sizeof(buf), 0);
                    }
                }
                std::cout<<buf;
            }
        }
        sleep(1);

    }
}

void sendMsg(){
    while(true){
        char buf[BUFFER_SIZE];
        char bufget[BUFFER_SIZE-sizeof("server: ")];
        fgets(bufget,sizeof(bufget), stdin);
        strcpy(buf, "server :");
        strcat(buf, bufget);
        std::list<int>::iterator it;
        for(it=li.begin(); it!=li.end(); ++it){
            send(*it, buf,sizeof(buf), 0);
        }
    }
}
int main(int argc, char* argv[]){
    s = socket(AF_INET, SOCK_STREAM, 0);
    memset(&servaddr, 0,sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    servaddr.sin_addr.s_addr = inet_addr(INET_ADDR);
    if(bind(s, (struct sockaddr* ) &servaddr,sizeof(servaddr))==-1){
        perror("bind");
        exit(1);
    }
    if(listen(s, 20) == -1){
        perror("listen");
        exit(1);
    }
    len =sizeof(servaddr);
    std::thread t(getConn);
    t.detach();
    std::thread t1(sendMsg);
    t1.detach();
    std::thread t2(getMsg);
    t2.detach();
    while(true){
    }
    return 0;
}
