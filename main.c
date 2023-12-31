#include<stdio.h>
#include<stdlib.h>
#include<string.h>

// Définir une structure pour représenter les données d'un étudiant
typedef struct Etudiant{
char matricule[13];
char nom[50];
char prenom[50];
float moyenne;
}Etudiant;

// Définir une structure pour l'en-tête du fichier

typedef struct file_header{
    size_t file_size;
    char file_name[50];
    char file_extension[20];
    int nb_element;
}file_header;


// Définir une structure pour l'en-tête de bloc
typedef struct block_header{
    size_t  block_size;
    char data_type[50];
}block_header;

// Fonction pour lire et afficher les données d'un étudiant
void read_element(Etudiant etudiant){
printf("nom : %s\n",etudiant.nom);
printf("preom : %s\n",etudiant.prenom);
printf("Matricule : %s\n",etudiant.matricule);
printf("moyenne : %f\n",etudiant.moyenne);
}
// Fonction pour écrire des blocs de données dans un fichier
void write_block(FILE* file,void* T,int facteur_blockage,size_t size_of_element,int n){
       // Créer un en-tête de bloc et définir son type de données à "Etudiant"
    block_header block;
    strcpy(block.data_type,"Etudiant");
        // Boucler à travers les données et écrire des blocs dans le fichier
    for(int i = 0;i<n;i=i+facteur_blockage){
                // Définir la taille du bloc en fonction du nombre d'éléments et de la taille de chaque élément
        block.block_size = size_of_element *facteur_blockage;
                // Calculer la taille totale de la mémoire pour le bloc
        size_t block_memory_size = sizeof(block_header)+block.block_size;
                // Allouer de la mémoire pour le buffer
        void* buffer = malloc(block_memory_size);
                // Copier l'en-tête de bloc et les données dans le buffer
        memcpy(buffer,&block,sizeof(block_header));
        memcpy(buffer+sizeof(block_header),T+i,size_of_element*facteur_blockage);
                // Copier l'en-tête de bloc et les données dans le tampon
        fwrite(buffer,block_memory_size,1,file);
        //*Libérer la mémoire allouée
        free(buffer);
    }
}




int main(){

    return 0;
}