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

#define TRUE 1
#define FALSE 0
#define PORT 8888
#define BUFFER_SIZE 5
const int MAX_CLIENTS = 1;

// the static IP address determined when making RPi a WiFi AP
const char* addr = "192.168.5.1";

// number of exchanges (two-way) we wish to have with each client per second
int fsamp = 100;

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


int main(int argc , char *argv[])
{
	srand(time(NULL));
	long dt_us = 1000000/fsamp;
	int opt = TRUE;
	int master_socket , addrlen , new_socket , client_socket[30] ,
		max_clients = MAX_CLIENTS, activity, i , valread , sd;
	int max_sd;
	struct sockaddr_in address;
	char buffer[BUFFER_SIZE]; //data buffer of 1K

	//set of socket descriptors
	fd_set readfds;

	//initialise all client_socket[] to 0 so not checked
	for (i = 0; i < max_clients; i++)
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
	if( setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt,
		sizeof(opt)) < 0 )
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


	int msgcount = 0;

	clock_t sample_timer = clock();

	int base_station = 0;

	while(TRUE)
	{
		if (((double)(clock() - sample_timer))/CLOCKS_PER_SEC > 1.0) {
			printf("msg count is %d\n", msgcount);
			msgcount = 0;
			sample_timer = clock();
		}

		//clear the socket set
		FD_ZERO(&readfds);

		//add master socket to set
		FD_SET(master_socket, &readfds);
		max_sd = master_socket;

		//add child sockets to set
		for ( i = 0 ; i < max_clients ; i++)
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
			for (i = 0; i < max_clients; i++)
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
		for (i = 0; i < max_clients; i++)
		{
			sd = client_socket[i];

			if (FD_ISSET( sd , &readfds))
			{

				if (sd != base_station) {
					printf("reading\n");
					read (sd, read_msg.bytes, sizeof(read_msg.bytes));
					printf("%f %f\n", read_msg.floats[0], read_msg.floats[1]);
					bzero(read_msg.bytes, sizeof(read_msg.bytes));
					msgcount++;
				}
				else {
					printf("Not reading\n");
				}
			}

		}

		// TODO: compute the torque control here


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

		// iterate over clients again (which are now waiting for reply) and send reply torques
		for (i = 0; i < max_clients; i++)
		{
			sd = client_socket[i];
			if (sd == base_station) {
				// send all floats
				write(sd, all_data.bytes, sizeof(all_data.bytes));
			}
			else {
				reply_msg.flt = torques[i];
				write(sd, reply_msg.bytes, 4);
			}
 		}

		usleep(dt_us);
		sample_timer -= dt_us;

	}

	return 0;

}
