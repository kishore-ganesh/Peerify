#define pieceSize 128000
#include <cstddef>
#include <stdio.h>
#include <math.h>
#include<vector>
using namespace std;
struct file_header
{

    int position_in_file;
};

struct file_section
{
    struct file_header header;
    char *databuf;
    size_t size_of_databuf;
};

size_t findSizeOfFile(FILE *fp);
size_t copy_into_buffer(FILE *fp, char **bufp, int index);
struct file_section *split_file_into_sections(FILE *fp, size_t size);
bool compare_sections(struct file_section a, struct file_section b);
void reconstruct_from_sections(vector<file_section> sections, int numberOfPieces);
int findNumberOfPieces(FILE* fp);