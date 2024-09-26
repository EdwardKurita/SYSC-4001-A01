#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

//linked list for storing trace
typedef struct NODE {
    char activity[50];
    int interrupt_number;
    int time;
    struct NODE *next;
} line_t;

//function prototyping
line_t *create_line(char *full_line);
line_t *add_to_process(line_t *node, line_t *head);
line_t *read_file(char *filename);
void simulate(line_t *process);
int random_num(int max, int min);

//nothing here yet
int main(void){
    line_t *hello = read_file("C:\\Users\\phant\\CLionProjects\\SYSC-4001-A01\\trace.txt");
    simulate(hello);

    return 0;
}

//creating a new line from the trace file
line_t *create_line(char *full_line) {
    //allocating memory for the line
    line_t *new_line = malloc(sizeof(line_t));

    //getting the activity from the line
    strcpy(new_line->activity, strtok(full_line, " "));

    if (strcmp(new_line->activity, "CPU,") == 0) {
        //removing appended comma from CPU.
        new_line->activity[3] = '\0';
        //setting the interrupt_number to NULL for CPU lines because there is no interrupt in this step.
        new_line->interrupt_number = 0;
    } else {
        //using comma delimiter to get the interrupt_number for SYSCALL and END_IO
        new_line->interrupt_number = atoi(strtok(NULL, ", "));
    }
    //getting the time for each line
    new_line->time = atoi(strtok(NULL, " "));

    new_line->next = NULL;

    return new_line;
}

//function to add each line to the process in order
line_t *add_to_process(line_t *node, line_t *head) {
    line_t *curr = head;
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
line_t *read_file(char *filename) {
    char str[50];
    line_t *process_list = NULL;

    printf("%s\n", filename);

    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error opening file");
        exit(1);
    }

    while(fgets(str, 50, file)) {
        line_t *cur_line = create_line(str);
        process_list = add_to_process(cur_line, process_list);
    }

    fclose(file);

    return process_list;
}

void simulate(line_t *process) {
    line_t *cur_line = process;
    int flag = 0;
    int counter = 0;

    while (cur_line != NULL) {
        if (strcmp(cur_line->activity, "CPU") == 0 && flag == 0) {
            flag = 1;
        } else if (strcmp(cur_line->activity, "CPU") == 0 && flag == 1) {


        } else if (strcmp(cur_line->activity, "SYSCALL") == 0) {
            int rd_num1 = random_num(cur_line->time/2-10, 10);
            int rd_num2 = random_num(cur_line->time/2-10, 10);
            int rd_num3 = cur_line->time - (rd_num1 + rd_num2);

            printf ("%d, %d, %s: run the ISR\n", counter, rd_num1, cur_line->activity);
            counter += rd_num1;
            printf("%d, %d, transfer data\n", counter, rd_num2);
            counter += rd_num2;
            printf("%d, %d, check for errors\n", counter, rd_num3);
            counter += rd_num3;
            printf("%d, %d, IRET\n", counter, 1);
            counter += 1;

        } else if (strcmp(cur_line->activity, "END_IO") == 0) {
            flag = 0;

        } else {
            printf("Unexpected command");
            exit(1);
        }
        cur_line = cur_line->next;
    }
}

int random_num(int max, int min){
    return rand() % (max- min + 1) + min;
}
