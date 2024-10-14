/*
We want to change the options (a, b, c) to receive integer arguments and print the sum at the end.
*/
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h> // needed for atoi function
#include <unistd.h>

int main (int argc, char **argv)
{
    int aflag = 0;
    int bflag = 0;
    int cflag = 0;
    int index;
    int c;

    opterr = 0;

    while ((c = getopt (argc, argv, "a:b:c:")) != -1) // 'a:b:c:' means all 3 options expect arguments.
        switch (c)
        {
            case 'a':
                aflag = atoi(optarg);
                break;
            case 'b':
                bflag = atoi(optarg);
                break;
            case 'c':
                cflag = atoi(optarg);
                break;
            case '?':
                if (optopt == 'c')
                    fprintf (stderr, "Option -%c requires an argument.\n", optopt);
                else if (isprint (optopt))
                    fprintf (stderr, "Unknown option `-%c'.\n", optopt);
                else
                    fprintf (stderr,
                             "Unknown option character `\\x%x'.\n",
                             optopt);
                return 1;
            default:
                abort ();
        }
    int sum = aflag+bflag+cflag;
    printf ("aflag = %d, bflag = %d, cflag = %d, sum = %d.\n",
            aflag, bflag, cflag, sum);

    for (index = optind; index < argc; index++)
        printf ("Non-option argument %s\n", argv[index]);
    return 0;
}//
// Created by heran on 14/10/2024.
//
