#include "systemcalls.h"
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include "stdbool.h"
#include "fcntl.h"

/**
 * @param cmd the command to execute with system()
 * @return true if the command in @param cmd was executed
 *   successfully using the system() call, false if an error occurred,
 *   either in invocation of the system() call, or if a non-zero return
 *   value was returned by the command issued in @param cmd.
*/
bool do_system(const char *cmd)
{

/*
 * TODO  add your code here
 *  Call the system() function with the command set in the cmd
 *   and return a boolean true if the system() call completed with success
 *   or false() if it returned a failure
*/
    if (cmd != NULL)
    {
        int status = system(cmd);

        if (status == -1)
        {
            return false;
        }
        else {
            if (WIFEXITED(status)) 
            {
                int exit_status = WEXITSTATUS(status);
                if (exit_status == 0) 
                {
                    printf("Command executed successfully.\n");
                    return true;
                } else 
                {
                    printf("Command exited with error code: %d\n", exit_status);
                    return false;
                }
            } 
            else 
            {
                printf("Command terminated abnormally.\n");
                return false;
            }}
    }

    return false;

}

/**
* @param count -The numbers of variables passed to the function. The variables are command to execute.
*   followed by arguments to pass to the command
*   Since exec() does not perform path expansion, the command to execute needs
*   to be an absolute path.
* @param ... - A list of 1 or more arguments after the @param count argument.
*   The first is always the full path to the command to execute with execv()
*   The remaining arguments are a list of arguments to pass to the command in execv()
* @return true if the command @param ... with arguments @param arguments were executed successfully
*   using the execv() call, false if an error occurred, either in invocation of the
*   fork, waitpid, or execv() command, or if a non-zero return value was returned
*   by the command issued in @param arguments with the specified arguments.
*/

bool do_exec(int count, ...)
{
    va_list args;
    va_start(args, count);
    char * command[count+1];
    int i;
    for(i=0; i<count; i++)
    {
        command[i] = va_arg(args, char *);
    }
    command[count] = NULL;

    // checking if command is absolute path or not
    if (command[0][0]!='/')
    {
        return false;
    }
/*
 * TODO:
 *   Execute a system command by calling fork, execv(),
 *   and wait instead of system (see LSP page 161).
 *   Use the command[0] as the full path to the command to execute
 *   (first argument to execv), and use the remaining arguments
 *   as second argument to the execv() command.
 *
*/

    pid_t pid =fork();

    if (pid < 0){
        printf("child process cant be created");
        return false;
    }
    if(pid==0)//child process
    {
        execv(command[0],command);
        // process should not return from here
        return false;
    }

    int status;
    waitpid(pid,&status,0);
    va_end(args);

    if(WIFEXITED(status))
    {
        if(WEXITSTATUS(status))
        {return false;}
        else return true;
    }

    return false;

}

/**
* @param outputfile - The full path to the file to write with command output.
*   This file will be closed at completion of the function call.
* All other parameters, see do_exec above
*/
bool do_exec_redirect(const char *outputfile, int count, ...)
{
    va_list args;
    va_start(args, count);
    char * command[count+1];
    int i;
    for(i=0; i<count; i++)
    {
        command[i] = va_arg(args, char *);
    }
    command[count] = NULL;
/*
 * TODO
 *   Call execv, but first using https://stackoverflow.com/a/13784315/1446624 as a refernce,
 *   redirect standard out to a file specified by outputfile.
 *   The rest of the behaviour is same as do_exec()
 *
*/
    pid_t pid =fork();

    if (pid < 0){
        printf("child process cant be created");
        return false;
    }
    if(pid==0)//child process
    {
        // Open the output file
        int fd = open(outputfile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (fd < 0) {
            fprintf(stderr, "Failed to open output file\n");
            return false;
        }

        // Redirect stdout to the output file
        // dup2 function copies the file descriptor and closed the second descriptoe
        if (dup2(fd, STDOUT_FILENO) < 0) {
            fprintf(stderr, "Failed to redirect stdout\n");
            return false;
        }

        // Close the file descriptor
        close(fd);

        execv(command[0],command);
        // process should not return from here
        return false;
    }

    int status;
    waitpid(pid,&status,0);

    if(WIFEXITED(status))
    {
        printf("child process excited with status %d\n",status);
        return false;
    }

    va_end(args);

    return true;
}