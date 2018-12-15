
#include <vector>
#include <map>

#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include<arpa/inet.h>
#define PORT 2000
#define ID_SIZE 128


using namespace std;
struct User
{
    char* id;
    vector<int> files;
    sockaddr_in address;
};

struct Handshake
{
    char* ID;
    int port;

};

struct FileInfo{
    char* user_id;
    int file_id;
};

struct FileRequest{
    char* user_id;
    int file_id;

};

struct FileRequestResponse{
    vector<sockaddr_in> clients;
};

