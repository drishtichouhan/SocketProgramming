### ApplicationId- 4 DNS Resolving System
IITG CS349 Computer Networks Lab Assignment 3 on Socket Programming

### Group-32
Members:- 
1. Aryan  Agrawal
2. Aviral Gupta
3. Devansh Gupta

### Assignment Requirements
1. Request can be of two types.(Type1 & Type 2)
	1. Type 1 Message field contains Domain Name and requests for corresponding IP address.
	2. Type 2 Message field contains IP address and request for the corresponding Domain Name.

2. Response can be of two types again (Type3 & Type4)
	1. Type 3 Message field contains Domain Name/IP address.
	2. Type 4: Message field contains error message “entry not found in the database”.

3. Proxy server caches DNS Server response.

4. Proxy server's cache size is 3.

5. FIFO scheme is used for updating the cache by the proxy server .

### UserFlow
1. On line 104 in p.c enter IP of network on which you are going to host server or s.c in place of "192.168.43.2"
		if(inet_pton(AF_INET, "192.168.43.2", &dns_serv_addr.sin_addr)<=0)
 We had hardcoded server IP to keep the format same as given in assignment to run proxy server.

2. Open three terminal (Assuming using same system to run proxy, client and server)

3. Compile all the 3 files in 3 different terminal by command "gcc -o client c.c", "gcc -o proxy p.c", "gcc -o server s.c"

4. IP/Domain name present in file "database.csv" can be resolved and you can add or remove IP from the databse.

5. run "./server" on 1st terminal.File "database.csv" should be present with ./server if running server on different system. 

6. run "./proxy 8000" on 2nd terminal where we are taking 8000 as proxy server port. "Wrong Packet Format" message will print on server (3rd) terminal shows connection working fine between proxy and server.

7. run "./client <IpOfProxySever> 8000" on 3rd terminal in place of <IpOfProxyServer> enter the IP where you hosted proxy server, if using same system then enter the system IP.

8. Enter your request in client (3rd) terminal  (no space between type and domain name or IP address)
	Request Type 1 example "1google.com" where 1 is type of request and "google.com" domain name.
   	Request Type 2 example "21.1.1.1" where 2 is type of request and 1.1.1.1 IP address.

9. Reponse to client
	Response Type 3 example "31.1.1.1" where 3 is type of response and 1.1.1.1 is response message.
   	Response Type 4 example "4Entry not found in the database" where 4 is type of response and "Entry not found in the database" is reponse message.

10. In proxy (2nd) terminal you can see that if domain name or IP not found in cache and found in dns server it prints "CACHE UPDATED" and if found it prints "CACHE FOUND".

11. If you request for IP or Domain already present in proxy cache then request will not be forwarded to dns server i.e no message will be printed on server (1st) terminal.