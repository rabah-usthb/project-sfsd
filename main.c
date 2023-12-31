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
    int real_nb_block_element;
    int facteur_blockage;
    char data_type[50];
}block_header;


void read_element(Etudiant etudiant){
printf("nom : %s\n",etudiant.nom);
printf("preom : %s\n",etudiant.prenom);
printf("Matricule : %s\n",etudiant.matricule);
printf("moyenne : %f\n",etudiant.moyenne);
}
void insertion_block(char* file_path,void* T,int facteur_blockage,size_t size_of_element,int n){
    FILE* file = fopen(file_path,"ab");
    block_header block;
    file_header header;
    strcpy(block.data_type,"Etudiant");
    block.facteur_blockage = facteur_blockage;
    for(int i = 0;i<n;i=i+facteur_blockage){
        int facteur;
         if(i+facteur_blockage>n){
            facteur = facteur_blockage;
            while(i+facteur > n){
                --facteur;
            }
            block.real_nb_block_element =facteur;
            }
        else{
            facteur = facteur_blockage;
            block.real_nb_block_element = facteur_blockage;}
        block.block_size = size_of_element *facteur_blockage;
        size_t block_memory_size = sizeof(block_header)+block.block_size;
        void* buffer = malloc(block_memory_size);
        memcpy(buffer,&block,sizeof(block_header));
        memcpy(buffer+sizeof(block_header),T+i,size_of_element*facteur);
        fwrite(buffer,block_memory_size,1,file);

    }
    fclose(file);

    FILE* file_2 = fopen(file_path,"r+b");
    fread(&header,sizeof(file_header),1,file_2);
    header.nb_element +=n;
    fseek(file_2,0,SEEK_END);
    header.file_size = ftell(file_2);
    fseek(file_2,0,SEEK_SET);
    fwrite(&header,sizeof(file_header),1,file_2);

}


int main(){

    return 0;
}