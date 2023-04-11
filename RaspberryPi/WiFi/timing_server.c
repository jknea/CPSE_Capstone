//Example code: A simple server side code, which echos back the received message.
//Handle multiple socket connections with select and fd_set on Linux
#include <stdio.h>
#include <string.h> //strlen
#include <stdlib.h>
#include <errno.h>
#include <unistd.h> //close
#include <arpa/inet.h> //close
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros
#include <time.h>

#include <math.h>
#include <sys/ioctl.h> // compile with -lm to link math library


#include <signal.h>

#define TRUE 1
#define FALSE 0
#define PORT 8888
#define BUFFER_SIZE 5

const int MAX_CLIENTS = 1;

const int IMU_BUFF_SIZE = 256;

int client_socket[30];

int master_socket;



void sigint(int sig)
{
	printf("caught ctrl-c\n");
	// close the clients
	for ( int i = 0 ; i < MAX_CLIENTS; i++)
	{
		//socket descriptor
		int sd = client_socket[i];
		close(sd);
	}
	close(master_socket);
	exit(0);
}



long min_sent = 1000000;
long iter = 0;


// the static IP address determined when making RPi a WiFi AP
const char* addr = "192.168.5.1";



// number of exchanges (two-way) we wish to have with each client per second
int fsamp = 1;

// union for swapping 1 float with 4 bytes -- this is for client to receive computed control torque
union float_union1 {
  unsigned char bytes[4];
  float flt;
};

// union for swapping 3 float with 8 bytes -- this is for client to send motor and IMU angles
union float_union2 {
  unsigned char bytes[8];
  float floats[2];
};

// union for swapping 3 float with 8 bytes -- this is for client to send motor and IMU angles
union float_union7 {
  unsigned char bytes[28];
  float floats[7];
};

union float_union1 reply_msg;
union float_union2 read_msg;
union float_union7 all_data;


int msgcount = 0;

int base_station = 0;

// for computing elapsed time
struct timeval start, end;

clock_t sample_timer;

long seconds, micros;



int bytes_avail;






int main(int argc , char *argv[])
{

	unsigned char imu_buff[IMU_BUFF_SIZE];
	signal(SIGINT, sigint);

	srand(time(NULL));
	long dt_us = 1000000/fsamp;

	int opt = TRUE;
	int  addrlen , new_socket;
	int activity, i , valread , sd;
	int max_sd;
	struct sockaddr_in address;
	char buffer[BUFFER_SIZE];

	//set of socket descriptors
	fd_set readfds;

	//initialise all client_socket[] to 0 so not checked
	for (i = 0; i < MAX_CLIENTS; i++)
	{
		client_socket[i] = 0;
	}

	//create a master socket
	if( (master_socket = socket(AF_INET , SOCK_STREAM , 0)) == 0)
	{
		perror("socket failed");
		exit(EXIT_FAILURE);
	}

	//set master socket to allow multiple connections ,
	//this is just a good habit, it will work without this
	if( setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0 )
	{
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}

	//type of socket created
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = inet_addr(addr);
	address.sin_port = htons( PORT );

	//bind the socket to localhost port 8888
	if (bind(master_socket, (struct sockaddr *)&address, sizeof(address))<0)
	{
		perror("bind failed");
		exit(EXIT_FAILURE);
	}
	printf("Listener on port %d \n", PORT);

	//try to specify maximum of 3 pending connections for the master socket
	if (listen(master_socket, 3) < 0)
	{
		perror("listen");
		exit(EXIT_FAILURE);
	}

	//accept the incoming connection
	addrlen = sizeof(address);
	puts("Waiting for connections ...");



	gettimeofday(&start, NULL);
	sample_timer = clock();
	// entering infinite connection loop
	while(TRUE)
	{
		gettimeofday(&end, NULL);
		seconds = (end.tv_sec - start.tv_sec);
		micros = ((seconds*1000000) + end.tv_usec) - start.tv_usec;
		if (micros > 1000000) {
			if (msgcount < min_sent && msgcount != 0) {
				min_sent = msgcount;
			}
			iter++;
//			printf("min sent = %d, iter = %d\n", min_sent, iter);
			printf("msg count is %d, mincount = %d\n", msgcount, min_sent);
			msgcount = 0;
			gettimeofday(&start, NULL);
		}

		//clear the socket set
		FD_ZERO(&readfds);

		//add master socket to set
		FD_SET(master_socket, &readfds);
		max_sd = master_socket;

		//add child sockets to set
		for ( i = 0 ; i < MAX_CLIENTS; i++)
		{
			//socket descriptor
			sd = client_socket[i];

			//if valid socket descriptor then add to read list
			if(sd > 0) {
				FD_SET( sd , &readfds);
			}
			//highest file descriptor number, need it for the select function
			if(sd > max_sd) {
				max_sd = sd;
			}
		}
		//wait for an activity on one of the sockets , timeout is NULL ,
		//so wait indefinitely
		activity = select( max_sd + 1 , &readfds , NULL , NULL , NULL);

		if ((activity < 0) && (errno!=EINTR))
		{
			printf("select error");
		}

		//If something happened on the master socket ,
		//then its an incoming connection
		if (FD_ISSET(master_socket, &readfds))
		{
			if ((new_socket = accept(master_socket,
					(struct sockaddr *)&address, (socklen_t*)&addrlen))<0)
			{
				perror("accept");
				exit(EXIT_FAILURE);
			}
			//inform user of socket number - used in send and receive commands

			printf("New connection , socket fd is %d , ip is : %s , port : %d\n" , new_socket , inet_ntoa(address.sin_addr) , ntohs(address.sin_port));


			// TODO: add a read operation here to determine the type of client (base station or pi zero w)
			read(new_socket, read_msg.bytes, 8);
			printf("received %f, %f\n", read_msg.floats[0], read_msg.floats[1]);


			const float BASESTATION_ID = 1.1;
			if (read_msg.floats[1] == BASESTATION_ID) {
				printf("Device is a Base Station\n");
				base_station = new_socket;
			}


			//add new socket to array of sockets
			for (i = 0; i < MAX_CLIENTS; i++)
			{
				//if position is empty
				if( client_socket[i] == 0 )
				{
					client_socket[i] = new_socket;
					printf("Adding to list of sockets as %d\n" , i);
					break;
				}
			}
		}

		//else its some IO operation on some other socket
		for (i = 0; i < MAX_CLIENTS; i++)
		{
			sd = client_socket[i];

			if (FD_ISSET( sd , &readfds))
			{

				ioctl(sd, FIONREAD, &bytes_avail);
//				printf("%d bytes available for read\n", bytes_avail);


				if (sd != base_station) {
//					printf("reading\n");
//					int x = read (sd, read_msg.bytes, sizeof(read_msg.bytes))


//					for (int i = 0; i < (count / 8); i++)
//					{
//						read(sd, read_msg.bytes, 8);
//						usleep(100);
//					}

//					printf("read %d times\n", (count/8));
					bytes_avail = (bytes_avail / 8) * 8;
					int min = fmin(bytes_avail, IMU_BUFF_SIZE);
					int x = read(sd, imu_buff, fmin(bytes_avail, IMU_BUFF_SIZE));

//					read(sd, imu_buff, bytes_avail);
//						ioctl(sd, FIONREAD, &count);
//					}
					if (x != - 1) {
					 for (int i = bytes_avail - 8, j = 0; i < bytes_avail; i++, j++) {
						read_msg.bytes[j] = imu_buff[i];
					 }
					}



//					read(sd, read_msg.bytes, sizeof(read_msg.bytes));


//					printf("%d\n", x);
//					printf("%f %f\n", read_msg.floats[0], read_msg.floats[1]);
//					printf("theta = %f\n", read_msg.floats[0]);

					bzero(read_msg.bytes, sizeof(read_msg.bytes));
					msgcount++;
				}
				else {
//					printf("Not reading\n");
				}
			}

		}

		// TODO: compute the torque control here


		float Dq[5][5] = {1.1};
                float Cq[5][5] = {2.2};
		float Gq[5] = {3.3};
		for (int i = 0; i < 5; ++i) {
			for (int j = 0; j < 5; ++j) {
				Dq[i][j] *= 1;
				Cq[i][j] *= 1;
 			}
		}
		for (int i = 0 ; i < 5; ++i) {
			Gq[i] *= 1;
		}

		float torques[] = {1.1, 2.2, 3.3, 4.4, 5.5};

//		float all_data. {1.1,2.2,3.3,4.4,5.5,6.6,7.7};

		float noise = (rand() % 10)/10.0;

		all_data.floats[0] = 0.1 + noise;
		all_data.floats[1] = 1.1 + noise;
		all_data.floats[2] = 2.1 + noise;
		all_data.floats[3] = 3.1 + noise;
		all_data.floats[4] = 4.1 + noise;
		all_data.floats[5] = 5.1 + noise;
		all_data.floats[6] = 6.1 + noise;


/*
		// iterate over clients again (which are now waiting for reply) and send reply torques
		for (i = 0; i < MAX_CLIENTS; i++)
		{
			sd = client_socket[i];
			if (sd == base_station) {
				// send all floats
				write(sd, all_data.bytes, sizeof(all_data.bytes));
			}
			else {
				reply_msg.flt = torques[i];
//				int x = write(sd, reply_msg.bytes, 4);
				write(sd, reply_msg.bytes, 4);

//				printf("%d\n", x);
			}
 		}

*/
		usleep(dt_us);
		sample_timer -= dt_us;

	}

	return 0;

}
