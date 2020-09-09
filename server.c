/*
	Assignment 3
	Group no. 32
*/

// DNS Server
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>

#define PORT 8080
#define QUEUE_LENGTH 3

int main(int argc, char const *argv[]) {
	
	int dns_server_socket;
	int opt = 1;
	

	// Creating socket file descriptor for listening at the DNS Server
	if ((dns_server_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
		perror("Socket Failure : ");
		exit(EXIT_FAILURE);
	}

	// Setting options to allow for reuse of the port and DNS Address
	if (setsockopt(dns_server_socket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
		perror("Socket Options Failure : ");
		exit(EXIT_FAILURE);
	}

	// Setting the DNS server IP Address and Port
	struct sockaddr_in dns_address;

	dns_address.sin_family = AF_INET;
	dns_address.sin_addr.s_addr = INADDR_ANY;
	dns_address.sin_port = htons( PORT );

	// Attaching socket to the address & port
	if (bind(dns_server_socket, (struct sockaddr *)&dns_address, sizeof(dns_address))<0) {
		perror("Bind Failure : ");
		exit(EXIT_FAILURE);
	}

	// Listening on the created socket
	if (listen(dns_server_socket, QUEUE_LENGTH) < 0) {
		perror("Socket Listen Failure : ");
		exit(EXIT_FAILURE);
	}

	struct sockaddr_in proxy_address;
	char buffer_in[1024] = {0}, buffer_out[1024] = {0} ;
	FILE *database_fp;
	int found_flag;
	char ip_address[1023], domain_name[1023];

	int dns_connection_socket;
	int proxy_address_len= sizeof(proxy_address);
	
	while(1) {

		
		if ((dns_connection_socket = accept(dns_server_socket, (struct sockaddr *)&proxy_address, (socklen_t*)&proxy_address_len))<0) {
			perror("Socket Accept Failure : ");
			exit(EXIT_FAILURE);
		}

		// empty the current buffers
		memset(&buffer_in, '\0', 1024);
		memset(&buffer_out, '\0', 1024);

		
		read( dns_connection_socket , buffer_in, 1024);
		printf("Request :\tType-%c,\tMessage-%s\n",buffer_in[0], buffer_in+1 );

		
		
		
		if(buffer_in[0]=='1' || buffer_in[0]=='2'){
			// Scan the database & send the responding message if found in the database
			found_flag = 0;
			database_fp = fopen("database.csv","r");
			while(fscanf(database_fp, "%[^,],%[^\n]\n", domain_name, ip_address) != -1){
				// Recieved message contains Domain name, requesting IP address
				if(buffer_in[0]=='1'){
					if(strcmp(domain_name, buffer_in+1) == 0){
						found_flag = 1;
						buffer_out[0] = '3';
						
						strcat(buffer_out, ip_address);
						send(dns_connection_socket, buffer_out, strlen(buffer_out), 0);
						break;
					}
				}
				// Recvd message contains IP address, requesting Domain name
				else{
					if(strcmp(ip_address, buffer_in+1) == 0){
						found_flag = 1;
						buffer_out[0] = '3';
						strcat(buffer_out, domain_name);
						send(dns_connection_socket, buffer_out, strlen(buffer_out), 0);
						break;
					}

				}
			}
			fclose(database_fp);

			// If not found in the database, then send a not found message
			if (found_flag == 0) {
				strcpy(buffer_out, "4Entry not found in the database");
				send(dns_connection_socket, buffer_out, strlen(buffer_out), 0);
			}
		}

		else{
			// If incorrect format, then send error type message.
			strcpy(buffer_out, "4Wrong Packet Format");
			send(dns_connection_socket, buffer_out, strlen(buffer_out) , 0 );
			fprintf(stderr, "%s\n", "Wrong Packet Format Recvd");

		}

		printf("Response :\tType-%c,\tMessage-%s\n", buffer_out[0], buffer_out+1);
		printf("%s\n", "______________________________________________________________________________");
		
		close(dns_connection_socket);
	}
	return 0;
}
