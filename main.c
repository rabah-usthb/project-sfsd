#include <gtk/gtk.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// definir structure de l'etudiant
typedef struct Etudiant {
  char matricule[13];
  char nom[50];
  char prenom[50];
  float moyenne;
} Etudiant;

typedef struct file_header {
  size_t file_size;
  char file_name[50];
  char file_extension[20];
  int nb_element;
} file_header;

typedef struct block_header {
  size_t block_size;
  int real_nb_block_element;
  int facteur_blockage;
  char data_type[50];
} block_header;

void read_element(Etudiant etudiant) {
  printf("nom : %s\n", etudiant.nom);
  printf("preom : %s\n", etudiant.prenom);
  printf("Matricule : %s\n", etudiant.matricule);
  printf("moyenne : %f\n", etudiant.moyenne);
}

void insertion_block(char *file_path, void *T, int facteur_blockage,
                     size_t size_of_element, int n) {
  FILE *file = fopen(file_path, "r+b");
  fseek(file, 0, SEEK_END);
  block_header block;
  file_header header;
  strcpy(block.data_type, "Etudiant");
  block.facteur_blockage = facteur_blockage;
  void *buffer;
  for (int i = 0; i < n; i = i + facteur_blockage) {
    int facteur;
    if (i + facteur_blockage > n) {
      facteur = facteur_blockage;
      while (i + facteur > n) {
        --facteur;
      }
      block.real_nb_block_element = facteur;
    } else {
      facteur = facteur_blockage;
      block.real_nb_block_element = facteur_blockage;
    }
    block.block_size = size_of_element * facteur_blockage;
    size_t block_memory_size = sizeof(block_header) + block.block_size;
    buffer = malloc(block_memory_size);
    memcpy(buffer, &block, sizeof(block_header));
    memcpy(buffer + sizeof(block_header), (Etudiant *)T + i,
           size_of_element * facteur);
    fwrite(buffer, block_memory_size, 1, file);
  }

  free(buffer);

  fseek(file, 0, SEEK_SET);
  fread(&header, sizeof(file_header), 1, file);
  header.nb_element += n;
  fseek(file, 0, SEEK_END);
  header.file_size = ftell(file);
  fseek(file, 0, SEEK_SET);
  fwrite(&header, sizeof(file_header), 1, file);
  fclose(file);
}

void create_file(char *name, char *file_extension) {
  char *Full_file_name = malloc(strlen(name) + strlen(file_extension) + 2);
  strcpy(Full_file_name, name);
  strcat(Full_file_name, file_extension);

  FILE *file = fopen(Full_file_name, "wb");
  if (file == NULL) {
    // Handle file creation failure
    printf("Failed to create file\n");
    free(Full_file_name);
    return;
  }

  file_header header;
  strcpy(header.file_name, name);
  strcpy(header.file_extension, file_extension);
  header.file_size = sizeof(file_header);
  header.nb_element = 0;
  fwrite(&header, sizeof(file_header), 1, file);
  fclose(file);

  GtkBuilder *builder = gtk_builder_new();
  gtk_builder_add_from_file(builder, "design5.glade", NULL);
  GtkListBox *fileList =
      GTK_LIST_BOX(gtk_builder_get_object(builder, "ListFIle"));

  // Create a new button with a label
  GtkWidget *newButton = gtk_button_new_with_label(Full_file_name);

  // Connect a callback (signal) to the button, if needed
  // g_signal_connect(newButton, "clicked", G_CALLBACK(your_callback_function),
  // NULL);

  // Add the new button to the GtkListBox
  gtk_list_box_insert(fileList, newButton, -1);

  // Show all widgets
  gtk_widget_show_all(GTK_WIDGET(fileList));

  free(Full_file_name);
  g_object_unref(builder);
}
void read_file(char *file_path) {
  FILE *file = fopen(file_path, "rb");
  file_header header_file;
  fread(&header_file, sizeof(file_header), 1, file);
  Etudiant T[header_file.nb_element];
  block_header header_block;
  int i = 0;
  while (fread(&header_block, sizeof(block_header), 1, file) > 0) {
    if (header_block.real_nb_block_element != 0) {
      fread(T + i, sizeof(Etudiant), header_block.real_nb_block_element, file);
      i = i + header_block.real_nb_block_element;
      if (header_block.facteur_blockage != header_block.real_nb_block_element) {

        fseek(file,
              (header_block.facteur_blockage -
               header_block.real_nb_block_element) *
                  sizeof(Etudiant),
              SEEK_CUR);
      }
    } else {

      fseek(file, sizeof(Etudiant) * header_block.facteur_blockage, SEEK_CUR);
    }
  }

  for (int j = 0; j < header_file.nb_element; j++)
    read_element(T[j]);

  fclose(file);
}

void search(char *file_path, char *mat) {
  FILE *file = fopen(file_path, "rb");
  file_header header_file;
  fread(&header_file, sizeof(file_header), 1, file);
  Etudiant T[header_file.nb_element];
  block_header block;
  int found = 0;
  int i = 0;
  int cx;
  int j;
  while (fread(&block, sizeof(block_header), 1, file) > 0 && found == 0) {
    if (block.real_nb_block_element != 0) {
      fread(T + i, sizeof(Etudiant), block.real_nb_block_element, file);
      cx = block.real_nb_block_element;
      j = i;
      while (cx > 0) {
        if (strcmp(T[j].matricule, mat) == 0) {
          read_element(T[j]);
          found = 1;
          break;
        }
        j++;
        cx--;
      }
      i = i + block.real_nb_block_element;
      if (block.real_nb_block_element != block.facteur_blockage) {
        int empty = block.facteur_blockage - block.real_nb_block_element;
        fseek(file, sizeof(Etudiant) * empty, SEEK_CUR);
      }
    }

    else {
      fseek(file, sizeof(Etudiant) * (block.facteur_blockage), SEEK_CUR);
    }
  }

  if (found == 0) {
    printf("\nNot Found\n");
  }
  fclose(file);
}

void supprimer(char *file_path, char *mat) {
  FILE *file = fopen(file_path, "r+b");
  file_header header;
  block_header block;
  fread(&header, sizeof(file_header), 1, file);
  Etudiant T[header.nb_element];
  int found = 0;
  int i = 0;
  long offset;
  int j;
  int cx;
  while (fread(&block, sizeof(block_header), 1, file) > 0 && found == 0) {
    if (block.real_nb_block_element != 0) {

      fread(T + i, sizeof(Etudiant), block.real_nb_block_element, file);
      offset = sizeof(block_header) +
               sizeof(Etudiant) * (block.real_nb_block_element);
      j = i;
      cx = block.real_nb_block_element;
      while (cx > 0) {
        if (strcmp(mat, T[j].matricule) == 0) {
          found = 1;
          --block.real_nb_block_element;
          void *buffer = malloc(sizeof(block_header) +
                                sizeof(Etudiant) * (block.facteur_blockage));
          memcpy(buffer, &block, sizeof(block_header));
          if (i == j) {
            // memcpy(buffer + sizeof(block_header), T + 9, sizeof(Etudiant));
            memcpy(buffer + sizeof(block_header), T + (i + 1),
                   sizeof(Etudiant) * (block.real_nb_block_element));

          } else if (j == (i + block.real_nb_block_element)) {
            memcpy(buffer + sizeof(block_header), T + i,
                   sizeof(Etudiant) * (block.real_nb_block_element));
          } else {
            memcpy(buffer + sizeof(block_header), T + i,
                   sizeof(Etudiant) * (j - i + 1));
            memcpy(buffer + sizeof(block_header) +
                       sizeof(Etudiant) * (j + i + 1),
                   T + j + 1,
                   sizeof(Etudiant) * (block.real_nb_block_element - j));
          }
          fseek(file, -offset, SEEK_CUR);
          fwrite(buffer,
                 sizeof(block_header) +
                     sizeof(Etudiant) * (block.facteur_blockage),
                 1, file);
          free(buffer);
          break;
        }
        --cx;
        ++j;
      }
    } else {
      fseek(file, sizeof(Etudiant) * block.facteur_blockage, SEEK_CUR);
    }

    i = i + block.real_nb_block_element;
  }
  fseek(file, 0, SEEK_END);
  --header.nb_element;
  header.file_size = ftell(file);
  fseek(file, 0, SEEK_SET);
  fwrite(&header, sizeof(file_header), 1, file);
  fclose(file);
}

int exist(char *file_name, char *file_extension) {
  char *full_name = malloc(strlen(file_extension) + strlen(file_name) + 1);
  strcpy(full_name, file_name);
  strcat(full_name, file_extension);
  FILE *file = fopen(full_name, "rb");
  if (file == NULL) {
    return 0;
  } else {
    fclose(file);
    return 1;
  }
}

void retrieve_input_file_name(GtkButton *button, gpointer user_data) {
  GtkBuilder *builder = GTK_BUILDER(user_data);
  GtkWidget *fileNameEntry =
      GTK_WIDGET(gtk_builder_get_object(builder, "FileNameField"));
  GtkWidget *fileExtensionEntry =
      GTK_WIDGET(gtk_builder_get_object(builder, "FileExtensionField"));
  char file_name[50], file_extension[50];
  strcpy(file_name, gtk_entry_get_text(GTK_ENTRY(fileNameEntry)));
  strcpy(file_extension, gtk_entry_get_text(GTK_ENTRY(fileExtensionEntry)));
  if (exist(file_name, file_extension) == 0) {
    create_file(file_name, file_extension);
    GtkWidget *window =
        GTK_WIDGET(gtk_builder_get_object(builder, "InfoCreate"));
    gtk_window_close(GTK_WINDOW(window));
    gtk_main_quit();
  }
}
void create_file_gtk(GtkButton *button, gpointer user_data) {
  GtkBuilder *create_builder = gtk_builder_new();
  gtk_builder_add_from_file(create_builder, "fillCreate.glade", NULL);
  GtkWidget *window =
      GTK_WIDGET(gtk_builder_get_object(create_builder, "InfoCreate"));
  gtk_widget_show_all(window);
  GtkWidget *ConfirmCreateButton =
      GTK_WIDGET(gtk_builder_get_object(create_builder, "ConfirmCreateButton"));
  g_signal_connect(ConfirmCreateButton, "clicked",
                   G_CALLBACK(retrieve_input_file_name), create_builder);
  gtk_main();
}

int main(int argc, char *argv[]) {
  // Initialize GTK
  gtk_init(&argc, &argv);

  // Create a new GtkBuilder
  GtkBuilder *builder = gtk_builder_new();

  // Load the UI definition from file
  gtk_builder_add_from_file(builder, "design5.glade", NULL);

  // Get the main window
  GtkWidget *window = GTK_WIDGET(gtk_builder_get_object(builder, "MyWindow"));
  GtkWidget *CreateButton =
      GTK_WIDGET(gtk_builder_get_object(builder, "CreateButton"));
  g_signal_connect(CreateButton, "clicked", G_CALLBACK(create_file_gtk), NULL);

  // Show the window
  gtk_widget_show_all(window);

  // Run the GTK main loop
  gtk_main();

  // Clean up
  g_object_unref(builder);

  return 0;
}
