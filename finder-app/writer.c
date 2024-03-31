#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<string.h>
#include<syslog.h>

int main(int arg, char * args[])
{
    openlog(NULL,0,LOG_USER);
    syslog(LOG_DEBUG,"Starting the program");
    if (arg != 3)
    {
        printf("parameters are not specified");
        syslog(LOG_ERR,"parameters are not specified");
        return 1;
    }

    char *file_path = args[1];
    char *content = args[2];
    size_t content_length = strlen(content);

    FILE* fptr;
 
    // opening the file in read mode
    fptr = fopen(file_path, "w");
 
    // checking if the file is opened successfully
    if (fptr == NULL) 
    {
        perror("file can not be opened");
        syslog(LOG_ERR,"file can not be opened");
        return 1;
    }

    size_t bytes_written = fwrite(content, sizeof(char), content_length, fptr);
    if (bytes_written != content_length) {
        perror("Error writing to file");
        syslog(LOG_ERR,"Error writing to file");
        return 1;
    }
    else{

        syslog(LOG_DEBUG,"Writing %s to %s",content,file_path);
        //printf("Writing %s to %s\n",content,file_path);
    }

    // Close the file
    fclose(fptr);

    printf("Content successfully written to %s\n", file_path);

    return 0;

}