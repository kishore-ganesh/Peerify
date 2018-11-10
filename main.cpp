#include <stdio.h>
#include<stdlib.h>

#include<vector>
#include<map>
#include<algorithm>
#include "fileops.h"



using namespace std;

size_t findSizeOfFile(FILE *fp)
{
    fseek(fp, 0, SEEK_END);
    size_t sz= ftell(fp);
    fseek(fp, 0, SEEK_SET);
    return sz;
}

size_t copy_into_buffer(FILE *fp, char **bufp, int index)
{
    *bufp = (char *)malloc(pieceSize);
    int position = index * pieceSize;
    // fseek(fp, SEEK_SET, index*pieceSize+1);
    size_t readsz=fread(*bufp, 1, pieceSize, fp);
    
    return readsz;
}

struct file_section *split_file_into_sections(FILE *fp, size_t size)
{
    // make this better  by reading a  buffered amount
    // size_t result=fread(buffer, size, 1, fp);
    struct file_section *file_sections;
    // if(result!=size){
    int numberOfPieces = floor((size / pieceSize) + 1);
    file_sections = (struct file_section *)malloc(numberOfPieces * sizeof(struct file_section));
    for (int i = 0; i < numberOfPieces; i++)
    {
        // file_sections[i].header = (struct file_header *)malloc(sizeof(struct file_header));
        file_sections[i].header.position_in_file = i;
        file_sections[i].size_of_databuf=copy_into_buffer(fp, &file_sections[i].databuf, i);
    }

    // }

    // else{
    //
    // }

    return file_sections;
}

bool compare_sections(struct file_section a, struct file_section b){
    return a.header.position_in_file<b.header.position_in_file;
}

void reconstruct_from_sections(struct file_section* sections, size_t size, int numberOfPieces){
    FILE* fp=fopen("test.jpg", "wb");
    sort(sections, sections+numberOfPieces, compare_sections);
    for(int i=0; i<numberOfPieces; i++){
        fwrite(sections[i].databuf, 1,sections[i].size_of_databuf, fp);
    }


}

// int main(){
//     FILE* fp=fopen("test1.jpg", "rb");
//     struct file_section* sections=split_file_into_sections(fp, findSizeOfFile(fp));
//     reconstruct_from_sections(sections, findSizeOfFile(fp), floor((findSizeOfFile(fp)/pieceSize)+1));

//     //is it being advanced?
// }