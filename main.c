#include<stdio.h>
#include<stdlib.h>
#include<string.h>
// definir structure de l'etudiant
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
        memcpy(buffer+sizeof(block_header),(Etudiant*)T+i,size_of_element*facteur);
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

// Fonction de recherche d'un etudiant par matricule dans un fichier
int recherche (char*chemin_fichier,char*matricule_cible)
{
        // Ouverture du fichier en mode lecture 
        FILE* fichier=fopen(chemin_fichier,"rb");

        if (fichier==NULL)
        {
            printf("error lors du l'ouverture du fichier");
            return -1; 
        }

            // Lecture de l'entete du fichier
            file_header entete_fichier;
            fread(&entete_fichier,sizeof(file_header),1,fichier);

      // Declaration des structures pour stocker les donnees de l'etudiant et du bloc courant
         Etudiant etudiant_courant ;
         block_header  bloc_courant;
          // Boucle pour parcourir chaque bloc dans le fichier
          for (int i = 0; i < entete_fichier.nb_element; i++) {
           // Lecture de l'entete du bloc courant
           fread(&bloc_courant, sizeof(block_header), 1, fichier);
          // Lecture des donnees (etudiants) du bloc courant
           fread(&etudiant_courant, sizeof(Etudiant), bloc_courant.block_size / sizeof(Etudiant), fichier);
            // Boucle pour parcourir chaque etudiant dans le bloc courant
          for (int j = 0; j < bloc_courant.block_size / sizeof(Etudiant); j++) {
            // Comparaison du matricule de l'etudiant avec le matricule cible
            if (strcmp(etudiant_courant.matricule, matricule_cible) == 0) {
                // Affichage des details de l'etudiant trouve
                printf("Element trouve :\n");
                read_element(&etudiant_courant);
                // Fermeture du fichier et retour de l'indice du bloc ou l'element a ete trouve
                fclose(fichier);
                return i;
            }
            }
        }

    // Affichage si l'element n'est pas trouve
    printf("Element non trouve.\n");
    // Fermeture du fichier et retour d'un code d'erreur
    fclose(fichier);
    return -2;
}


void create_file(char* name, char*file_extension){
    // Concatenation du nom + extension
    char* Full_file_name = malloc(strlen(name) + strlen(file_extension) + 2);
    strcpy(Full_file_name, name);
    strcat(Full_file_name, file_extension);
    
    //verifier si le nom a éte deja utilisé
    FILE *Existence = fopen(Full_file_name,"rb");
    if (Existence == NULL){

    FILE *file = fopen(Full_file_name, "wb");
    //initialization du file header
    file_header header;
    strcpy(header.file_name , name);
    strcpy(header.file_extension , file_extension);
    header.file_size = sizeof(file_header);
    header.nb_element = 0;
    fwrite(&header, sizeof(file_header), 1 ,file);
    fclose(file);
    free(Full_file_name);
    }
    else 
    {
        fclose (Existence);
        printf("un fichier utilisant le meme nom existe deja, veuillez reessayer avec un autre nom");
    }

}

void read_file(char* file_path){
FILE* file = fopen(file_path,"rb");
file_header header_file;
fread(&header_file,sizeof(file_header),1,file);
Etudiant T[header_file.nb_element];
block_header header_block;
int i = 0;
while(fread(&header_block,sizeof(block_header),1,file)>0){
    if(header_block.real_nb_block_element!=0){
    fread(T+i,sizeof(Etudiant),header_block.real_nb_block_element,file);
    i = i + header_block.real_nb_block_element;
    if(header_block.facteur_blockage!=header_block.real_nb_block_element){

        fseek(file,(header_block.facteur_blockage-header_block.real_nb_block_element)*sizeof(Etudiant),SEEK_CUR);
    }
}
else{

    fseek(file,sizeof(Etudiant)*header_block.facteur_blockage,SEEK_CUR);
}

}

for(int j = 0 ; j < header_file.nb_element;j++)
    read_element(T[j]);

fclose(file);
}

void suppression(char* file_path, char* matricule_cible) {
    FILE* file = fopen(file_path, "r+b");

    if (file == NULL) {
        printf("Erreur lors de l'ouverture du fichier");
        return;
    }

    file_header header;
    fread(&header, sizeof(file_header), 1, file);
    Etudiant etudiant_courant;
    block_header bloc_courant;

    // Tableau pour stocker les blocs
    void* buffer = malloc(header.file_size);
    int buffer_index = 0;
    for (int i = 0; i < header.nb_element; i++) {
        fread(&bloc_courant, sizeof(block_header), 1, file);
        // Allocation d'un espace pour le bloc courant dans le buffer
        void* bloc_buffer = malloc(sizeof(block_header) + bloc_courant.block_size);
        // Copie de l'entete du bloc courant dans le buffer du bloc
        memcpy(bloc_buffer, &bloc_courant, sizeof(block_header));
        fread(bloc_buffer + sizeof(block_header), bloc_courant.block_size, 1, file);

        for (int j = 0; j < bloc_courant.real_nb_block_element; j++) {
            if (strcmp(((Etudiant*)(bloc_buffer + sizeof(block_header))) + j, matricule_cible) == 0) {
                // Si l'etudiant est trouve, ne pas le copier dans le buffer principal
                bloc_courant.real_nb_block_element--;

                // Liberation de la memoire allouee pour le buffer du bloc
                free(bloc_buffer);
                continue;
            }
            // Copier l'etudiant dans le buffer principal
            memcpy(buffer + buffer_index, bloc_buffer + sizeof(block_header) + j * sizeof(Etudiant), sizeof(Etudiant));
            buffer_index += sizeof(Etudiant);
        }
        free(bloc_buffer);
        // Mettre a jour l'entete du bloc
        memcpy(buffer + buffer_index, &bloc_courant, sizeof(block_header));
        buffer_index += sizeof(block_header);
    }
    
    fseek(file, 0, SEEK_SET);
    header.nb_element--;
    header.file_size = buffer_index;
    fwrite(&header, sizeof(file_header), 1, file);
    // Reecrire le buffer dans le fichier
    fseek(file, sizeof(file_header), SEEK_SET);
    fwrite(buffer, buffer_index - sizeof(file_header), 1, file);

    free(buffer);
    fclose(file);
}




int main() {
    
    char fetd[] = "etudiants.bin";
    Etudiant etudiant1 = {"222231546468", "Boualit", "mohamed", 15};
    Etudiant etudiant2 = {"222214864984", "chaabane", "rabah", 15};
    Etudiant etudiant3 = {"2231518484648", "akouira", "djemou", 15};

    // Creation du fichier s'il n'existe pas
    create_file(fetd, ".bin");

    // Insertion des etudiants dans le fichier
    insertion_block(fetd, &etudiant1, 1, sizeof(Etudiant), 1);
    insertion_block(fetd, &etudiant2, 1, sizeof(Etudiant), 1);
    insertion_block(fetd, &etudiant3, 1, sizeof(Etudiant), 1);

    // Affichage du contenu du fichier
    printf("Contenu du fichier apres insertion :\n");
    read_file(fetd);

    // Recherche d'un etudiant par matricule
    char matricule_cible[] = "222231546468";
    int indice = recherche(fetd, matricule_cible);
    if (indice != -1) {
        printf("L'etudiant avec le matricule %s a ete trouve a l'indice %d.\n", matricule_cible, indice);
    }

    // Suppression d'un etudiant par matricule
    suppression(fetd, matricule_cible);

    // Affichage du contenu du fichier apres suppression
    printf("\nContenu du fichier apres suppression :\n");
    read_file(filename);

    return 0;
}
