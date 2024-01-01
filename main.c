#include<stdio.h>
#include<stdlib.h>
#include<string.h>
// définir structure de l'étudiant
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
    FILE* file = fopen(file_path,"r+b");
    fseek(file,0,SEEK_END);
    block_header block;
    file_header header;
    strcpy(block.data_type,"Etudiant");
    block.facteur_blockage = facteur_blockage;
    void* buffer;
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
         buffer = malloc(block_memory_size);
        memcpy(buffer,&block,sizeof(block_header));
        memcpy(buffer+sizeof(block_header),T+i,size_of_element*facteur);
        fwrite(buffer,block_memory_size,1,file);

    }

    free(buffer);

    fseek(file,0,SEEK_SET);
    fread(&header,sizeof(file_header),1,file);
    header.nb_element +=n;
    fseek(file,0,SEEK_END);
    header.file_size = ftell(file);
    fseek(file,0,SEEK_SET);
    fwrite(&header,sizeof(file_header),1,file);
    fclose(file);

}

// Fonction de recherche d'un étudiant par matricule dans un fichier
int recherche (char*chemin_fichier,char*matricule_cible)
{
        // Ouverture du fichier en mode lecture binaire
        FILE* fichier=fopen(chemin_fichier,"rb");

        if (fichier==NULL)
        {
            printf("error lors du l'ouverture du fichier");
            return -1; 
        }

            // Lecture de l'entête du fichier
            file_header entete_fichier;
            fread(&entete_fichier,sizeof(file_header),1,fichier);


      // Déclaration des structures pour stocker les données de l'étudiant et du bloc courant
         Etudiant etudiant_courant ;
         block_header  bloc_courant;

          // Boucle pour parcourir chaque bloc dans le fichier
          for (int i = 0; i < entete_fichier.nb_element; i++) {

           // Lecture de l'entête du bloc courant
           fread(&bloc_courant, sizeof(block_header), 1, fichier);

          // Lecture des données (étudiants) du bloc courant
           fread(&etudiant_courant, sizeof(Etudiant), bloc_courant.block_size / sizeof(Etudiant), fichier);

            // Boucle pour parcourir chaque étudiant dans le bloc courant
          for (int j = 0; j < bloc_courant.block_size / sizeof(Etudiant); j++) {

            // Comparaison du matricule de l'étudiant avec le matricule cible

            if (strcmp(etudiant_courant.matricule, matricule_cible) == 0) {

                // Affichage des détails de l'étudiant trouvé
                printf("Élément trouvé :\n");
                read_element(&etudiant_courant);
                // Fermeture du fichier et retour de l'indice du bloc où l'élément a été trouvé
                fclose(fichier);
                return i;
            }
            }
        }

    // Affichage si l'élément n'est pas trouvé
    printf("Élément non trouvé.\n");
    // Fermeture du fichier et retour d'un code d'erreur
    fclose(fichier);
    return -2;
}


void create_file(char* name, char*file_extension){
    // Concatenation du nom + extension
    char* Full_file_name = malloc(strlen(name) + strlen(file_extension) + 2);
    strcpy(Full_file_name, name);
    strcat(Full_file_name, file_extension);

    FILE *file = fopen(Full_file_name, "wb");

}


int main(){

    return 0;
}
