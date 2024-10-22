#include <ctype.h>
#include <stdlib.h>
#include <dirent.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

/* Pseudocode
function writeFileSizes(dir_path, output_file):
    for each item in dir_path:
        if item is a directory:
            if item is not "." or "..":
                writeFileSizes(item, output_file)  // Recursively traverse the subdirectory
        else if item is a regular file:// Here we finally end up with a reg file
            get the file size of item
            write the file name and size to output_file

main:
    source_directory = get command-line argument 1
    output_file = get command-line argument 2

    open output_file for writing
    traverse_directory(source_directory, output_file)
close output_file
*/

void writeFileSizes(char *dir_path, FILE *output_file)
{
    DIR *dir = opendir(dir_path);
    if (dir == NULL)
    {
        perror("NullPointer to dir_path");
        return; // returning nothing is only doable in void function.
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL)
    // it is null when it reaches the end. The loop continues as long as entry is not NULL, meaning that there are more files or directories to process.
    {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
        {
            continue; // to the next iteration. Also means Skip "." and ".." meaning if it's a directory we don't want to write the size into the file so skip directories.
        }

        // Construct the full path of the item (file or directory)
        char path[1024]; // This declares a character array path with a size of 1024 bytes to store the path.
        snprintf(path, sizeof(path), "%s/%s", dir_path, entry->d_name);

        struct stat statbuf;
        // The stat() function gets status information about a specified file and places it in the area of memory pointed to by the buf argument.

        if (stat(path, &statbuf) == -1) // If stat() returns -1, it means there was an error.
        {
            perror("Failed to get file status");
            continue; // to the next iteration
        }

        // If the item is a directory, recursively call writeFileSizes
        if (S_ISDIR(statbuf.st_mode))
        // S_ISDIR() checks if the file is a directory.
        // statbuf.st_mode contains information about the type of file
        // so this is if (this is a directory)
        {
            writeFileSizes(path, output_file); // recursive function call is called!
        }

        // If the item is a regular file, write its name and size to the output file
        else if (S_ISREG(statbuf.st_mode))
        // checks if the file is a regular file
        {
            fprintf(output_file, "File: %s, Size: %lld bytes\n", path, (long long)statbuf.st_size);
        }
        // %lld is a long long integer. If your file is larger than 2 GB, you cannot represent its size with an int because the number would overflow.int is 32 bit, longlong is 64 bit.
    }

    // Close the directory
    closedir(dir);
}

// Main
int main(int argc, char *argv[])
{
    if (argc != 3) // argc == 3 means the user provided exactly two arguments, which is the correct usage.
    {
        printf("Enter only 2 arguments.");
        return EXIT_FAILURE;
    }

    // Open the output file for writing
    FILE *output_file = fopen(argv[2], "w");
    if (output_file == NULL) // If it fails to open the output file, print error.
    {
        perror("Failed to open output file");
        return EXIT_FAILURE;
    }
    // Reaching here means there are 2 correct arguments and it opened the output file correctly.

    writeFileSizes(argv[1], output_file);
    // Traverse the directory and save file sizes
    // In this function, we will open the directory recursively and close it at the end.

    // Close the output file
    fclose(output_file);

    printf("Completed");

    return 0;
}