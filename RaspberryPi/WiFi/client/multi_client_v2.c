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


union float_union1 {
    unsigned char bytes[4];
    float flt;
};

union float_union2 {
    unsigned char bytes[8];
    float floats[2];
};


union float_union1 reply;
union float_union2 flt_un;

float start_val = 3.14159;


void func(int sockfd)
{
    int msgcount = 0;
    clock_t start_time = clock();
    long milliseconds = 10 * 1000.0;
    clock_t start = clock();
    while(1) {
        bzero(flt_un.bytes, 8);
        flt_un.floats[0] = start_val;
        flt_un.floats[1] = start_val - 1;
        start_val += 1;
	if (start_val > 10000.0) {
	    start_val = 3.14;
        }
    	printf("sent\n");
        write(sockfd, flt_un.bytes, 8);         // write the angles to the server
        read(sockfd, reply.bytes, 4);   // read the command torque from the server
	    printf("received %f\n", reply.flt);
    }
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
    else {
        printf("Socket successfully created..\n");
    }

    bzero(&servaddr, sizeof(servaddr));

    // assign IP, PORT
    const char* host = "192.168.5.1";
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(host);
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
    return 0;
}
