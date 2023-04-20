#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct IndexNode {
    long position;
    struct IndexNode *next;
} IndexNode;

#define TABLE_SIZE 1160

//Hash function that takes the sourceid as input and returns the hashed value.
 

int hash_function(int sourceid) {
    return sourceid % TABLE_SIZE;
}


/**
 * Reads the CSV file and creates an index file based on the sourceid.
 * The index file will be used to speed up searches in the CSV file.
 */

void create_indexed_file(const char *csv_filename, const char *index_filename) {
    FILE *csv_file = fopen(csv_filename, "r");
    if (csv_file == NULL) {
        perror("Error opening CSV file");
        return;
    }

    FILE *index_file = fopen(index_filename, "wb");
    if (index_file == NULL) {
        perror("Error opening index file");
        fclose(csv_file);
        return;
    }

    // Allocate memory for the hash table
    IndexNode **hash_table = (IndexNode **)calloc(TABLE_SIZE, sizeof(IndexNode *));

    char line[256];
    long position = ftell(csv_file);
    fgets(line, sizeof(line), csv_file); // Skip header line

    // Read the CSV file line by line and add entries to the hash table

    while (fgets(line, sizeof(line), csv_file)) {
        int sourceid;
        sscanf(line, "%d", &sourceid);
        int index = hash_function(sourceid);

        IndexNode *node = (IndexNode *)malloc(sizeof(IndexNode));
        node->position = position;
        node->next = hash_table[index];
        hash_table[index] = node;

        position = ftell(csv_file);
    }

    // Write the hash table to the index file

    for (int i = 0; i < TABLE_SIZE; i++) {
        IndexNode *current = hash_table[i];
        while (current != NULL) {
            fwrite(&i, sizeof(int), 1, index_file);
            fwrite(&current->position, sizeof(long), 1, index_file);
            current = current->next;
        }
    }
    // Close files

    fclose(index_file);
    fclose(csv_file);

    // Free memory
    for (int i = 0; i < TABLE_SIZE; i++) {
        IndexNode *current = hash_table[i];
        while (current != NULL) {
            IndexNode *temp = current;
            current = current->next;
            free(temp);
        }
    }
    free(hash_table);
}


/**
 * Main function that calls create_indexed_file to create an index file.
 */

int main() {
    create_indexed_file("uber_movement.csv", "index_file.bin");
    return 0;
}
