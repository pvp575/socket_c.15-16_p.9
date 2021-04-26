#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <stddef.h>
#include <unistd.h>
#include <stdio.h>

int main()
{
    int sock, listener;
    struct sockaddr_in addr;
    char buf[1024];
    int bytes_read;
    int child_pid;

    listener = socket(AF_INET, SOCK_STREAM, 0);
    if(listener < 0)
    {
        perror(NULL);
        exit(1);
    }
    
    addr.sin_family = AF_INET;
    addr.sin_port = htons(5000);
    addr.sin_addr.s_addr = INADDR_ANY;
    if(bind(listener, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        perror(NULL);
        exit(2);
    }

    listen(listener, 1);
    
    while(1)
    {
        sock = accept(listener, NULL, NULL);
        if(sock < 0)
        {
            perror(NULL);
            exit(3);
        }
        
        switch(child_pid = fork())
        {
        case -1:
            perror(NULL);
            break;
            
        case 0:
            close(listener);
            while(1)
            {
                bytes_read = recv(sock, buf, 1024, 0);
                if(bytes_read <= 0) break;
                send(sock, buf, bytes_read, 0);
            }
            close(sock);
            exit(0);
            
        default:
            printf("New connection is being processed by the child (PID: %d)\n", child_pid);
            close(sock);
        }
    }
    close(listener);
    return 0;
}