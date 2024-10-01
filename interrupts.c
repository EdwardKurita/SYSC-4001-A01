#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "interrupts.h"

int main(void){

    char *vector_table[25] = {  "0X0282", "0X0A98", "0X0C7F", "0X0F73", "0X0B2A",
                                "0X0778", "0X0C8B", "0X021A", "0X0C44", "0X04B7",
                                "0X0BB1", "0X0E6E", "0X0640", "0X024A", "0X0BFD",
                                "0X0C6C", "0X0271", "0X055D", "0X05FB", "0X001B",
                                "0X05F1", "0X057E", "0X062A", "0X03D5", "0X07CF"   };

    line_t *trace_process = read_file("C:\\Users\\phant\\CLionProjects\\SYSC-4001-A01\\trace.txt");

    line_t *head = trace_process;

    simulate(trace_process, vector_table);

    cleanup(head);

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

//function that controls the simulation.
void simulate(line_t *process, char **vector_table) {
    line_t *cur_line = process;
    bool mode = false;
    int counter = 0;
    int rand1, rand2, rand3, rand4;

    FILE *execution = fopen("C:\\Users\\phant\\CLionProjects\\SYSC-4001-A01\\execution.txt", "w");

    if (execution == NULL) {
        printf("Error opening file");
        exit(1);
    }

    while (cur_line != NULL) {

        if (cur_line <= 0) {
            printf("Duration of process is invalid.");
            fprintf(execution, "Duration of process is invalid\nProcess Exit");
            fclose(execution);
            exit(1);
        }

        if (strcmp(cur_line->activity, "CPU") == 0 && !mode) {

            mode = true;
            rand1 = random_num(3, 1);

            printf("%d, %d, CPU execution\n", counter, cur_line->time);
            fprintf(execution, "%d, %d, CPU execution\n", counter, cur_line->time);
            counter += cur_line->time;

            printf("%d, %d, switch to kernel mode\n", counter, 1);
            fprintf(execution, "%d, %d, switch to kernel mode\n", counter, 1);
            counter++;

            printf("%d, %d, context saved\n", counter, rand1);
            fprintf(execution, "%d, %d, context saved\n", counter, rand1);
            counter += rand1;

        } else if (strcmp(cur_line->activity, "CPU") == 0 && mode) {

            rand1 = random_num(3, 1);

            printf("%d, %d, check priority of interrupt\n", counter, 1);
            fprintf(execution, "%d, %d, check priority of interrupt\n", counter, 1);
            counter++;

            printf("%d, %d, check if masked\n", counter , 1);
            fprintf(execution, "%d, %d, check if masked\n", counter , 1);
            counter++;

            printf("%d, %d, switch to kernel mode\n", counter, 1);
            fprintf(execution, "%d, %d, switch to kernel mode\n", counter, 1);
            counter++;

            printf("%d, %d, context saved\n", counter, rand1);
            fprintf(execution, "%d, %d, context saved\n", counter, rand1);
            counter += rand1;

        } else if (strcmp(cur_line->activity, "SYSCALL") == 0) {

            rand2 = random_num(cur_line->time / 2 - 10, 10);
            rand3 = random_num(cur_line->time / 2 - 10, 10);
            rand4 = cur_line->time - (rand2 + rand3);

            printf("%d, %d, %s: run the ISR\n", counter, rand2, cur_line->activity);
            fprintf(execution, "%d, %d, %s: run the ISR\n", counter, rand2, cur_line->activity);
            counter += rand2;

            printf("%d, %d, transfer data\n", counter, rand3);
            fprintf(execution, "%d, %d, transfer data\n", counter, rand3);
            counter += rand3;

            printf("%d, %d, check for errors\n", counter, rand4);
            fprintf(execution, "%d, %d, check for errors\n", counter, rand4);
            counter += rand4;

            printf("%d, %d, IRET\n", counter, 1);
            fprintf(execution, "%d, %d, IRET\n", counter, 1);
            counter++;

        } else if (strcmp(cur_line->activity, "END_IO") == 0) {

            mode = false;

            counter = LOAD_PC(vector_table, counter, cur_line, execution);

            printf("%d, %d, END_IO\n", counter, cur_line->time);
            fprintf(execution, "%d, %d, END_IO\n", counter, cur_line->time);
            counter += cur_line->time;

            printf("%d, %d, IRET\n", counter, 1);
            fprintf(execution, "%d, %d, IRET\n", counter, 1);
            counter++;

        } else {

            printf("Unexpected command");
            fprintf(execution, "Unexpected command encountered\nProcess exit");
            fclose(execution);
            exit(1);

        }

        cur_line = cur_line->next;

    }

    fclose(execution);
}

int random_num(int max, int min){
    return rand() % (max- min + 1) + min;
}

int LOAD_PC(char **vector_table, int counter, line_t *cur_line, FILE *execution) {

    char PC[7] = "";

    printf("%d, %d, find vector %d in memory position 0x%x\n", counter, 1, cur_line->interrupt_number, 2*cur_line->interrupt_number);
    fprintf(execution, "%d, %d, find vector %d in memory position 0x%x\n", counter, 1, cur_line->interrupt_number, 2*cur_line->interrupt_number);
    counter++;

    strcpy(PC, vector_table[cur_line->interrupt_number]);
    printf("%d, %d, load address %s into PC\n", counter, 1, PC);
    fprintf(execution, "%d, %d, load address %s into PC\n", counter, 1, PC);
    counter++;

    return counter;
}

void cleanup(line_t *cur_line) {

    line_t *delete_line;

    while (cur_line != NULL) {
        delete_line = cur_line;
        cur_line = cur_line->next;
        free(delete_line);
    }

}