#include <stdio.h>
#include <map>
#include <string.h>
#include "networkstructures.h"
#include "networkops.h"

bool findInVector(int id, vector<int> A)
{
    for (int i = 0; i < A.size(); i++)
    {
        if (A[i]==id)
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


map<char*, User> users;

void readID(int client_socket, char** ID)
{
    *ID = (char* )malloc(ID_SIZE);
    read(client_socket, *ID, ID_SIZE);
}

// vector<User *>
//     users;

//Should client generate ID or should server generate the ID

int main()
{
   int socket_fd = setUpNetwork(PORT);

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
            printf("HANDSHAKE INITIATED\n");
            Handshake handshake;
            int r = read(client_socket, &handshake, sizeof(handshake));
            // printf("%d ", r);
            User user;
            user.address = client_address;
            user.address.sin_port=htons(handshake.port);

            readID(client_socket, &user.id);
            
            users.insert(make_pair(user.id, user));
            
            //handshake
            break;
        }

        // vector<User *>
        //     users;
        case 1:
        {
            printf("REGISTERING FILE INFO\n");
            FileInfo fileInfo;
            read(client_socket, &fileInfo, sizeof(fileInfo));
            readID(client_socket, &fileInfo.user_id);
            users[fileInfo.user_id].files.push_back(fileInfo.file_id);

            //info about file

            break;
        }

        case 2:
        {
            //requests file

            printf("FILE REQUESTED\n");
            FileRequest fileRequest;
            read(client_socket, &fileRequest, sizeof(FileRequest));
            readID(client_socket, &fileRequest.user_id);
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