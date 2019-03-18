#include <stdio.h>
#include "networkstructures.h"
#include <stdlib.h>
#include <string.h>
#include "fileops.h"
#include <thread>
#include <mutex>
#include <functional>
#include "networkops.h"
#include "util.h"

std::mutex mtx;
//port default else override

void writeSection(int client_socket, file_section section){
    write(client_socket, &section.header, sizeof(file_header));
    write(client_socket, &section.size_of_databuf, sizeof(section.size_of_databuf)); 
    write(client_socket, section.databuf, section.size_of_databuf);
}

file_section* readSection(int socket){
    file_section* t = (file_section*)malloc(sizeof(file_section));
    read(socket, &t->header, sizeof(t->header));
    read(socket, &t->size_of_databuf, sizeof(t->size_of_databuf));
    t->databuf = (char*) malloc(t->size_of_databuf);
    read(socket, t->databuf, t->size_of_databuf);
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

            int32_t toWrite = numberOfPieces / 2;
            write(client_socket, &toWrite, sizeof(toWrite));
            for (int i = 0; i < numberOfPieces / 2; i++){
                writeSection(client_socket, sections[i]);
            }
        }
        else
        {
            int32_t toWrite = numberOfPieces - numberOfPieces / 2;
            write(client_socket, &toWrite, sizeof(toWrite));
            for (int i = numberOfPieces / 2; i < numberOfPieces; i++)
            {

                writeSection(client_socket,sections[i]);
            }
        }
    }
}

void background_listen(int port)
{

    int socket_id = setUpNetwork(port);
    listenLoop(socket_id, port);
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
    int32_t numberOfPieces = 0;
    read(socket_id, &numberOfPieces, sizeof(numberOfPieces));
    for (int i = 0; i < numberOfPieces; i++)
    {
       //Change
        file_section section;
        section.databuf = (char *)malloc(section.size_of_databuf);
        read(socket_id, section.databuf, section.size_of_databuf);
        sections.push_back(section);
        section = *(readSection(socket_id));
    }

    mtx.unlock();
}


int sendID(int socket_id, char* ID)
{
    int written = write(socket_id, ID, strlen(ID));
    return written;
}



int sendHandshake(int socket_id, char* ID, int32_t port)
{
    int written = write(socket_id, &port, sizeof(port));
    written+=sendID(socket_id, ID);
    return written;
}

void sendFileInfo(int socket_id, char* ID, int32_t file_id)
{
    write(socket_id, &file_id, sizeof(file_id));
    sendID(socket_id, ID);
}

void requestForFile(int socket_id, char* id)
{
    vector<file_section> recieved;
    int32_t file_id = 1;
    int w = write(socket_id, &file_id, sizeof(file_id));

    sendID(socket_id, id);

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
}

void choiceLoop(struct sockaddr_in server_address, char* id, int32_t port)
{
    int socket_id = -1;
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
            requestForFile(socket_id, id);
            break;
            //synchronization?

            //also informs about the pieces it needs.
            //As a test, get first six from first, second six from second
            //in a separate thread also accept
            //request file
        }
        }
    }
}
//start background thread
//generate IDs automatically
//Ask for IP from user
int main(int argc, char *argv[])
{

    // printf("%d\n", socket_id);

    int32_t port;

    char  ID[128];

    generateUniqueID(ID, 128);
    
    struct sockaddr_in server_address;

    
    if (argc == 3)
    {
        port = atoi(argv[1]);
    }

    else
    {
        scanf("%d", &port);
       
    }

    printf("%d\n", port);

    std::thread background(background_listen, port); //what sort of call is this?

    server_address.sin_port = htons(PORT);
    server_address.sin_family = AF_INET;
    if (inet_pton(AF_INET, "127.0.0.1", &server_address.sin_addr) < 0)
    {
        printf("PTON ERRRO\n");
    };
    choiceLoop(server_address, ID, port);

    //1. Should tell server about files it has
    //2. Should serve the section it is asked to server
    //3. Should request the server for the file

    //1. Should tell about number of pieces, and the  pieces it has

    //Add GUI later
    //need checks for writing
}
