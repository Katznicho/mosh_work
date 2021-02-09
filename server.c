#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>

void bzero(void *a, size_t n)
{
    memset(a, 0, n);
}

void bcopy(const void *src, void *dest, size_t n)
{
    memmove(dest, src, n);
}

struct sockaddr_in *init_sockaddr_in(uint16_t port_number)
{
    struct sockaddr_in *socket_address = malloc(sizeof(struct sockaddr_in));
    memset(socket_address, 0, sizeof(*socket_address));
    socket_address->sin_family = AF_INET;
    socket_address->sin_addr.s_addr = htonl(INADDR_ANY);
    socket_address->sin_port = htons(port_number);
    return socket_address;
}
FILE *serverFile;// = fopen("serverfile.txt","a+");
char *process_operation(char *input)
{
    size_t n = strlen(input) * sizeof(char);
    char *output = malloc(n);
    memcpy(output, input, n);
    return output;
}

int main(int argc, char *argv[])
{
    serverFile = fopen("serverfile.txt","w");
    if(serverFile==NULL){

    printf("Sever failure");
    }
    const uint16_t port_number = 8081;
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in *server_sockaddr = init_sockaddr_in(port_number);
    struct sockaddr_in *client_sockaddr = malloc(sizeof(struct sockaddr_in));
    socklen_t server_socklen = sizeof(*server_sockaddr);
    socklen_t client_socklen = sizeof(*client_sockaddr);

    int firstBuffer = 1;
    char hospitalName[100];

    if (bind(server_fd, (const struct sockaddr *)server_sockaddr, server_socklen) < 0)
    {
        printf("Error! Bind has failed\n");
        exit(0);
    }
    if (listen(server_fd, 3) < 0)
    {
        printf("Error! Can't listen\n");
        exit(0);
    }

    //variable buffer to store strings receiver over the network
    const size_t buffer_len = 256;
    //memory allocation.
    char *buffer = malloc(buffer_len * sizeof(char));
    //char *line= malloc(buffer_len * sizeof(char));
    char *response = NULL;
    time_t last_operation;
    __pid_t pid = -1;

    //Run forever
    while (1)
    {
        int client_fd = accept(server_fd, (struct sockaddr *)&client_sockaddr, &client_socklen);

        pid = fork();

        if (pid == 0)
        {
            close(server_fd);

            if (client_fd == -1)
            {
                exit(0);
            }

            printf("Connection with `%d` has been established and delegated to the process %d.\nWaiting for a query...\n", client_fd, getpid());

            last_operation = clock();

            //printf("Client from %s\n",hospitalName);
            //strcpy(hospitalName,buffer);
            // char fileName[100];
            // printf("Hospitap:  %s\n",hospitalName);
            // if (firstBuffer==1)
            // {
            //     read(client_fd, hospitalName, buffer_len);
            //     firstBuffer = 0;

            //     printf("Client from %s\n",hospitalName);
            // }
         
            while (1)
            {
                recvfrom(client_fd,buffer,buffer_len,0,NULL,NULL);
                //read(client_fd, buffer, buffer_len);

                if (!strcmp(buffer, "close"))
                {
                    printf("Process %d: ", getpid());
                    close(client_fd);
                    printf("Closing session with `%d`. Bye!\n", client_fd);
                    break;
                }

                if (strlen(buffer) == 0)
                {
                    clock_t d = clock() - last_operation;
                    double dif = 1.0 * d / CLOCKS_PER_SEC;

                    if (dif > 1.0)
                    {
                        printf("Process %d: ", getpid());
                        close(client_fd);
                        printf("Connection timed out after %.3lf seconds. ", dif);
                        printf("Closing session with `%d`. Bye!\n", client_fd);
                        printf("Closing session with %s. Bye!\n", hospitalName);

                        break;
                    }

                    continue;
                }

                //printf("Process %d: ", getpid());

                //fprintf(serverFile,"%s", buffer);
                send(client_fd,buffer,buffer_len,0);
                fputs(buffer, serverFile);
    //close(client_fd);
                //strcpy(line,buffer);
                //printf("The buffer so far: %s\n",buffer);
                //fputs(buffer,filePtr);
                free(response);
                response = process_operation(buffer);
                bzero(buffer, buffer_len * sizeof(char));
  // read(client_fd, buffer, buffer_len);
                send(client_fd, response, strlen(response), 0);

                //printf("Responded with `%s`. Waiting for a new query...\n", response);

                last_operation = clock();
            }
            exit(0);
        }
        else
        {
            close(client_fd);
        }
    }
}
