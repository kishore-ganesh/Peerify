#include "networkops.h"
#include "networkstructures.h"
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