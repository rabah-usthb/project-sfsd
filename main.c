#include <gtk/gtk.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

GtkBuilder *globalbuilder;
GtkBuilder *create_builder;
GtkBuilder *FileExist_builder;
GtkBuilder *insertionBuilder;
GtkWidget *FileWindow;
GtkBuilder *insertioninputBuilder;
GtkBuilder *facteur_builder;
GtkWidget *FacteurWindow;
int facteur;
int nb_element;
int current_student;
char globale_path[50];
typedef struct config_path {
  char path[50];
} config_path;

typedef struct Etudiant {
  char matricule[13];
  char nom[50];
  char prenom[50];
  float moyenne;
} Etudiant;
Etudiant *T;
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

void read_file(GtkButton *button, gpointer user_data);

void write_config(char *Full_file_name) {
  config_path file_path;
  strcpy(file_path.path, Full_file_name);
  FILE *file = fopen("config.bin", "r+b");
  fseek(file, 0, SEEK_END);
  fwrite(&file_path, sizeof(config_path), 1, file);
  fseek(file, 0, SEEK_SET);
  file_header header;
  fread(&header, sizeof(file_header), 1, file);
  ++header.nb_element;
  fseek(file, 0, SEEK_SET);
  fwrite(&header, sizeof(file_header), 1, file);
  fclose(file);
}

void read_config(GtkListBox *listfile) {
  FILE *file = fopen("config.bin", "rb");
  file_header header;
  fread(&header, sizeof(file_header), 1, file);
  config_path T[header.nb_element];
  fread(T, sizeof(config_path), header.nb_element, file);
  fclose(file);
  for (int i = 0; i < header.nb_element; i++) {
    GtkWidget *newButton = gtk_button_new_with_label(T[i].path);
    g_signal_connect(newButton, "clicked", G_CALLBACK(read_file), NULL);
    GtkWidget *row = gtk_list_box_row_new();
    gtk_container_add(GTK_CONTAINER(row), newButton);
    gtk_list_box_insert(listfile, row, -1);
  }
}
bool IsEmpty(char *file_path) {
  FILE *file = fopen(file_path, "rb");
  file_header header;
  fread(&header, sizeof(file_header), 1, file);
  fclose(file);
  if (header.nb_element == 0) {
    return true;

  } else {
    return false;
  }
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

void retrieve_student(GtkButton *button, gpointer user_data) {
  GtkEntry *matriculeEntry =
      GTK_ENTRY(gtk_builder_get_object(insertionBuilder, "MatriculeField"));
  GtkEntry *nomEntry =
      GTK_ENTRY(gtk_builder_get_object(insertionBuilder, "LastNameField"));
  GtkEntry *prenomEntry =
      GTK_ENTRY(gtk_builder_get_object(insertionBuilder, "FirstNameField"));
  GtkEntry *gradeEntry =
      GTK_ENTRY(gtk_builder_get_object(insertionBuilder, "GradeField"));

  if (current_student < nb_element) {
    strcpy(T[current_student].matricule,
           gtk_entry_get_text(GTK_ENTRY(matriculeEntry)));
    strcpy(T[current_student].nom, gtk_entry_get_text(GTK_ENTRY(nomEntry)));
    strcpy(T[current_student].prenom,
           gtk_entry_get_text(GTK_ENTRY(prenomEntry)));
    T[current_student].moyenne =
        atof(gtk_entry_get_text(GTK_ENTRY(gradeEntry)));
    read_element(T[current_student]);
    ++current_student;

    gtk_entry_set_text(matriculeEntry, "");
    gtk_entry_set_text(prenomEntry, "");
    gtk_entry_set_text(nomEntry, "");
    gtk_entry_set_text(gradeEntry, "");

    GtkWidget *label =
        GTK_WIDGET(gtk_builder_get_object(insertionBuilder, "student"));
    char label_text[50];
    sprintf(label_text, "Student %d", current_student + 1);
    gtk_label_set_text(GTK_LABEL(label), label_text);
  }

  if (current_student == nb_element) {
    GtkWidget *windowstudent =
        GTK_WIDGET(gtk_builder_get_object(insertionBuilder, "StudentWindow"));
    gtk_window_close(GTK_WINDOW(windowstudent));
    insertion_block(globale_path, (Etudiant *)T, facteur, sizeof(Etudiant),
                    nb_element);
    free(T);
  }
}

void insertion_gtk(GtkButton *button, gpointer user_data);

void retrieve_insertion(GtkButton *button, gpointer user_data) {
  GtkEntry *nbfield =
      GTK_ENTRY(gtk_builder_get_object(insertioninputBuilder, "NbField"));
  nb_element = atoi(gtk_entry_get_text(nbfield));
  GtkEntry *facteurfield =
      GTK_ENTRY(gtk_builder_get_object(insertioninputBuilder, "FacteurField"));
  facteur = atoi(gtk_entry_get_text(facteurfield));
  GtkWidget *window = GTK_WIDGET(
      gtk_builder_get_object(insertioninputBuilder, "InsertionWindow"));
  gtk_window_close(GTK_WINDOW(window));
  if (facteur <= nb_element) {
    g_object_unref(insertionBuilder);
    insertionBuilder = gtk_builder_new();
    gtk_builder_add_from_file(insertionBuilder, "insertion.glade", NULL);
    GtkWidget *windowstudent =
        GTK_WIDGET(gtk_builder_get_object(insertionBuilder, "StudentWindow"));
    T = (Etudiant *)malloc(sizeof(Etudiant) * nb_element);
    GtkWidget *label =
        GTK_WIDGET(gtk_builder_get_object(insertionBuilder, "student"));
    current_student = 0;
    char label_text[50];
    sprintf(label_text, "Student %d", current_student + 1);
    gtk_label_set_text(GTK_LABEL(label), label_text);
    gtk_widget_show_all(windowstudent);
    GtkWidget *confirmbutton =
        GTK_WIDGET(gtk_builder_get_object(insertionBuilder, "ConfirmButton"));
    g_signal_connect(confirmbutton, "clicked", G_CALLBACK(retrieve_student),
                     NULL);
  } else {
    g_object_unref(facteur_builder);
    facteur_builder = gtk_builder_new();
    gtk_builder_add_from_file(facteur_builder, "Facteur.glade", NULL);
    FacteurWindow =
        GTK_WIDGET(gtk_builder_get_object(facteur_builder, "FileWindow"));
    gtk_widget_show_all(FacteurWindow);
    GtkWidget *RetryButton =
        GTK_WIDGET(gtk_builder_get_object(facteur_builder, "RetryButton"));
    g_signal_connect(RetryButton, "clicked", G_CALLBACK(insertion_gtk), NULL);
  }
}

void insertion_gtk(GtkButton *button, gpointer user_data) {

  if (FacteurWindow != NULL && gtk_widget_get_visible(FacteurWindow)) {
    gtk_widget_destroy(FacteurWindow);
  }

  g_signal_handlers_disconnect_by_func(button, G_CALLBACK(retrieve_insertion),
                                       NULL);
  g_object_unref(insertioninputBuilder);
  insertioninputBuilder = gtk_builder_new();
  gtk_builder_add_from_file(insertioninputBuilder, "insertioninput.glade",
                            NULL);
  GtkWidget *window = GTK_WIDGET(
      gtk_builder_get_object(insertioninputBuilder, "InsertionWindow"));
  gtk_widget_show_all(window);
  GtkWidget *confirmbutton = GTK_WIDGET(
      gtk_builder_get_object(insertioninputBuilder, "ConfirmButton"));
  g_signal_connect(confirmbutton, "clicked", G_CALLBACK(retrieve_insertion),
                   NULL);
}

void read_file(GtkButton *button, gpointer user_data) {
  char *file_path = g_strdup(gtk_button_get_label(button));
  FILE *file = fopen(file_path, "rb");
  file_header header_file;
  fread(&header_file, sizeof(file_header), 1, file);

  GtkTextView *textView =
      GTK_TEXT_VIEW(gtk_builder_get_object(globalbuilder, "FileContent"));
  GtkTextBuffer *buffer = gtk_text_view_get_buffer(textView);
  gtk_text_buffer_set_text(buffer, "", -1);

  // Set font size and family (use a monospaced font)
  PangoFontDescription *font_desc = pango_font_description_new();
  pango_font_description_set_size(font_desc, 15 * PANGO_SCALE);
  pango_font_description_set_family(font_desc, "Monospace");

  GtkTextTagTable *tag_table = gtk_text_buffer_get_tag_table(buffer);

  // Check if the tag already exists
  GtkTextTag *tag = gtk_text_tag_table_lookup(tag_table, "my_tag");
  if (!tag) {
    tag = gtk_text_buffer_create_tag(buffer, "my_tag", "font-desc", font_desc,
                                     NULL);
  }

  // Set wrap mode to none
  gtk_text_view_set_wrap_mode(textView, GTK_WRAP_NONE);

  if (header_file.nb_element != 0) {
    Etudiant Tab[header_file.nb_element];
    block_header header_block;
    int i = 0;
    while (fread(&header_block, sizeof(block_header), 1, file) > 0) {
      if (header_block.real_nb_block_element != 0) {
        fread(Tab + i, sizeof(Etudiant), header_block.real_nb_block_element,
              file);
        i = i + header_block.real_nb_block_element;
        if (header_block.facteur_blockage !=
            header_block.real_nb_block_element) {
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

    // Append the file content to the GtkTextView with the specified tag
    for (int j = 0; j < header_file.nb_element; j++) {
      gchar *text = g_strdup_printf(
          "Nom: %s Prenom: %s Matricule: %s Moyenne: %.2f\n", Tab[j].nom,
          Tab[j].prenom, Tab[j].matricule, Tab[j].moyenne);

      GtkTextIter iter;
      gtk_text_buffer_get_end_iter(buffer, &iter);
      gtk_text_buffer_insert_with_tags_by_name(buffer, &iter, text, -1,
                                               "my_tag", NULL);

      g_free(text);
    }
  } else {
    // If the file is empty, insert a message with the specified font
    gchar *empty_text = g_strdup_printf("File is empty.\n");
    GtkTextIter iter;
    gtk_text_buffer_get_end_iter(buffer, &iter);
    gtk_text_buffer_insert_with_tags_by_name(buffer, &iter, empty_text, -1,
                                             "my_tag", NULL);
    g_free(empty_text);
  }

  pango_font_description_free(font_desc);
  strcpy(globale_path, file_path);
  free(file_path);
  GtkWidget *InsertionButton =
      GTK_WIDGET(gtk_builder_get_object(globalbuilder, "InsertionButton"));
  g_signal_handlers_disconnect_by_func(InsertionButton,
                                       G_CALLBACK(insertion_gtk), NULL);
  g_signal_connect(InsertionButton, "clicked", G_CALLBACK(insertion_gtk), NULL);
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

void create_file(char *name, char *file_extension, GtkListBox *listfile) {
  // Concatenation du nom + extension
  char *Full_file_name = malloc(strlen(name) + strlen(file_extension) + 1);
  strcpy(Full_file_name, name);
  strcat(Full_file_name, file_extension);

  // initialisation de l'entete du fichier
  FILE *file = fopen(Full_file_name, "wb");
  file_header header;
  strcpy(header.file_name, name);
  strcpy(header.file_extension, file_extension);
  header.file_size = sizeof(file_header);
  header.nb_element = 0;
  fwrite(&header, sizeof(file_header), 1, file);
  fclose(file);

  // Create a new button
  GtkWidget *newButton = gtk_button_new_with_label(Full_file_name);
  g_signal_connect(newButton, "clicked", G_CALLBACK(read_file), NULL);
  GtkWidget *row = gtk_list_box_row_new();
  gtk_container_add(GTK_CONTAINER(row), newButton);
  // Add the button to the ListBox
  gtk_list_box_insert(listfile, row, -1); // Use 'row' instead of 'newButton'
  gtk_widget_show_all(GTK_WIDGET(listfile));
  write_config(Full_file_name);
  free(Full_file_name);
}

void create_file_gtk(GtkButton *button);
void retrieve_input_file_name(GtkButton *button) {
  GtkWidget *window =
      GTK_WIDGET(gtk_builder_get_object(create_builder, "InfoCreate"));
  GtkWidget *fileNameEntry =
      GTK_WIDGET(gtk_builder_get_object(create_builder, "FileNameField"));
  GtkWidget *fileExtensionEntry =
      GTK_WIDGET(gtk_builder_get_object(create_builder, "FileExtensionField"));
  char file_name[50], file_extension[50];
  strcpy(file_name, gtk_entry_get_text(GTK_ENTRY(fileNameEntry)));
  strcpy(file_extension, gtk_entry_get_text(GTK_ENTRY(fileExtensionEntry)));

  if (exist(file_name, file_extension) == 0) {
    GtkListBox *filelist =
        GTK_LIST_BOX(gtk_builder_get_object(globalbuilder, "ListFIle"));
    create_file(file_name, file_extension, filelist);
    gtk_widget_destroy(window);
  } else {
    gtk_widget_destroy(window);
    g_object_unref(FileExist_builder);
    FileExist_builder = gtk_builder_new();
    gtk_builder_add_from_file(FileExist_builder, "FileExist.glade", NULL);
    FileWindow =
        GTK_WIDGET(gtk_builder_get_object(FileExist_builder, "FileWindow"));
    gtk_widget_show_all(FileWindow);
    GtkWidget *RetryButton =
        GTK_WIDGET(gtk_builder_get_object(FileExist_builder, "RetryButton"));
    g_signal_connect(RetryButton, "clicked", G_CALLBACK(create_file_gtk), NULL);
  }
}
void create_file_gtk(GtkButton *button) {
  if (FileWindow != NULL && gtk_widget_get_visible(FileWindow)) {
    gtk_widget_destroy(FileWindow);
  }
  g_object_unref(create_builder);
  create_builder = gtk_builder_new();
  gtk_builder_add_from_file(create_builder, "fillCreate.glade", NULL);
  GtkWidget *window =
      GTK_WIDGET(gtk_builder_get_object(create_builder, "InfoCreate"));
  gtk_widget_show_all(window);
  GtkWidget *ConfirmCreateButton =
      GTK_WIDGET(gtk_builder_get_object(create_builder, "ConfirmCreateButton"));
  g_signal_connect(ConfirmCreateButton, "clicked",
                   G_CALLBACK(retrieve_input_file_name), NULL);
  // gtk_main();
}

int main(int argc, char *argv[]) {
  // Initialize GTK
  gtk_init(&argc, &argv);

  globalbuilder = gtk_builder_new();
  create_builder = gtk_builder_new();
  FileExist_builder = gtk_builder_new();
  insertioninputBuilder = gtk_builder_new();
  insertionBuilder = gtk_builder_new();
  facteur_builder = gtk_builder_new();

  // Load the UI definition from file
  gtk_builder_add_from_file(globalbuilder, "design.glade", NULL);
  gtk_builder_add_from_file(create_builder, "fileCreate.glade", NULL);
  gtk_builder_add_from_file(FileExist_builder, "FileExist.glade", NULL);
  gtk_builder_add_from_file(insertioninputBuilder, "insertioninput.glade",
                            NULL);
  gtk_builder_add_from_file(insertionBuilder, "insertion.glade", NULL);
  gtk_builder_add_from_file(facteur_builder, "Facteur.glade", NULL);

  // Get the main window
  GtkWidget *window =
      GTK_WIDGET(gtk_builder_get_object(globalbuilder, "MyWindow"));
  GtkWidget *CreateButton =
      GTK_WIDGET(gtk_builder_get_object(globalbuilder, "CreateButton"));
  g_signal_connect(CreateButton, "clicked", G_CALLBACK(create_file_gtk), NULL);
  GtkListBox *listfile =
      GTK_LIST_BOX(gtk_builder_get_object(globalbuilder, "ListFIle"));
  if (IsEmpty("config.bin") == false) {
    read_config(listfile);
  }
  // Connect the destroy signal to exit the application
  g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

  // Show the window
  gtk_widget_show_all(window);

  // Start the GTK main loop
  gtk_main();

  return 0;
}
