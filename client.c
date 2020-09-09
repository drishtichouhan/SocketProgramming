// Client
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    
    //Check if input format is correct
    if(argc!=3){
            
            printf("%s\n", "Usage : ./client <proxy_ip_address> <proxy_port>");
            return 0;
    }
    
    char * proxy_ip_address = argv[1];      //IP Address of the proxy server
    char * proxy_port = argv[2];            // port of the proxy server

    int client_socket = 0;
  

    //sock_stream for TCP
    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return -1;
    }


    struct sockaddr_in proxy_socket_address;
    memset(&proxy_socket_address, '0', sizeof(proxy_socket_address));

    proxy_socket_address.sin_family = AF_INET; // Address family
    proxy_socket_address.sin_port = htons(atoi(proxy_port)); //translate short integer from host byte order to network byte order

    // Convert IP addresses from text to binary form
    if(inet_pton(AF_INET, proxy_ip_address, &proxy_socket_address.sin_addr)<=0)
    {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }
   
    char * message;
       
    char buffer[1024] = {0};
    
    while(1)
    {
        
        memset(buffer, '\0', 1024); // empty buffer
        scanf("%s",message);

        //creating socket descriptor to connect to proxy
        if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        {
            printf("\n Socket creation error \n");
            return -1;
        }

        //Establish connection with proxy server
        if (connect(client_socket, (struct sockaddr *)&proxy_socket_address, sizeof(proxy_socket_address)) < 0)
        {
            printf("\nConnection Failed \n");
            return -1;
        }

         
        send(client_socket , message , strlen(message) , 0 ); //send 
        read( client_socket , buffer, 1024); //store response in buffer
        
        printf("%s\n",buffer );
        
        close(client_socket);
        printf("%s\n", "______________________________________________________________________________");
    }

    return 0;
}