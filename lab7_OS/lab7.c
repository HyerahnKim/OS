#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/stat.h> // For fstat
#include <fcntl.h>    // For open

#define BUFFER_SIZE 1024

void reverse_string(const char *input, char *reversed)
{
    int len = strlen(input);
    for (int i = 0; i < len; i++)
    {
        reversed[i] = input[len - i - 1];
    }
    reversed[len] = '\0';
}

void analyze_content(const char *input, char *output)
{
    char digits[BUFFER_SIZE] = {0}, uppercase[BUFFER_SIZE] = {0}, lowercase[BUFFER_SIZE] = {0}, others[BUFFER_SIZE] = {0};
    int digit_count = 0, upper_count = 0, lower_count = 0, other_count = 0;

    // Analyze the string
    for (int i = 0; input[i] != '\0'; i++)
    {
        if (isdigit(input[i]))
        {
            digits[digit_count++] = input[i];
        }
        else if (isupper(input[i]))
        {
            uppercase[upper_count++] = input[i];
        }
        else if (islower(input[i]))
        {
            lowercase[lower_count++] = input[i];
        }
        else
        {
            others[other_count++] = input[i];
        }
    }

    // Format the result
    // <int N><N chars><int M><M chars><int L><L chars><K chars>
    sprintf(output, "%d%s%d%s%d%s%s",
            digit_count, digits,
            upper_count, uppercase,
            lower_count, lowercase,
            others);
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <file_path>\n", argv[0]);
        return 1;
    }

    // Open the file
    int file_fd = open(argv[1], O_RDONLY);
    if (file_fd == -1)
    {
        perror("Error opening file");
        return 1;
    }

    // Use fstat to get file size
    struct stat file_stat;

    // Open the file and read the content
    if (fstat(file_fd, &file_stat) == -1)
    {
        perror("Error getting file stats");
        close(file_fd);
        return 1;
    }

    long fileSize = file_stat.st_size;

    // Dynamically allocate memory for file content
    char *fileContent = (char *)malloc(fileSize + 1); // +1 for null terminator

    // Read file content
    if (read(file_fd, fileContent, fileSize) != fileSize) // read from file_fd and save the content to the variable fileContent(String)
    {
        perror("Error reading file");
        free(fileContent);
        close(file_fd);
        return 1;
    }
    fileContent[fileSize] = '\0'; // The last slot is a Null-terminater the string
    close(file_fd);

    // Pipes: fd1 for parent-to-child, fd2 for child-to-parent
    int fd1[2], fd2[2];
    // If pipes are not created
    if (pipe(fd1) == -1 || pipe(fd2) == -1)
    {
        perror("Error creating pipes");
        free(fileContent);
        return 1;
    }

    if (fork() == 0) // we are at the Child process
    {
        close(fd1[1]); // Close write end of fd1 because we will only read from fd1
        close(fd2[0]); // Close read end of fd2 because we will only write to fd2

        char received[BUFFER_SIZE] = {0};
        read(fd1[0], received, BUFFER_SIZE); // Read reversed string from parent
        close(fd1[0]);                       // fd1 is all closed but fd2 is still open

        char analyzed[BUFFER_SIZE] = {0};
        analyze_content(received, analyzed); // Call the function "analyze_content" on the received string from parent process to find the final information

        write(fd2[1], analyzed, strlen(analyzed) + 1); // Send result to parent
        close(fd2[1]);
        exit(0);
    }

    // Parent process
    close(fd1[0]); // Close read end of fd1
    close(fd2[1]); // Close write end of fd2

    // Reverse the file content and send to child
    char reversed[BUFFER_SIZE] = {0};
    reverse_string(fileContent, reversed);         // reverse the file content string and save it to the variable reversed
    write(fd1[1], reversed, strlen(reversed) + 1); // the reversed string is saved in the write part of fd1
    close(fd1[1]);                                 // close the pipe fd1

    // Receive the result from child to parent using pipe fd2
    char result[BUFFER_SIZE] = {0};
    read(fd2[0], result, BUFFER_SIZE);
    close(fd2[0]);

    // Print the result
    printf("Processed Content: %s\n", result);

    // Free allocated memory
    free(fileContent);

    return 0;
}
