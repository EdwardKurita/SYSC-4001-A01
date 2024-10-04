#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <ctype.h>
#include "interrupts.h"

int main(int argc, char **argv) {

    //hardcoded vector table
    char *vector_table[26] = {  "0X0282", "0X0A98", "0X0C7F", "0X0F73", "0X0B2A",
                                "0X0778", "0X0C8B", "0X021A", "0X0C44", "0X04B7",
                                "0X0BB1", "0X0E6E", "0X0640", "0X024A", "0X0BFD",
                                "0X0C6C", "0X0271", "0X055D", "0X05FB", "0X001B",
                                "0X05F1", "0X057E", "0X062A", "0X03D5", "0X07CF",
                                "0x0321"                                            };
    int num;
    char exec[30];

    if (argc == 0) { //checking if there is an additional argument that contains the trace file in the command line
        printf("No trace file detected.");
        exit(1);
    } else if (argc > 2) { //making sure there aren't any additional arguments
        printf("Too many arguments");
        exit(1);
    } else { //if there is a file name in the test it allows the program to progress

        char *check = argv[1];

        //loop that goes through each character in the filename to extract the n from "trace[n].txt"
        while (*check) {
            if (isdigit(*check)) {
                num = strtol(check, &check, 10);
            } else {
                check++;
            }
        }
        //names the output file appropriately in accordance to the number of the trace file
        sprintf(exec, "execution%d.txt", num);
    }

    //reads in the trace file to a linked list in order to sequentially execute the program
    line_t *trace_process = read_file(argv[1]);

    //saves the head of the trace process linked list in order to free the memory used by the linked list later.
    line_t *head = trace_process;

    //starts the simulation
    simulate(trace_process, vector_table, exec);

    //cleans up the heap memory used by the program stored in the trace linked list
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
        //using comma as delimiter to get the interrupt_number for SYSCALL and END_IO
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

    FILE *file = fopen(filename, "r");

    if (file == NULL) {
        printf("Error opening file1");
        exit(1);
    }

    while(fgets(str, 50, file)) {
        line_t *cur_line = create_line(str); //creates a new node that contains the command, interrupt number, and time of each line
        process_list = add_to_process(cur_line, process_list); //adds each node to the linked list, and returns head so iteration isnt broken
    }

    fclose(file);

    return process_list;
}

//function that controls the simulation.
void simulate(line_t *process, char **vector_table, char *output_filename) {

    line_t *cur_line = process;
    bool mode = false; //mode bit to alternate between kernel and user mode
    int counter = 0;
    int overhead = 0;
    int rand1 = 0;
    int rand2 = 0;
    int rand3 = 0;
    int rand4 = 0;

    //opening the output file to write
    FILE *execution = fopen(output_filename, "w");

    if (execution == NULL) {
        printf("Error opening file");
        exit(1);
    }

    //setting the time seed so random numbers are unique in each run of the code
    srand(time(NULL));

    while (cur_line != NULL) {
        //checking that the time value of the current line is possible given constraints
        if (!(cur_line->time > 0 && cur_line->time <= 400)) {
            printf("Duration of process is invalid.");
            fprintf(execution, "Duration of process is invalid\nProcess Exit");
            fclose(execution);
            exit(1);
        }

        if (strcmp(cur_line->activity, "CPU") == 0) {
            printf("%d, %d, CPU execution\n", counter, cur_line->time);
            fprintf(execution, "%d, %d, CPU execution\n", counter, cur_line->time);
            counter += cur_line->time;

        } else if (strcmp(cur_line->activity, "SYSCALL") == 0) {

            mode = false;

            printf("%d, %d, switch to kernel mode\n", counter, 1);
            fprintf(execution, "%d, %d, switch to kernel mode\n", counter, 1);
            counter++;
            overhead++;

            rand1 = random_num(3, 1);

            printf("%d, %d, context saved\n", counter, rand1);
            fprintf(execution, "%d, %d, context saved\n", counter, rand1);
            counter += rand1;
            overhead += rand1;

            check_vector_table(cur_line, execution);
            counter = LOAD_PC(vector_table, counter, cur_line, execution);
            overhead += 2;

            //next three lines for generating random values that sum to the time given by the SYSCALL activity
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
            overhead++;

            mode = true;

        } else if (strcmp(cur_line->activity, "END_IO") == 0) {

            printf("%d, %d, check priority of interrupt\n", counter, 1);
            fprintf(execution, "%d, %d, check priority of interrupt\n", counter, 1);
            counter++;
            overhead++;

            printf("%d, %d, check if masked\n", counter , 1);
            fprintf(execution, "%d, %d, check if masked\n", counter , 1);
            counter++;
            overhead++;

            mode = false;
            printf("%d, %d, switch to kernel mode\n", counter, 1);
            fprintf(execution, "%d, %d, switch to kernel mode\n", counter, 1);
            counter++;
            overhead++;

            rand1 = random_num(3, 1);

            printf("%d, %d, context saved\n", counter, rand1);
            fprintf(execution, "%d, %d, context saved\n", counter, rand1);
            counter += rand1;
            overhead += rand1;

            check_vector_table(cur_line, execution);
            counter = LOAD_PC(vector_table, counter, cur_line, execution);

            printf("%d, %d, END_IO\n", counter, cur_line->time);
            fprintf(execution, "%d, %d, END_IO\n", counter, cur_line->time);
            counter += cur_line->time;

            printf("%d, %d, IRET\n", counter, 1);
            fprintf(execution, "%d, %d, IRET\n", counter, 1);
            counter++;
            overhead++;

            mode = true;

        } else {
            //if, for whatever reason, there is a command word that isn't recognized, the program ends.
            printf("Unexpected command");
            fprintf(execution, "Unexpected command encountered\nProcess exit");
            fclose(execution);
            exit(1);

        }

        cur_line = cur_line->next;
    }
    //gives us the system overhead for program testing
    printf("\n\nTotal System Overhead: %dms\n\n", overhead);

    fclose(execution);
}

//function to generate random numbers whenever we need
int random_num(int max, int min){
    return rand() % (max- min + 1) + min;
}

//loading the PC
int LOAD_PC(char **vector_table, int counter, line_t *cur_line, FILE *execution) {

    char PC[7] = "";

    printf("%d, %d, find vector %d in memory position 0x%04x\n", counter, 1, cur_line->interrupt_number, 2*cur_line->interrupt_number);
    fprintf(execution, "%d, %d, find vector %d in memory position 0x%04x\n", counter, 1, cur_line->interrupt_number, 2*cur_line->interrupt_number);
    counter++;

    strcpy(PC, vector_table[cur_line->interrupt_number]);
    printf("%d, %d, load address %s into PC\n", counter, 1, PC);
    fprintf(execution, "%d, %d, load address %s into PC\n", counter, 1, PC);
    counter++;

    return counter;
}

//cleanup function using a reference to head to remove the linked list from heap memory
void cleanup(line_t *cur_line) {

    line_t *delete_line;

    while (cur_line != NULL) {
        delete_line = cur_line;
        cur_line = cur_line->next;
        free(delete_line);
    }

}

void check_vector_table(line_t *cur_line, FILE *execution) {
    if (cur_line->interrupt_number < 0 || cur_line->interrupt_number > 25) {
        printf("Interrupt number outside the bounds of the vector table");
        fprintf(execution, "Interrupt number outside the bounds of the vector table\nProcess exit");
        fclose(execution);
        exit(1);
    }
}