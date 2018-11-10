#include <stdio.h>
#include "networkstructures.h"
#include <stdlib.h>
#include <string.h>
#include "fileops.h"

void background_listen(int socket_id, struct sockaddr_in *address)
{
    int opt = 1;
    FILE *fp = fopen("test1.jpg", "rb");
    struct file_section *sections = split_file_into_sections(fp, findSizeOfFile(fp));
    int numberOfPieces = floor((findSizeOfFile(fp) / pieceSize) + 1);
    setsockopt(socket_id, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));
    bind(socket_id, (struct sockaddr *)address, sizeof(address));
    listen(socket_id, 10);
    while (1)
    {

        struct sockaddr_in *client_address;
        int len = sizeof(sockaddr_in);
        int client_socket = accept(socket_id, (struct sockaddr *)&client_address, (socklen_t *)&len);
        int f; //temp, used to see which part to recieve, first six or last six
        read(client_socket, &f, sizeof(f));
        if (f == 0)
        {
            for (int i = 0; i < numberOfPieces / 2; i++)
            {
                write(client_socket, &sections[i], sizeof(sections[i]));
            }
        }
        else
        {
            for (int i = numberOfPieces / 2 + 1; i < numberOfPieces; i++)
            {
                write(client_socket, &sections[i], sizeof(sections[i]));
            }
        }
    }
}

int connectToSocket(int socket, sockaddr_in address)
{
}


// template<typename T>
vector<sockaddr_in> readVector(int sock){
    int size;
    read(sock, &size, sizeof(int));
    vector<sockaddr_in> A(size);
    for(int i=0; i<size; i++){
        read(sock, &A[i], sizeof(sockaddr));
    }

    return A;
}
//start background thread

int main()
{

    // printf("%d\n", socket_id);
    struct sockaddr_in server_address;
    server_address.sin_port = htons(PORT);
    server_address.sin_family = AF_INET;
    if (inet_pton(AF_INET, "127.0.0.1", &server_address.sin_addr) < 0)
    {
        printf("PTON ERRRO\n");
    };
    int id;
    scanf("%d", &id);
    int socket_id;
    while (1)
    {
       
        if(socket_id>1){
            close(socket_id);
        }
        
        socket_id=socket(AF_INET, SOCK_STREAM, 0);
        int choice;
        scanf("%d", &choice);

        if (connect(socket_id, (struct sockaddr *)&server_address, sizeof(server_address)) < 0)
        {
            perror("connect error: ");
            exit(EXIT_FAILURE);
        };
        write(socket_id, &choice, sizeof(choice));
        switch (choice)
        {
        case 0:
        {
            // send handshake

            Handshake handshake;
            handshake.id = id;
            int written = write(socket_id, &handshake, sizeof(Handshake));
            break;
        }

        case 1:
        {

            int file_id;
            scanf("%d", &file_id);
            FileInfo info;
            info.user_id = id;
            info.file_id = file_id;
            write(socket_id, &info, sizeof(info));
            break;
            //send info of file
        }

        case 2:
        {

            vector<file_section> recieved;

            FileRequest request;
            request.user_id = id;
            request.file_id = 1;
            int w = write(socket_id, &request, sizeof(request));

            
            FileRequestResponse response;
            response.clients= readVector(socket_id);
            // read(socket_id, &response, sizeof(response));
            printf("%d\n", response.clients.size());
            // for (int i = 0; i < response.clients.size(); i++)
            // {
            //     // get stuff
            // }

            break;

            //synchronization?

            //also informs about the pieces it needs.
            //As a test, get first six from first, second six from second
            //in a separate thread also accept
            //request file
        }
        }
    }

    //1. Should tell server about files it has
    //2. Should serve the section it is asked to server
    //3. Should request the server for the file

    //1. Should tell about number of pieces, and the  pieces it has
    //need checks for writing
}