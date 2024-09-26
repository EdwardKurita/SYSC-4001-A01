#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//linked list for storing trace
typedef struct NODE {
    char *activity;
    int interrupt_number;
    int time;
    struct NODE *next;
} line;

//linked list for storing trace (have to write function for parsing hex)
typedef struct VECT {
    int interrupt_number;
    int value;
    struct VECT *next;
} vect;

//function prototyping
line *create_line(char *full_line);
line *add_to_process(line *node, line *head);
line *read_file(char *filename);

//nothing here yet
int main(void){

    return 0;
}

//creating a new line from the trace file
line *create_line(char *full_line) {
    //allocating memory for the line
    line *new_line = (line *)malloc(sizeof(line));

    //allocating memory for the activity
    new_line->activity = malloc(10 * sizeof(char));

    //getting the activity from the line
    new_line->activity = strtok(full_line, " ");

    if (strcmp(new_line->activity, "CPU,") == 0) {
        //removing appended comma from CPU.
        new_line->activity[4] = '\0';
        //setting the interrupt_number to NULL for CPU lines because there is no interrupt in this step.
        new_line->interrupt_number = NULL;
    } else {
        //using comma delimiter to get the interrupt_number for SYSCALL and END_IO
        new_line->interrupt_number = atoi(strtok(NULL, ","));
    }
    //getting the time for each line
    new_line->time = atoi(strtok(NULL, " "));

    new_line->next = NULL;

    return new_line;
}

//function to add each line to the process in order
line *add_to_process(line *node, line *head) {
    line *curr = head;
    if (head == NULL) {
        head = node;
    } else {
        while (curr->next != NULL) {
            curr = curr->next;
        }
        curr->next = node;
    }
    return head;
}

//function to read the trace file and add each process to the trace linked list
line *read_file(char *filename) {
    FILE
}