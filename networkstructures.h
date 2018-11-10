
#include <vector>
#include <map>

#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include<arpa/inet.h>
#define PORT 2000


using namespace std;
struct User
{
    int id;
    vector<int> files;
    sockaddr_in address;
};

struct Handshake
{
    int id;

};

struct FileInfo{
    int user_id;
    int file_id;
};

struct FileRequest{
    int user_id;
    int file_id;

};

struct FileRequestResponse{
    vector<sockaddr_in> clients;
};

