#include<stdio.h>
#include<stdlib.h>
#include<string.h>

typedef struct Etudiant{
char matricule[13];
char nom[50];
char prenom[50];
float moyenne;
}Etudiant;


typedef struct file_header{
    size_t file_size;
    char file_name[50];
    char file_extension[20];
    int nb_element;
}file_header;


typedef struct block_header{
    size_t  block_size;
    char data_type[50];
}block_header;


void read_element(Etudiant etudiant){
printf("nom : %s\n",etudiant.nom);
printf("preom : %s\n",etudiant.prenom);
printf("Matricule : %s\n",etudiant.matricule);
printf("moyenne : %f\n",etudiant.moyenne);
}

void write_block(FILE* file,void* T,int facteur_blockage,size_t size_of_element,int n){
    block_header block;
    strcpy(block.data_type,"Etudiant");
    for(int i = 0;i<n;i=i+facteur_blockage){
        block.block_size = size_of_element *facteur_blockage;
        size_t block_memory_size = sizeof(block_header)+block.block_size;
        void* buffer = malloc(block_memory_size);
        memcpy(buffer,&block,sizeof(block_header));
        memcpy(buffer+sizeof(block_header),T+i,size_of_element*facteur_blockage);
        fwrite(buffer,block_memory_size,1,file);

    }
}




int main(){

    return 0;
}