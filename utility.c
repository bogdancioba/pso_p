#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <utime.h>
#include <time.h>

void _ls(const char *dir, int op_a, int op_l)
{
    // Here we will list the directory
    struct dirent *d;
    DIR *dh = opendir(dir);
    if (!dh)
    {
        if (errno = ENOENT)
        {
            // If the directory is not found
            perror("Directory doesn't exist");
        }
        else
        {
            // If the directory is not readable then throw error and exit
            perror("Unable to read directory");
        }
        exit(EXIT_FAILURE);
    }
    // While the next entry is not readable we will print directory files
    while ((d = readdir(dh)) != NULL)
    {
        // If hidden files are found we continue
        if (!op_a && d->d_name[0] == '.')
            continue;
        printf("%s  ", d->d_name);
        if (op_l)
            printf("\n");
    }
    if (!op_l)
        printf("\n");
}

int touch(const char *pathname)
{
    // Open or create the file
    int fd = open(pathname, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    if (fd < 0)
    {
        perror("Error opening file");
        return 1;
    }

    // Update the file's timestamp
    struct utimbuf times = {.actime = time(NULL), .modtime = time(NULL)};
    if (utime(pathname, &times) != 0)
    {
        perror("Error updating timestamp");
        return 1;
    }

    // Close the file
    if (close(fd) != 0)
    {
        perror("Error closing file");
        return 1;
    }

    return 0;
}

int cat(const char *pathname)
{
    // Open the file
    FILE *f = fopen(pathname, "r");
    if (f == NULL)
    {
        perror("Error opening file");
        return 1;
    }

    // Read the file and display its contents to the screen
    char c;
    while ((c = fgetc(f)) != EOF)
    {
        fputc(c, stdout);
    }

    // Close the file
    fclose(f);

    return 0;
}



int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Usage: busybox [utility] [arguments]\n");
        return 1;
    }

    char *utility = argv[1];

    // ECHO
    if (strcmp(utility, "echo") == 0)
    {
        for (int i = 2; i < argc; i++)
        {
            printf("%s ", argv[i]);
        }
        printf("\n");
    }

    if (strcmp(utility, "hello") == 0)
    {
        time_t t;   // not a primitive datatype
        time(&t);
        printf("\nThis program has been writeen at (date and time): %s", ctime(&t));
    }

    // PWD
    else if (strcmp(utility, "pwd") == 0)
    {
        char cwd[1024];
        if (getcwd(cwd, sizeof(cwd)) != NULL)
        {
            printf("%s\n", cwd);
        }
    }

    // MKDIR
    else if (strcmp(utility, "mkdir") == 0)
    {
        if (mkdir(argv[2], 0777) == -1)
            perror("Error: No name given for the directory.\n");
        else
            printf("Directory created\n");
    }

    // LS
    else if (strcmp(utility, "ls") == 0)
    {
        if (argc == 2)
        {
            _ls(".", 0, 0);
        }
        else if (argc == 3)
        {
            if (argv[2][0] == '-')
            {
                // Checking if option is passed
                // Options supporting: a, l
                int op_a = 0, op_l = 0;
                char *p = (char *)(argv[2] + 1);
                while (*p)
                {
                    if (*p == 'a')
                        op_a = 1;
                    else if (*p == 'l')
                        op_l = 1;
                    else
                    {
                        perror("Option not available");
                        exit(EXIT_FAILURE);
                    }
                    p++;
                }
                _ls(".", op_a, op_l);
            }
        }
    }

    // CD
    else if (strcmp(utility, "cd") == 0)
    {
        char s[100];
        printf("%s\n", getcwd(s, 100));
        chdir(argv[2]);
        printf("%s\n", getcwd(s, 100));
    }

    // RM
    else if (strcmp(utility, "rm") == 0)
    {
        const char *f = argv[2];

        if (remove(f) != 0)
        {
            perror("Error deleting file");
            return 1;
        }
    }

    // CP
    else if (strcmp(utility, "cp") == 0)
    {
        FILE *src = fopen(argv[2], "r");
        if (src == NULL)
        {
            perror("Error opening source file");
            return 1;
        }

        // Open the destination file
        FILE *dst = fopen(argv[3], "w");
        if (dst == NULL)
        {
            perror("Error opening destination file");
            return 1;
        }

        // Copy the contents of the source file to the destination file
        char c;
        while ((c = fgetc(src)) != EOF)
        {
            fputc(c, dst);
        }

        // Close the source and destination files
        fclose(src);
        fclose(dst);
    }

    // TOUCH
    else if (strcmp(utility, "touch") == 0)
    {
        if (touch(argv[2]) != 0)
        {
            fprintf(stderr, "Error touching file\n");
            return 1;
        }
    }

    // CLEAR
    else if (strcmp(utility, "clear") == 0)
    {
        if (system("clear") != 0)
        {
            perror("Error clearing the screen");
            return 1;
        }
    }

    // CAT
    else if (strcmp(utility, "cat") == 0)
    {
        for (int i = 2; i < argc; i++)
        {
            if (cat(argv[2]) != 0)
            {
                fprintf(stderr, "Error concatenating file\n");
                return 1;
            }
        }
    }

    // UNKNOWN
    else
    {
        printf("Unknown utility: %s\n", utility);
    }

    return 0;
}