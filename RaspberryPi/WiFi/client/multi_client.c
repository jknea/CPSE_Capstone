#include <arpa/inet.h> // inet_addr()
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h> // bzero()
#include <sys/socket.h>
#include <unistd.h> // read(), write(), close()

#include <time.h>
//#include <delay.h>

#define MAX 5
#define PORT 8888
#define SA struct sockaddr


char c0[] = {'1','2','3','4'};
char c1[] = {'a','b','c','d'};
char c2[] = {'5','6','7','8'};
char c3[] = {'e','f','g','h'};

int cidx = 0;

void func(int sockfd)
{
    char buff[MAX] = {'a', 'b', 'c', 'd'};
    int msgcount = 0;
    clock_t start_time = clock();
    long milliseconds = 10 * 1000.0;
    clock_t start = clock();
//    while (clock() < start_time + milliseconds) {
    while(1) {
        bzero(buff, sizeof(buff));
//      buff[0] = 'a';
//	buff[1] = 'b';
//	buff[2] = 'c';
//	buff[3] = 'd';
//	buff[4] = '\0';
//	printf("cidx = %d\n", cidx);
        buff[0] = c0[cidx];
	buff[1] = c1[cidx];
	buff[2] = c2[cidx];
	buff[3] = c3[cidx];
	buff[4] = '\0';
//	printf("buf is %s\n", buff);
	cidx++;
	cidx %= 4;

	printf("sent\n");
        write(sockfd, buff, sizeof(buff));
	char buff1[2] = {0};
        read(sockfd, buff1, 2);
	printf("received %s\n", buff1);
//        usleep(1000);
    }

    bzero(buff, sizeof(buff));
    buff[0] = 'e';
    buff[1] = 'x';
    buff[2] = 'i';
    buff[3] = 't';
    buff[4] = '\0';
    write(sockfd, buff, sizeof(buff));
    printf("msg count is %d\n", msgcount);
    printf("Elapsed time %f\n", (clock() - start)/1000.0);
}


int main(){

    int sockfd, connfd;
    struct sockaddr_in servaddr, cli;
    // socket create and verification
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        printf("socket creation failed...\n");
        exit(0);
    }
    else
        printf("Socket successfully created..\n");
    bzero(&servaddr, sizeof(servaddr));

    // assign IP, PORT

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr("100.90.188.13");
    servaddr.sin_port = htons(PORT);

    // connect the client socket to server socket
    if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr))
        != 0) {
        printf("connection with the server failed...\n");
        exit(0);
    }
    else
        printf("connected to the server..\n");

    // function for chat
    func(sockfd);

    // close the socket
    close(sockfd);
}
