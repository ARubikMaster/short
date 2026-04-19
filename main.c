#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<linux/limits.h> // if u dont use linux thats your problem
#include<stdbool.h>
#include<libgen.h>

int main(int argc, char *argv[]) {
    char filename[100] = "short.conf";
    bool modified = false;
    bool silent = false;
    bool here = false;

    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        if (argc > 1) {
            int i = 1;

            while (i < argc) {
                if (argv[i][0] != '-') {
                    if (!modified) {
                        strcpy(filename, argv[1]);
                        modified = true;
                    } else {
                        printf("ERROR: Filename is already set, defined a second time: %s\n", argv[i]);
                        return 1;
                    }
                } else {
                    if (strcmp(argv[i], "-s") == 0 || strcmp(argv[i], "--silent") == 0) {
                       silent = true; 
                    } else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--here") == 0) {
                       here = true; 
                    } else {
                        printf("ERROR: Unknown paramater: %s\n", argv[i]);
                        return 1;
                    }
                }

                i++;
            }
        }

        if (access(strcat(strcat(cwd, "/"), filename), F_OK) == 0 || access(filename, F_OK) == 0) {
            char command[1024]; // max characters
            int i = 0;

            // reading file content
            char ch;
            FILE *fptr;
            if (filename[0] == '/' || filename[0] == '~') {
                fptr = fopen(filename, "r");
            } else {
                fptr = fopen(cwd, "r");
            }

            bool inComment = false;
            char lastCh = 'H';

            while ((ch = fgetc(fptr)) != EOF && i < 1024) {
                if (ch != '\n' && ch != '\r') {
                    if (ch == '/' && lastCh == '/') {
                        inComment = true;
                        i--; // removes the last char that was the first of the two "//"
                    }
                    if (!inComment) {
                        command[i] = ch;
                        i++;
                    }
                } else if (ch == '\n') {
                    inComment = false;
                    if (command[i-1] != ' ') {
                        command[i] = ' ';
                        i++;
                    }
                }

                lastCh = ch;
            }

            command[i] = '\0'; // terminate the string
            fclose(fptr);

            if (!silent) printf("Running command: %s\n", command);

            int result;
            
            if ((filename[0] == '/' || filename[0] == '~') && !here) {
                char pathCopy[1024];
                strncpy(pathCopy, command, sizeof(pathCopy));

                char *dir;
                dir = dirname(filename);

                char fullCmd[2048];
                int len = snprintf(fullCmd, sizeof(fullCmd), "cd %s && %s", dir, command);
                
                result = system(fullCmd);
            } else {
                result = system(command);
            }

            if (result != 0 && !silent) {
                printf("Ended with exit code %d\n", result);
            }
        } else {
            printf("ERROR: %s does not exist.\n", filename[0] == '/' || filename[0] == '~' ? filename : cwd); // already includes /build.gcc, wtf?!?!
            return 1;
        }
    } else {
        perror("Error getting current working directory\n");
        return 1;
    }
    
    return 0;
}
