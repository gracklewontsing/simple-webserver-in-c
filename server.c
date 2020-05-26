#include <sys/socket.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>

//important source: http://index-of.es/Programming/C/TCP%20IP%20Sockets%20in%20C,%20Second%20Edition%20Practical%20Guide%20for%20Programmers.pdf


int main( int argc, char const *argv[])
{
    //server vars
    int server_fd, new_socket;
    long valread;
    struct sockaddr_in address;
    int addrlen = sizeof(address);          

    //content vars
    const char *syllabus= "syllabus.html";    
    FILE *content = fopen(syllabus, "r+");
    size_t size = 0;
    char *content_buff = NULL;

    //content methods
    fseek(content,0,SEEK_END); //GO TO END
    size = ftell(content); //BYTECOUNT
    rewind(content);//GO TO BEGINNING
    content_buff = malloc((size+1) * sizeof(*content_buff)); //ALLOCATION
    fread(content_buff, size, 1, content);//READ INTO BUFFER
    content_buff[size] = '\0';//NULL-TERMINATE BUFFER    
        
    //server setup        
    const int PORT = 8080;
    memset((char *)&address,0,sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr= htonl(INADDR_ANY); //set 0.0.0.0 for localhost
    address.sin_port = htons(PORT);
    memset(address.sin_zero, '\0', sizeof address.sin_zero);    
    
    //content setting for the syllabus as plain 
    char *header_content = "HTTP/1.1 200 OK\n Content-Type: text/html\n Content-Length: 12\n\n";     
    char headers[200000];
    strcpy(headers, header_content);
    strcat(headers, content_buff);
    char *not_found = "HTTP/1.1 404 NOT FOUND\n Content-Type: text/html\n Connection: close\n\n <html> <h1> Damn &#128563; u just got 404'd &#128563; <br> better check the headers</h1></html>";

    //socket file descriptor create
    if((server_fd =  socket (AF_INET, SOCK_STREAM,0)) == 0) 
    {
        perror("Could not create socket");
        return 0;
    }
    if (bind(server_fd,(struct sockaddr *)&address, sizeof(address))<0)
    {
        perror("Bind failure");
        return 0;
    }        
    if(valread < 0)
    {
        printf("No bytes to read");
    }

    //serving
    while(1)
    {
        printf("-------------Waiting------------- \n\n");

        if(listen(server_fd, 1000)<0) //max connection requests
        {
            perror("SERVER: Listen");
            exit(EXIT_FAILURE);
        }

        if ((new_socket = accept(server_fd,(struct sockaddr *)&address, (socklen_t*)&addrlen))<0)
        {
            perror("SERVER: Accept");
            exit(EXIT_FAILURE);
        }

        if(new_socket > 0)
        {
            printf("SERVER: A client has connected!\n");
        }        
        
        char buffer[30000] = {0};
        int valread = read(new_socket, buffer, 30000);
        printf("%s\n", buffer);
        if(strstr(buffer, "GET / ") == NULL)
        {
            write(new_socket, not_found, strlen(not_found));      
        }
        else
        {
            write(new_socket, headers, strlen(headers));   
        }                     
        printf("------------Sending------------\n\n");        
        close(new_socket);
    }
    fclose(content);
    free(content_buff);
    close(server_fd);
    return 0;
}


