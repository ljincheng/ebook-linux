
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include "book-meta.h"

#define MAXLINE		10
#define OPEN_MAX	100
#define LISTENQ		20
#define SERV_PORT	5555
#define INFTIM		1000



void setnonblocking(int sock)
{
	int opts;
	opts = fcntl(sock, F_GETFL);

	if(opts < 0) {
		perror("fcntl(sock, GETFL)");
		exit(1);
	}

	opts = opts | O_NONBLOCK;

	if(fcntl(sock, F_SETFL, opts) < 0) {
		perror("fcntl(sock, SETFL, opts)");
		exit(1);
	}
}

int main(int argc, char *argv[])
{
	printf("epoll socket begins.\n");
	int i, maxi, listenfd, connfd, sockfd, epfd, nfds;
	ssize_t n;
	char line[MAXLINE];
	socklen_t clilen;

	struct epoll_event ev, events[20];

	epfd = epoll_create(256);

	struct sockaddr_in clientaddr;
	struct sockaddr_in serveraddr;

	listenfd = socket(AF_INET, SOCK_STREAM, 0);

	setnonblocking(listenfd);

	ev.data.fd = listenfd;
	ev.events = EPOLLIN | EPOLLET;

	epoll_ctl(epfd, EPOLL_CTL_ADD, listenfd, &ev);

	bzero(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	char *local_addr = "127.0.0.1";
	inet_aton(local_addr, &(serveraddr.sin_addr));
	serveraddr.sin_port = htons(SERV_PORT);

	bind(listenfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr));

	listen(listenfd, LISTENQ);

	maxi = 0;
	BookBuffer *buf=NULL;
BookBuffer *headbuf=NULL;
int startRead=0;
	for(; ;) {
		nfds = epoll_wait(epfd, events, 20, 500);

		for(i = 0; i < nfds; ++i) {
			if(events[i].data.fd == listenfd) {
				printf("accept connection, fd is %d\n", listenfd);
				connfd = accept(listenfd, (struct sockaddr *)&clientaddr, &clilen);
				if(connfd < 0) {
					perror("connfd < 0");
					exit(1);
				}

				setnonblocking(connfd);

				char *str = inet_ntoa(clientaddr.sin_addr);
				printf("connect from %s\n", str);

				ev.data.fd = connfd;
				ev.events = EPOLLIN | EPOLLET;
				epoll_ctl(epfd, EPOLL_CTL_ADD, connfd, &ev);
buf=book_buffer_init(1024);
headbuf=book_buffer_init(10);
			}
			else if(events[i].events & EPOLLIN) {
				if((sockfd = events[i].data.fd) < 0) continue;
				if((n = read(sockfd, line, MAXLINE)) < 0) {
					if(errno == ECONNRESET) {
						close(sockfd);
						events[i].data.fd = -1;
					} else {
						printf("readline error");
					}
				} else if(n == 0) {
					close(sockfd);
					events[i].data.fd = -1;
				}
//printf("received data:len=%d,data= %s\n", n,line);
if(startRead==0 && n>5)
{
book_buffer_addraw(headbuf,line,10);
startRead=1;
}
				book_buffer_addraw(buf,line,n);
				//printf("received data: %s\n", line);

				ev.data.fd = sockfd;
				ev.events = EPOLLOUT | EPOLLET;
				epoll_ctl(epfd, EPOLL_CTL_MOD, sockfd, &ev);
			}
			else if(events[i].events & EPOLLOUT) {
				sockfd = events[i].data.fd;
				write(sockfd, line, n);
if(startRead==1)
{

char *tempstr=NULL;

headbuf->offset=0;
uint8_t head_type=book_buffer_get8(headbuf); 
uint32_t strLen=book_buffer_get32(headbuf);
size_t buflen=strlen(buf->data+5);
//printf("buflen=%d,strLen=%d\n",buflen,strLen);
if(buflen==strLen)
{
buf->offset=0;
uint8_t msg_type=book_buffer_get8(buf); 
uint32_t dataLen=book_buffer_get32(buf);
if(dataLen==strLen)
{
char *msg=malloc(strLen + 1);
book_buffer_getstring(msg,buf,strLen);
				//
printf("获取数据结果:type=%d,len=%d,data= %s\n", msg_type,strLen,msg);
free(msg);
book_buffer_free(buf);
book_buffer_free(headbuf);
}
}
}

//printf("written data: %s\n", line);
				ev.data.fd = sockfd;
				ev.events = EPOLLIN | EPOLLET;
				epoll_ctl(epfd, EPOLL_CTL_MOD, sockfd, &ev);
			}
		}
	}
}
