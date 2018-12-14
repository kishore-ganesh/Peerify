#include <stdio.h>
#include "networkstructures.h"
#include <stdlib.h>
#include <string.h>
#include "fileops.h"
#include <thread>
#include <mutex>
#include <functional>

std::mutex mtx;
//port default else override
int setUpNetwork(int port)
{
    int opt = 1;
    int socket_id = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_id < -1)
    {
        perror("SOCKET ERROR");
    }

    struct sockaddr_in address;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    setsockopt(socket_id, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));
    if (bind(socket_id, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("BIND ERROR");
    };

    printf("BINDED\n");
    if (listen(socket_id, 10) < 0)
    {
        perror("LISTEN ERROR");
    };

    return socket_id;
}

void listenLoop(int socket_id, int port)
{
    FILE *fp = fopen("test1.jpg", "rb");
    struct file_section *sections = split_file_into_sections(fp, findSizeOfFile(fp));
    int numberOfPieces = findNumberOfPieces(fp);
    printf("NUMBER OF PIECES IS: %d\n", numberOfPieces);

    while (1)
    {

        struct sockaddr_in *client_address;
        int len = sizeof(sockaddr_in);
        int client_socket = accept(socket_id, (struct sockaddr *)&client_address, (socklen_t *)&len);
        printf("CONNECTED\n");
        int f; //temp, used to see which part to recieve, first six or last six
        // read(client_socket, &f, sizeof(f));
        printf("BEGINNING WRITE\n");
        if (port == 2012)
        {

            int toWrite = numberOfPieces / 2;
            write(client_socket, &toWrite, sizeof(int));
            for (int i = 0; i < numberOfPieces / 2; i++)
            {

                write(client_socket, &sections[i], sizeof(sections[i]));
                write(client_socket, sections[i].databuf, sections[i].size_of_databuf);
            }
        }
        else
        {
            int toWrite = numberOfPieces - numberOfPieces / 2;
            write(client_socket, &toWrite, sizeof(int));
            for (int i = numberOfPieces / 2; i < numberOfPieces; i++)
            {
                write(client_socket, &sections[i], sizeof(sections[i]));
                write(client_socket, sections[i].databuf, sections[i].size_of_databuf);
            }
        }
    }
}

void background_listen(int port)
{

    int socket_id = setUpNetwork(port);
    listenLoop(socket_id, port);
}

int connectToSocket(int socket, sockaddr_in address)
{
}

// template<typename T>
vector<sockaddr_in> readVector(int sock)
{
    int size;
    read(sock, &size, sizeof(int));
    vector<sockaddr_in> A(size);
    for (int i = 0; i < size; i++)
    {
        read(sock, &A[i], sizeof(sockaddr));
    }

    return A;
}

void recieve_section(sockaddr_in address, vector<file_section> &sections)
{
    mtx.lock();
    int socket_id = socket(AF_INET, SOCK_STREAM, 0);
    if (connect(socket_id, (sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("RECIEVE ERROR");
    };

    //for now make it 6

    //get number of pieces from the clients first
    int numberOfPieces = 0;
    read(socket_id, &numberOfPieces, sizeof(int));
    for (int i = 0; i < numberOfPieces; i++)
    {
        file_section section;
        read(socket_id, &section, sizeof(section));
        section.databuf = (char *)malloc(section.size_of_databuf);
        read(socket_id, section.databuf, section.size_of_databuf);
        sections.push_back(section);
    }

    mtx.unlock();
}

int sendHandshake(int socket_id, int id, int port)
{
    Handshake handshake;
    handshake.id = id;
    handshake.port = port;
    int written = write(socket_id, &handshake, sizeof(Handshake));
    return written;
}

void sendFileInfo(int socket_id, int id, int file_id)
{
    FileInfo info;
    info.user_id = id;
    info.file_id = file_id;
    write(socket_id, &info, sizeof(info));
}
//start background thread

int main()
{

    // printf("%d\n", socket_id);

    int port;
    int id;
    scanf("%d", &port);
    scanf("%d", &id);
    std::thread background(background_listen, port); //what sort of call is this?

    struct sockaddr_in server_address;
    server_address.sin_port = htons(PORT);
    server_address.sin_family = AF_INET;
    if (inet_pton(AF_INET, "127.0.0.1", &server_address.sin_addr) < 0)
    {
        printf("PTON ERRRO\n");
    };

    int socket_id;
    while (1)
    {

        int choice;
        scanf("%d", &choice);
        if (socket_id > 1)
        {
            close(socket_id);
        }

        socket_id = socket(AF_INET, SOCK_STREAM, 0);

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
            sendHandshake(socket_id, id, port);

            break;
        }

        case 1:
        {

            int file_id;
            scanf("%d", &file_id);
            sendFileInfo(socket_id, id, file_id);
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
            response.clients = readVector(socket_id);
            int ports[] = {2012, 2014};
            vector<thread> recieve_threads;
            printf("%d", htons(response.clients[0].sin_port));
            // printf("%d\n", response.clients.size());
            for (int i = 0; i < response.clients.size(); i++)
            {
                recieve_threads.push_back(std::thread(recieve_section, response.clients[i], std::ref(recieved)));
                // recieve_threads.push_back(recieve);
            }

            for (int i = 0; i < recieve_threads.size(); i++)
            {
                recieve_threads[i].join();
            }

            reconstruct_from_sections(recieved, recieved.size());

            // read(socket_id, &response, sizeof(response));
            // printf("%d\n", response.clients.size());
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