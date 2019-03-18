
#include <vector>
#include <map>

#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include<arpa/inet.h>
#include<cstdint>
#define PORT 2000
#define ID_SIZE 128


using namespace std;
struct User
{
    char* id;
    vector<int32_t> files;
    sockaddr_in address;
};

struct Handshake
{
    char* ID;
    int32_t port;

};

struct FileInfo{
    char* user_id;
    int32_t file_id;
};

struct FileRequest{
    char* user_id;
    int32_t file_id;

};

struct FileRequestResponse{
    vector<sockaddr_in> clients;
};

