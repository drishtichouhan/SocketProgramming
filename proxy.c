/*
    Assignment 3
    Group no. 32
*/

// Proxy Server
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>


#define QUEUE_LENGTH 3  // Max cache size

//structure definition of node of Cache queue
struct CACHE
{
    char ip_address[1024];
    char domain_name[1024];
    struct CACHE * next;
};

struct CACHE * cache_head;

/*Function to check if a domain/ip_address is present in the cache*/
int find(char *entry) {
    int type = entry[0]-'0';
    char message[1023];
    strcpy(message, entry+1);
    struct CACHE *temp = cache_head;
    while (temp != NULL){
        if (type == 2 && strcmp(message, temp->ip_address)==0) {
            return 1;
        } else if (type == 1 && strcmp(message, temp->domain_name)==0) {
            return 1;
        }
        temp=temp->next;
    }
    return 0;
}

/*  Function to return the domain_name/ip_address 
    corresponding to ip_adress/domain_name respectively */
void get(char *in, char *out) {
    int type = in[0]-'0';
    char message[1023];
    strcpy(message, in+1);

    struct CACHE *temp = cache_head;
    while (temp != NULL){
        if (type == 2 && strcmp(message, temp->ip_address)==0) {
            strcpy(out, "3");
            strcat(out+1, temp->domain_name);
            return;
        } else if (type == 1 && strcmp(message, temp->domain_name)==0) {
            strcpy(out, "3");
            strcat(out+1, temp->ip_address);
            return;
        }
        temp=temp->next;
    }
    return;
}




int main(int argc, char const *argv[])
{

    if(argc != 2){
        
        printf("%s\n", "Usage : ./proxy <proxy_port>");
        return (0);
        
    }

    char * proxy_port = argv[1];

    cache_head=NULL;

    int opt = 1;

    /*connecting with dns*/

    int dns_server_socket=0;
    
    if ((dns_server_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return -1;
    }

    struct sockaddr_in dns_serv_addr;
    memset(&dns_serv_addr, '0', sizeof(dns_serv_addr));

    dns_serv_addr.sin_family = AF_INET;
    dns_serv_addr.sin_port = htons(8080);

    // Convert IP addresses from text to binary form
    if(inet_pton(AF_INET, "10.42.0.118", &dns_serv_addr.sin_addr)<=0)
    {
        printf("\nInvalid client_address/ client_address not supported \n");
        return -1;
    }

    if (connect(dns_server_socket, (struct sockaddr *)&dns_serv_addr, sizeof(dns_serv_addr)) < 0)
    {
        printf("\nConnection with DNS Failed \n");
        return -1;
    }

    close(dns_server_socket);

    /*-------------------------------------*/

    int proxy_server_socket;
    // Creating proxy server socket 
    if ((proxy_server_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Setting options to allow for reuse of the port and DNS Address
    if (setsockopt(proxy_server_socket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                                                  &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in client_address;
 	memset(&client_address, '0', sizeof(client_address));

    client_address.sin_family = AF_INET;
    client_address.sin_addr.s_addr = INADDR_ANY;
    client_address.sin_port = htons( atoi(proxy_port) );

    // Forcefully attaching socket to the proxy_port
    if (bind(proxy_server_socket, (struct sockaddr *)&client_address,
                                 sizeof(client_address))<0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(proxy_server_socket, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    
    int proxy_connection_socket;
    int client_address_len = sizeof(client_address);
    char buffer_in[1024] = {0};
    char buffer_out[1024] = {0};
    memset(&buffer_in, '\0', 1024);
    memset(&buffer_out, '\0', 1024);

    while(1)
    {
        //establish connection between client and proxy_server
        if ((proxy_connection_socket = accept(proxy_server_socket, (struct sockaddr *)&client_address,
                           (socklen_t*)&client_address_len))<0)
        {
            perror("accept");
            exit(EXIT_FAILURE);
        }

        // read the data in buffer_in from the proxy server socket
        read( proxy_connection_socket , buffer_in, 1024);

        printf("Request :\tType-%c,\tMessage-%s\n",buffer_in[0], buffer_in+1 );

        // If message type is 1 or 2 then process the request else return the error message
        if(buffer_in[0]=='1'||buffer_in[0]=='2')
        {
            // If requested domain_name/ip_address alredy present in cache 
            // return the corresponding value
            // else ask the dns server to return the corresponding value
            // and store in the cache based on FIFO mechanism
            if(find(buffer_in))
            {
                get(buffer_in,buffer_out);
                memset(&buffer_in,'\0',1024);
                send(proxy_connection_socket , buffer_out , strlen(buffer_out) , 0 );
                memset(&buffer_out, '\0', 1024);
                close(proxy_connection_socket);
            	printf("CACHE FOUND\n");
            }
            else
            {
                int i;
                int type1_or_2 = buffer_in[0]=='1'?1:0;
                
                struct CACHE * new_entry;
                new_entry = (struct CACHE *)malloc(sizeof(struct CACHE));
                memset(&(new_entry->domain_name),'\0',1024);
                memset(&(new_entry->ip_address),'\0',1024);
                
                if(buffer_in[0]=='1'){
                    for(i=1;i<strlen(buffer_in);i++){
                        new_entry->domain_name[i-1]=buffer_in[i];
                    }
                }
                else{    
                    for(i=1;i<strlen(buffer_in);i++){
                        new_entry->ip_address[i-1]=buffer_in[i];
                    }
                }    
                
                if ((dns_server_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
                {
                    printf("\n Socket creation error \n");
                    return -1;
                }

                // Try connecting with dns server
                if (connect(dns_server_socket, (struct sockaddr *)&dns_serv_addr, sizeof(dns_serv_addr)) < 0)
                {
                    printf("\nConnection with DNS Failed \n");
                    return -1;
                }

                send(dns_server_socket , buffer_in , strlen(buffer_in) , 0 );
                memset(&buffer_in,'\0',1024);
                read( dns_server_socket , buffer_in, 1024);
                close(dns_server_socket);

                // update the cache using FIFO 
                if(buffer_in[0]=='3')
                {
                    if(type1_or_2){ 
                        for(i=1;i<strlen(buffer_in);i++){
                            new_entry->ip_address[i-1]=buffer_in[i];
                        }
                    }
                    else {
                        for(i=1;i<strlen(buffer_in);i++){
                            new_entry->domain_name[i-1]=buffer_in[i];
                        }
                    }
                    int size=0;
                    new_entry->next=cache_head;
                    cache_head=new_entry;

                    struct CACHE * current=cache_head;
                    while(current->next!=NULL)
                    {
                        current=current->next;size++;
                    }
                    if(size>=QUEUE_LENGTH)
                    {
                        
                        current=cache_head;
                        size = 1;
                        while(size!=QUEUE_LENGTH){
                            current = current->next;
                            size++;
                        }
                        free(current->next);
                        current->next = NULL;

                    }
                    printf("CACHE UPDATED\n");
                }
                else free(new_entry);

                send(proxy_connection_socket ,buffer_in , strlen(buffer_in) , 0 );
                close(proxy_connection_socket);
                printf("Response :\tType-%c,\tMessage-%s\n", buffer_in[0], buffer_in+1);
                memset(&buffer_in,'\0',1024);
            }
        }
        else
        {
            memset(&buffer_in,'\0',1024);
            strcpy(buffer_in,"4ErrorMessage");
            send(proxy_connection_socket ,buffer_in , strlen(buffer_in) , 0 );
            close(proxy_connection_socket);
            memset(&buffer_in,'\0',1024);
        }
        printf("%s\n", "______________________________________________________________________________");

    }
    return 0;
}
