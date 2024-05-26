#define __USE_XOPEN2K
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <syslog.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>
#include <stdbool.h>
#include<fcntl.h>



#define PORT "9000"
#define BUFFER_SIZE 1024
#define FILE_PATH "/var/tmp/aesdsocketdata"

int server_fd, client_fd;
FILE *file_ptr;

void handle_sigint(int sig) {
    syslog(LOG_INFO, "Caught signal, exiting");
    if (client_fd != -1) {
        close(client_fd);
    }
    if (server_fd != -1) {
        close(server_fd);
    }
    if (file_ptr != NULL) {
        fclose(file_ptr);
    }
    if (remove(FILE_PATH)==0)
    {
        printf("file deleted");
    }
    closelog();
    exit(0);
}

int main(int argc, char* argv[]) {

    if (remove(FILE_PATH)==0)
    {
        printf("file deleted");
    }

    int daemon = 0;
    if((argc > 1) && (strcmp(argv[1], "-d") == 0))
    {
     	daemon = 1; 
     	syslog(LOG_INFO,"Success: Running in daemon mode...\n");     	
    }

    /*SYSLOG */
    signal(SIGINT, handle_sigint);
    signal(SIGTERM, handle_sigint);

    openlog("aesdsocket", LOG_PID, LOG_USER);

    /*SOCKET COMMUNICATION*/
    struct sockaddr_in client_addr;
    socklen_t client_addr_len;

    char buffer[BUFFER_SIZE];
    ssize_t bytes_received;
    char client_ip[INET_ADDRSTRLEN];

    int opt = 1;

    // first, load up address structs with getaddrinfo():
    struct addrinfo hints;
    struct addrinfo *res;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;  // use IPv4 or IPv6, whichever
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;     // fill in my IP for me

    getaddrinfo(NULL, PORT, &hints, &res);

    // make a socket:

    server_fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if ((server_fd  == -1)) {
        syslog(LOG_ERR, "Error creating socket");
        return -1;
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
        syslog(LOG_ERR, "Error setting socket options: %s", strerror(errno));
        close(server_fd);
        return -1;
    }

    // bind it to the port we passed in to getaddrinfo():

    bind(server_fd, res->ai_addr, res->ai_addrlen);

    if (daemon)
    {
        pid_t pid = fork();

    	if (pid == -1)
    	{
        	syslog(LOG_ERR, "Error forking: %m");
        	close(server_fd);
    		closelog();
        	exit(0);
    	} 	
    	else if (pid != 0)
    	{
    	    close(server_fd);
    		closelog();
        	exit(0); // Parent exits
    	}
    }

    if (listen(server_fd, 10) == -1) {
        syslog(LOG_ERR, "Error listening on socket");
        close(server_fd);
        return -1;
    }

    while (1) {
        client_addr_len=sizeof(client_addr);
        client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_addr_len);

        if (client_fd == -1) {
            syslog(LOG_ERR, "Error accepting connection");
            continue;
        }

        inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);
        syslog(LOG_INFO, "Accepted connection from %s", client_ip);

        file_ptr = fopen(FILE_PATH, "a+");
        if (file_ptr == NULL) {
            syslog(LOG_ERR, "Error opening file");
            close(client_fd);
            continue;
        }

        while ((bytes_received = recv(client_fd, buffer, BUFFER_SIZE, 0)) > 0) {

            //buffer[bytes_received] = '\0';
            size_t bytes_written=fwrite(buffer, sizeof(char), bytes_received, file_ptr);
            if (bytes_written != bytes_received) 
            {
                syslog(LOG_ERR,"Error writing to file");
                return -1;
            }

            if (strchr(buffer, '\n')) {
                fseek(file_ptr, 0, SEEK_SET);
                while (fgets(buffer, BUFFER_SIZE, file_ptr) != NULL) {
                    send(client_fd, buffer, strlen(buffer), 0);
                }
                fseek(file_ptr, 0, SEEK_END);
            }
        }

        syslog(LOG_INFO, "Closed connection from %s", client_ip);
        fclose(file_ptr);
        close(client_fd);
        client_fd = -1;
    }
    remove(FILE_PATH);
    close(server_fd);
    closelog();
    return 0;
}
