#ifndef HEADERS_H
#define HEADERS_H
#include <stdio.h>

typedef struct NODE {
    char activity[50];
    int interrupt_number;
    int time;
    struct NODE *next;
} line_t;

line_t *create_line(char *full_line);
line_t *add_to_process(line_t *node, line_t *head);
line_t *read_file(char *filename);
void simulate(line_t *process, char **vector_table);
int random_num(int max, int min);
int LOAD_PC(char **vector_table, int counter, line_t *cur_line, FILE *execution);
void cleanup(line_t *cur_line);
void write_execution(char *message);

#endif