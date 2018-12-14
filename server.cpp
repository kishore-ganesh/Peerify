#include <stdio.h>
#include <map>
#include <string.h>
#include "networkstructures.h"

bool findInVector(int id, vector<int> A)
{
    for (int i = 0; i < A.size(); i++)
    {
        if (A[i] == id)
        {
            return true;
        }
    }

    return false;
}





template<typename T>
void sendVector(vector<T> A, int sock){
    //make htis more generic later
    int size=A.size();
    write(sock, &size, sizeof(int));
    for(int i=0; i<A.size(); i++){
        write(sock, &A[i], sizeof(T));
    }
}


map<int, User> users;

// vector<User *>
//     users;

int main()
{
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd <= 0)
    {
        printf("SOCKET ERROR\n");
    }
    int opt = 1;
    if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    };
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_port = htons(PORT);
    address.sin_addr.s_addr = INADDR_ANY;

    if (bind(socket_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        printf("BIND ERROR\n");
    };

    if (listen(socket_fd, 10) < 0)
    {
        printf("LISTEN ERROR\n");
    }

    while (1)
    {

        struct sockaddr_in client_address;
        int addrlen = sizeof(client_address);
        int client_socket = accept(socket_fd, (sockaddr *)&client_address, (socklen_t *)&(addrlen));
        if (client_socket < 0)
        {
            printf("ACCEPT FAILURE\n");
        }
        int mid;
        
        int r = read(client_socket, &mid, sizeof(int));
        printf("%d\n", mid);
        switch (mid)
        {
        case 0:
        {
            printf("ZERO\n");
            Handshake handshake;
            int r = read(client_socket, &handshake, sizeof(handshake));
            // printf("%d ", r);
            User user;
            user.address = client_address;
            user.address.sin_port=htons(handshake.port);
            user.id = handshake.id;
            users.insert(make_pair(user.id, user));
            
            //handshake
            break;
        }

        // vector<User *>
        //     users;
        case 1:
        {
            printf("ONE\n");
            FileInfo fileInfo;
            read(client_socket, &fileInfo, sizeof(fileInfo));
            users[fileInfo.user_id].files.push_back(fileInfo.file_id);

            //info about file

            break;
        }

        case 2:
        {
            //requests file

            printf("TWO\n");
            FileRequest fileRequest;
            read(client_socket, &fileRequest, sizeof(FileRequest));
            printf("%d\n", fileRequest.user_id);
            //use boost serialization
            FileRequestResponse response;
            for (auto i = users.begin(); i != users.end(); i++)
            {
                bool found = findInVector(fileRequest.file_id, i->second.files);
                if (found)
                {
                    response.clients.push_back(i->second.address);
                }
            }

            // write(client_socket, &response, sizeof(response));
            sendVector(response.clients, client_socket);
            break;
        }

    //disconnect
        
        }

        close(client_socket);

        //should we close socket?
    }
}