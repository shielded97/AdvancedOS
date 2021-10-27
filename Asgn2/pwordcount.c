/*
    Aidan Lambrecht
    Advanced Operating Systems
    Project 2: Wordcount
        - Take in a text file, use two pipes to count the words, return wordcount
*/

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#define BUFFER_SIZE 255
#define READ_END  0
#define WRITE_END 1

//returns an open file according to the name passed in via user input
FILE* openFile( char *fileName ) {
    char title[255] = ""; // file name is max of 255 characters if input after program has begun
    FILE *fp1;
    char stream[255];

    //open the file
    if ((fp1 = fopen(fileName,"r")) == NULL) {
        printf("Please enter a file name. \n");
        printf("Usage: ./pwordcount <file_name>\n");
        printf("Type 'exit' to quit, or please enter one valid text file name: ");
        scanf("%s", &title);
        if (strcmp(title, "exit") == 0) {
            return fp1;
        }
        while ((fp1 = fopen(title,"r")) == NULL) {
            printf("Enter a valid text file name: ");
            scanf("%s", &title);
            if (strcmp(title, "exit") == 0) {
            return fp1;
            }
        }
    }
    return fp1;
}

//returns the contents of the file
char readFile(FILE *fp, int buffSize) {
    char *stream[BUFFER_SIZE];
    fgets(stream, 255, (FILE*)fp);
    // printf(stream, "\n");
    // fclose(fp);
    return stream;
}

//returns the number of words in the given stream
int wordCount(char *stream) {
    //get whole stream, add 1 to count when encountering a whitespace followed by a character
    int count = 0;
    int flag = 0; //0 when last char was not a space, 1 otherwise
    
    //if first char is not whitespace, start count at 1
    if (*stream != '\n' && *stream != '\t' && *stream != ' ') {
        count = 1;
    }
    while (*stream != NULL) {
        if (*stream == '\n' || *stream == '\t' || *stream == ' ') {
            flag = 1; //trip the flag if encountering any whitespace
        }
        else if (flag == 1 && (*stream != '\n' || *stream != '\t' || *stream != ' ')) {
            count = count + 1; //add to count
            flag = 0; //reset flag
        }
        stream++;
    }
    return count;
}

int main( int argc, char *argv[])
{
    //----------------------------------
    // Variable Declaration
    //----------------------------------
    int fd0[2];
    int fd1[2];
    pid_t pid;
    char *stream[BUFFER_SIZE];
    char *reading[BUFFER_SIZE];
    FILE *fp;

    // create pipe
    if (pipe(fd0) == -1) {
        printf("pipe fd failed");
        return 0;
    }

    if (pipe(fd1) == -1) {
        printf("pipe fd1 failed");
        return 0;
    }

    // create second process
    pid = fork();

    if (pid < 0) {
        printf("fork failed");
        return 0;
    }

    // if parent proc
    if (pid > 0) {
        //open file
        fp = openFile(argv[1]);
        if (fp == NULL) {
            return 0;
        }

        //read file
        printf("Process 1 is reading file “input.txt” now ...\n");

        //send the data to the child proc to be counted
        close(fd0[READ_END]); 
        printf("Process 1 starts sending data to Process 2 ...\n");
        int c = 0;
        while (fgets(stream, 255, (FILE*)fp) != NULL) {
            c = c + wordCount(stream);
            write(fd0[WRITE_END], stream, BUFFER_SIZE);
        }
        fclose(fp); //close file after finished reading
        close(fd0[WRITE_END]); //close write-end of first pipe
        printf("Process 1 counted %d words. . .\n", c);

        //wait for response from child
        wait(NULL);

        //read info from child proc
        close(fd1[WRITE_END]);
        char *count;
        read(fd1[READ_END], &count, sizeof(count));
        close(fd1[READ_END]);
        
        //print result
        printf("Process 1: The total number of words is %d", count);
        printf(".\n\n");
        
    }
    
    // else if child proc
    else {
        close(fd0[WRITE_END]);
        printf("Process 2 finishes receiving data from Process 1 ...\n");
        printf("Process 2 is counting words now ...\n");

        //call wordCount() until all batches complete
        int count = 0; 
        while (read(fd0[READ_END], reading, BUFFER_SIZE) != NULL) {
            count = count + wordCount(reading);
        }
        //send count to parent
        close(fd0[READ_END]);

        printf("Process 2 counted %d words. . .\n", count);
        printf("Process 2 is sending the result back to Process 1 ...\n");
        write(fd1[WRITE_END], &count, sizeof(count));
        close(fd1[WRITE_END]);

        //kill child to avoid zombie
        exit(0);
    }
    return 0;
}