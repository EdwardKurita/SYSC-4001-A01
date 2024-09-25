#include <stdio.h>
#include <stdlib.h>
#include <string.h>


//each line contains the instruction, followed by


typedef struct LINE line;

typedef struct NODE node;

//using a linked list in order to hold/execute all process in order
//each node is a new line of the program
struct NODE {
    line *cur_line;
    struct NODE *next;
};

struct LINE {
    char activity[10];
    int address;
    int time;
};

line create_line(char *)



int main(void) {
    //pseudocode commenting:

    //initialize variables
        //file for reading from the vector table
        //file for reading the input file
        //file for writing the output to
        //int mode variable
        //variable for the loop instance
    FILE *file1 = fopen("trace.txt","r");
    FILE *file2 = fopen("vector_table.txt","r");
    FILE *file3 = fopen("execution.txt","w");
    int mode = 1; //starting in user mode

    //set up a 2D string array for each file
        //file 1
        //file 2

    //set up the third file for writing

    //while loop for repeating the code until EOF

        //read_from_sensor() aka reading from the input txt file, I guess we'll do it so it reads one line of the file per loop?
            //this needs to be able to read all the lines of the code at once, from CPU to END_IO, so maybe hard code it to read return 3 lines of the string array?

        //calculate the value that needs to be displayed from y
            //input of Sting array,

        //display the value

        //loop to EOF

    return 0;
}

//function for file reading
    //local variables:
        //3d string array (using code block, line, part of line, sectioning).

//function for value display
