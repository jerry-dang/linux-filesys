#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <dirent.h>
#include <math.h>

#define STRING_CONSTANT 1024

/*******************
 * Function: countFormat()
 * Parameters: int count, int vnode
 * Returns: void (N/A)
 * 
 * This function adds/decreases whitespace between the line counter and 
 * the PID value depending on the value of count and vnode.
*******************/
void countFormat(int count, int vnode) {
    if (vnode == 0) {
        if (count > 9 && count < 100) {
            printf("      ");
        } else if (count > 99 && count < 1000) {
            printf("     ");
        } else if (count > 999 && count < 10000) {
            printf("    ");
        } else {
            printf("       ");
        }
    } else if (vnode == 1) {
        if (count > 9 && count < 100) {
            printf("        ");
        } else if (count > 99 && count < 1000) {
            printf("       ");
        } else if (count > 999 && count < 10000) {
            printf("      ");
        } else {
            printf("         ");
        }
    }
}


/*******************
 * Function: pidFormat()
 * Parameters: int pid
 * Returns: void (N/A)
 * 
 * This function adds/decreases whitespace between the pid value
 * and the FD value.
*******************/
void pidFormat(int pid) {
    if (pid > 9 && pid < 100) {
        printf("      ");
    } else if (pid > 99 && pid < 1000) {
        printf("     ");
    } else if (pid > 999 && pid < 10000) {
        printf("    ");
    } else if (pid > 9999 && pid < 100000) {
        printf("   ");
    } else if (pid > 99999 && pid < 1000000) {
        printf("  ");
    } else if (pid > 999999 && pid < 10000000) {
        printf(" ");
    } else {
        printf("       ");
    }
}


/*******************
 * Function: fdFormat()
 * Parameters: int fd, int fd_node
 * Returns: void (N/A)
 * 
 * This function adds/decreases whitespace between the fd value and 
 * the Filename depending on the values of fd and fd_node.
*******************/
void fdFormat(int fd, int fd_node) {
    if (fd_node == 0) {
        if (fd > 9 && fd < 100) {
            printf("      ");
        } else if (fd > 99 && fd < 1000) {
            printf("     ");
        } else if (fd > 999 && fd < 10000) {
            printf("    ");
        } else {
            printf("       ");
        }
    } else if (fd_node == 1) {
        if (fd > 9 && fd < 100) {
            printf("            ");
        } else if (fd > 99 && fd < 1000) {
            printf("           ");
        } else if (fd > 999 && fd < 10000) {
            printf("          ");
        } else {
            printf("             ");
        }
    }
}


/*******************
 * Function: filenameFormat()
 * Parameters: int strlen
 * Returns: void (N/A)
 * 
 * This function adds/decreases whitespace between the Filename and
 * the Inode number depending on the length of the Filename.
*******************/
void filenameFormat(int strlen) {
    if (strlen < 24) {
        int i=strlen;
        while (i < 24) {
            printf(" ");
            i++;
        }
    } else {
        printf("     ");
    }
}


/*******************
 * Function: printComposite(int pid, int size, int threshold)
 * Parameters: int pid, int size, int threshold
 * Returns: void (N/A)
 * 
 * This function prints a composite view of the FD table.
*******************/
void printComposite(int pid, int size, int threshold) {

    int vnode = 0; int fd_node = 0;

    int threshold_array[size][2];
    int index = 0;

    DIR *directory = opendir("/proc");  // all processes
    if (directory == NULL) {
        fprintf(stderr, "error opening /proc\n");
    }

    struct dirent *entry;

    printf("         PID     FD      Filename                Inode\n");
    printf("        ===============================================\n");

    if (pid == -1) {
        int count=0;
        while ((entry = readdir(directory)) != NULL) {
            int pid = atoi(entry->d_name); // get pid from FD

            char pathname[STRING_CONSTANT];
            // parse PID value into pathname 
            snprintf(pathname, sizeof(pathname), "/proc/%d/fd/", pid);

            DIR *fd_directory = opendir(pathname);
            if (fd_directory == NULL) {
                continue;
            }

            struct dirent *fd_entry;

            while ((fd_entry = readdir(fd_directory)) != NULL) {
                // getting rid of unwanted data
                if (strcmp(fd_entry->d_name, ".") != 0 && strcmp(fd_entry->d_name, "..") != 0) {
                    count++;
                    char fd_pathname[STRING_CONSTANT*2];
                    snprintf(fd_pathname, sizeof(fd_pathname), "%s%s", pathname, fd_entry->d_name);
                    char link_buff[STRING_CONSTANT];
                    // reading symbolic link
                    ssize_t link_value = readlink(fd_pathname, link_buff, sizeof(link_buff));
                    if (link_value == -1) {
                        fprintf(stderr, "error with readlink\n");
                    }
                    link_value = '\0';  // since readlink doesn't add a null terminator at the end
                    int fd_count = atoi(fd_entry->d_name);
                    if (threshold != -1) {
                        if (fd_count > threshold) {
                            threshold_array[index][0] = pid;
                            threshold_array[index][1] = fd_count;
                            index++;
                        } else {
                            printf(" %d", count);
                            countFormat(count, vnode);
                            printf("%d", pid);
                            pidFormat(pid);
                            printf("%s", fd_entry->d_name);
                            fdFormat(fd_count, fd_node);
                            printf("%s", link_buff);
                            filenameFormat(strlen(link_buff));
                            printf("%ld\n", fd_entry->d_ino);
                        }
                    } else {
                        printf(" %d", count);
                        countFormat(count, vnode);
                        printf("%d", pid);
                        pidFormat(pid);
                        printf("%s", fd_entry->d_name);
                        fdFormat(fd_count, fd_node);
                        printf("%s", link_buff);
                        filenameFormat(strlen(link_buff));
                        printf("%ld\n", fd_entry->d_ino);
                    }
                }
            }
            closedir(fd_directory);
        }
    } else {
        char pathname[STRING_CONSTANT];
        snprintf(pathname, sizeof(pathname), "/proc/%d/fd/", pid);
        
        DIR *fd_directory = opendir(pathname);
        if (fd_directory == NULL) {
            fprintf(stderr, "no such process id found\n");
        }

        struct dirent *fd_entry;
        while ((fd_entry = readdir(fd_directory)) != NULL) {
            if (strcmp(fd_entry->d_name, ".") != 0 && strcmp(fd_entry->d_name, "..") != 0) {
                char fd_pathname[STRING_CONSTANT*2];
                snprintf(fd_pathname, sizeof(fd_pathname), "%s%s", pathname, fd_entry->d_name);
                char link_buff[STRING_CONSTANT];
                ssize_t link_value = readlink(fd_pathname, link_buff, sizeof(link_buff));
                if (link_value == -1) {
                    fprintf(stderr, "error with readlink\n");
                }
                link_value = '\0';  // since readlink doesn't add a null terminator at the end
                int fd_count = atoi(fd_entry->d_name);
                if (threshold != -1) {
                    if (fd_count > threshold) {
                        threshold_array[index][0] = pid;
                        threshold_array[index][1] = fd_count;
                        index++;
                    } else {
                        printf("         %d", pid);
                        pidFormat(pid);
                        printf("%s", fd_entry->d_name);
                        fdFormat(fd_count, fd_node);
                        printf("%s", link_buff);
                        filenameFormat(strlen(link_buff));
                        printf("%ld\n", fd_entry->d_ino);
                    }
                } else {
                    printf("         %d", pid);
                    pidFormat(pid);
                    printf("%s", fd_entry->d_name);
                    fdFormat(fd_count, fd_node);
                    printf("%s", link_buff);
                    filenameFormat(strlen(link_buff));
                    printf("%ld\n", fd_entry->d_ino);
                }
            }
        }
        closedir(fd_directory);
    }
   
    printf("        ===============================================\n");
    closedir(directory);

    if (threshold != -1) { 
        printf("## Offending processes:\n");
        for (int i=0; i<size; i++) {
            printf("%d (%d)", threshold_array[i][0], threshold_array[i][1]);
            if (i != size-2) {
                printf(", ");
            }
        }
        printf("\n");
    }
    
    printf("\n");
}


/*******************
 * Function: printVnodes(int pid, int size, int threshold)
 * Parameters: int pid, int size, int threshold
 * Returns: void (N/A)
 * 
 * This function prints only a Vnodes view of the FD table.
*******************/
void printVnodes(int pid, int size, int threshold) {

    int vnode = 1; int fd_node = 1;

    int threshold_array[size][2];
    int index = 0;

    DIR *directory = opendir("/proc");
    if (directory == NULL) {
        fprintf(stderr, "error opening /proc\n");
    }

    struct dirent *entry;

    printf("           FD            Inode\n");
    printf("        ========================================\n");

    if (pid == -1) {
        int count=0;
        while ((entry = readdir(directory)) != NULL) {
            int pid = atoi(entry->d_name);

            char pathname[STRING_CONSTANT];
            snprintf(pathname, sizeof(pathname), "/proc/%d/fd/", pid);

            DIR *fd_directory = opendir(pathname);
            if (fd_directory == NULL) {
                continue;
            }

            struct dirent *fd_entry;
            while ((fd_entry = readdir(fd_directory)) != NULL) {
                if (strcmp(fd_entry->d_name, ".") != 0 && strcmp(fd_entry->d_name, "..") != 0) {
                    count++;
                    char fd_pathname[STRING_CONSTANT*2];
                    // parsing pathname for FD
                    snprintf(fd_pathname, sizeof(fd_pathname), "%s%s", pathname, fd_entry->d_name);
                    char link_buff[STRING_CONSTANT];
                    ssize_t link_value = readlink(fd_pathname, link_buff, sizeof(link_buff));
                    if (link_value == -1) {
                        fprintf(stderr, "error with readlink\n");
                    }
                    link_value = '\0';  // since readlink doesn't add a null terminator at the end
                    int fd_count = atoi(fd_entry->d_name);
                    if (threshold != -1) {
                        if (fd_count > threshold) {
                            threshold_array[index][0] = pid;
                            threshold_array[index][1] = fd_count;
                            index++;
                        } else {
                            printf(" %d", count);
                            countFormat(count, vnode);
                            printf("%s", fd_entry->d_name);
                            fdFormat(fd_count, fd_node);
                            printf("%ld\n", fd_entry->d_ino);
                        }
                    } else {
                        printf(" %d", count);
                        countFormat(count, vnode);
                        printf("%s", fd_entry->d_name);
                        fdFormat(fd_count, fd_node);
                        printf("%ld\n", fd_entry->d_ino);
                    }
                }
            }
            closedir(fd_directory);
        }
    } else {
        char pathname[STRING_CONSTANT];
        snprintf(pathname, sizeof(pathname), "/proc/%d/fd/", pid);
        
        DIR *fd_directory = opendir(pathname);
        if (fd_directory == NULL) {
            fprintf(stderr, "no such process id found\n");
        }

        struct dirent *fd_entry;
        while ((fd_entry = readdir(fd_directory)) != NULL) {
            if (strcmp(fd_entry->d_name, ".") != 0 && strcmp(fd_entry->d_name, "..") != 0) {
                char fd_pathname[STRING_CONSTANT*2];
                snprintf(fd_pathname, sizeof(fd_pathname), "%s%s", pathname, fd_entry->d_name);
                char link_buff[STRING_CONSTANT];
                ssize_t link_value = readlink(fd_pathname, link_buff, sizeof(link_buff));
                if (link_value == -1) {
                    fprintf(stderr, "error with readlink\n");
                }
                link_value = '\0';  // since readlink doesn't add a null terminator at the end
                int fd_count = atoi(fd_entry->d_name);
                if (threshold != -1) {
                    if (fd_count > threshold) {
                        threshold_array[index][0] = pid;
                        threshold_array[index][1] = fd_count;
                        index++;
                    } else {
                        printf("           %s", fd_entry->d_name);
                        fdFormat(fd_count, fd_node);
                        printf("%ld\n", fd_entry->d_ino);
                    }
                } else {
                    printf("           %s", fd_entry->d_name);
                    fdFormat(fd_count, fd_node);
                    printf("%ld\n", fd_entry->d_ino);
                }
            }
        }
        closedir(fd_directory);
    }
   
    printf("        ========================================\n");
    closedir(directory);

    if (threshold != -1) {
        printf("## Offending processes:\n");
        for (int i=0; i<size; i++) {
            printf("%d (%d)", threshold_array[i][0], threshold_array[i][1]);
            if (i != size-2) {
                printf(", ");
            }
        }
        printf("\n");
    }

    printf("\n");
}


/*******************
 * Function: printSystemWide(int pid, int size, int threshold)
 * Parameters: int pid, int size, int threshold
 * Returns: void (N/A)
 * 
 * This function prints a system-wide view of the FD table.
*******************/
void printSystemWide(int pid, int size, int threshold) {
        
    int vnode = 0; int fd_node = 0;

    int threshold_array[size][2];
    int index = 0;

    DIR *directory = opendir("/proc");
    if (directory == NULL) {
        fprintf(stderr, "error opening /proc\n");
    }

    struct dirent *entry;

    printf("         PID     FD      Filename \n");
    printf("        ========================================\n");

    if (pid == -1) {
        int count=0;
        while ((entry = readdir(directory)) != NULL) {
            int pid = atoi(entry->d_name);

            char pathname[STRING_CONSTANT];
            snprintf(pathname, sizeof(pathname), "/proc/%d/fd/", pid);

            DIR *fd_directory = opendir(pathname);
            if (fd_directory == NULL) {
                continue;
            }

            struct dirent *fd_entry;
            while ((fd_entry = readdir(fd_directory)) != NULL) {
                if (strcmp(fd_entry->d_name, ".") != 0 && strcmp(fd_entry->d_name, "..") != 0) {
                    count++;
                    char fd_pathname[STRING_CONSTANT*2];
                    snprintf(fd_pathname, sizeof(fd_pathname), "%s%s", pathname, fd_entry->d_name);
                    char link_buff[STRING_CONSTANT];
                    ssize_t link_value = readlink(fd_pathname, link_buff, sizeof(link_buff));
                    if (link_value == -1) {
                        fprintf(stderr, "error with readlink\n");
                    }
                    link_value = '\0';  // since readlink doesn't add a null terminator at the end
                    int fd_count = atoi(fd_entry->d_name);
                    if (threshold != -1) {
                        if (fd_count > threshold) {
                            threshold_array[index][0] = pid;
                            threshold_array[index][1] = fd_count;
                            index++;
                        } else {
                            printf(" %d", count);
                            countFormat(count, vnode);
                            printf("%d", pid);
                            pidFormat(pid);
                            printf("%s", fd_entry->d_name);
                            fdFormat(fd_count, fd_node);
                            printf("%s\n", link_buff);
                        }
                    } else {
                        printf(" %d", count);
                        countFormat(count, vnode);
                        printf("%d", pid);
                        pidFormat(pid);
                        printf("%s", fd_entry->d_name);
                        fdFormat(fd_count, fd_node);
                        printf("%s\n", link_buff);
                    }
                }
            }
            closedir(fd_directory);
        }
    } else {
        char pathname[STRING_CONSTANT];
        snprintf(pathname, sizeof(pathname), "/proc/%d/fd/", pid);
        
        DIR *fd_directory = opendir(pathname);
        if (fd_directory == NULL) {
            fprintf(stderr, "no such process id found\n");
        }

        struct dirent *fd_entry;
        while ((fd_entry = readdir(fd_directory)) != NULL) {
            if (strcmp(fd_entry->d_name, ".") != 0 && strcmp(fd_entry->d_name, "..") != 0) {
                char fd_pathname[STRING_CONSTANT*2];
                snprintf(fd_pathname, sizeof(fd_pathname), "%s%s", pathname, fd_entry->d_name);
                char link_buff[STRING_CONSTANT];
                ssize_t link_value = readlink(fd_pathname, link_buff, sizeof(link_buff));
                if (link_value == -1) {
                    fprintf(stderr, "error with readlink\n");
                }
                link_value = '\0';  // since readlink doesn't add a null terminator at the end
                int fd_count = atoi(fd_entry->d_name);
                if (threshold != -1) {
                    if (fd_count > threshold) {
                        threshold_array[index][0] = pid;
                        threshold_array[index][1] = fd_count;
                        index++;
                    } else {
                        printf("         %d", pid);
                        pidFormat(pid);
                        printf("%s", fd_entry->d_name);
                        fdFormat(fd_count, fd_node);
                        printf("%s\n", link_buff);
                    }
                } else {
                    printf("         %d", pid);
                    pidFormat(pid);
                    printf("%s", fd_entry->d_name);
                    fdFormat(fd_count, fd_node);
                    printf("%s\n", link_buff);
                }
            }
        }
        closedir(fd_directory);
    }
   
    printf("        ========================================\n");
    closedir(directory);

    if (threshold != -1) {
        printf("## Offending processes:\n");
        for (int i=0; i<size; i++) {
            printf("%d (%d)", threshold_array[i][0], threshold_array[i][1]);
            if (i != size-2) {
                printf(", ");
            }
        }
        printf("\n");
    }

    printf("\n");
}


/*******************
 * Function: printPerProcess(int pid, int size, int threshold)
 * Parameters: int pid, int size, int threshold
 * Returns: void (N/A)
 * 
 * This function prints a per-process view of the FD table.
*******************/
void printPerProcess(int pid, int size, int threshold) {

    int vnode = 0;

    int threshold_array[size][2];
    int index = 0;

    DIR *directory = opendir("/proc");
    if (directory == NULL) {
        fprintf(stderr, "error opening /proc\n");
    }

    struct dirent *entry;

    printf("         PID     FD\n");
    printf("        ============\n");

    if (pid == -1) {
        int count=0;
        while ((entry = readdir(directory)) != NULL) {
            int pid = atoi(entry->d_name);

            char pathname[STRING_CONSTANT];
            snprintf(pathname, sizeof(pathname), "/proc/%d/fd/", pid);

            DIR *fd_directory = opendir(pathname);
            if (fd_directory == NULL) {
                continue;
            }

            struct dirent *fd_entry;
            while ((fd_entry = readdir(fd_directory)) != NULL) {
                if (strcmp(fd_entry->d_name, ".") != 0 && strcmp(fd_entry->d_name, "..") != 0) {
                    count++;
                    char fd_pathname[STRING_CONSTANT*2];
                    snprintf(fd_pathname, sizeof(fd_pathname), "%s%s", pathname, fd_entry->d_name);
                    char link_buff[STRING_CONSTANT];
                    ssize_t link_value = readlink(fd_pathname, link_buff, sizeof(link_buff));
                    if (link_value == -1) {
                        fprintf(stderr, "error with readlink\n");
                    }
                    link_value = '\0';  // since readlink doesn't add a null terminator at the end
                    int fd_count = atoi(fd_entry->d_name);
                    if (threshold != -1) {
                        if (fd_count > threshold) {
                            threshold_array[index][0] = pid;
                            threshold_array[index][1] = fd_count;
                            index++;
                        } else {
                            printf(" %d", count);
                            countFormat(count, vnode);
                            printf("%d", pid);
                            pidFormat(pid);
                            printf("%s\n", fd_entry->d_name);
                        }
                    } else {
                        printf(" %d", count);
                        countFormat(count, vnode);
                        printf("%d", pid);
                        pidFormat(pid);
                        printf("%s\n", fd_entry->d_name);
                    }
                }
            }
            closedir(fd_directory);
        }
    } else {
        char pathname[STRING_CONSTANT];
        snprintf(pathname, sizeof(pathname), "/proc/%d/fd/", pid);
        
        DIR *fd_directory = opendir(pathname);
        if (fd_directory == NULL) {
            fprintf(stderr, "no such process id found\n");
        }

        struct dirent *fd_entry;
        while ((fd_entry = readdir(fd_directory)) != NULL) {
            if (strcmp(fd_entry->d_name, ".") != 0 && strcmp(fd_entry->d_name, "..") != 0) {
                char fd_pathname[STRING_CONSTANT*2];
                snprintf(fd_pathname, sizeof(fd_pathname), "%s%s", pathname, fd_entry->d_name);
                char link_buff[STRING_CONSTANT];
                ssize_t link_value = readlink(fd_pathname, link_buff, sizeof(link_buff));
                if (link_value == -1) {
                    fprintf(stderr, "error with readlink\n");
                }
                link_value = '\0';  // since readlink doesn't add a null terminator at the end
                int fd_count = atoi(fd_entry->d_name);
                if (threshold != -1) {
                    if (fd_count > threshold) {
                        threshold_array[index][0] = pid;
                        threshold_array[index][1] = fd_count;
                        index++;
                    } else {
                        printf("         %d", pid);
                        pidFormat(pid);
                        printf("%s\n", fd_entry->d_name);
                    }
                } else {
                    printf("         %d", pid);
                    pidFormat(pid);
                    printf("%s\n", fd_entry->d_name);
                }
            }
        }
        closedir(fd_directory);
    }
   
    printf("        ============\n");
    closedir(directory);

    if (threshold != -1) {
        printf("## Offending processes:\n");
        for (int i=0; i<size; i++) {
            printf("%d (%d)", threshold_array[i][0], threshold_array[i][1]);
            if (i != size-2) {
                printf(", ");
            }
        }
        printf("\n");
    }

    printf("\n");
}


/*******************
 * Function: display(int pid, int *perProcessF, int *systemWideF, int*vnodesF, int *compositeF, int *threshold)
 * Parameters: int pid, int *perProcessF, int *systemWideF, int *vnodesF, int *compositeF, int *threshold
 * Returns: void (N/A)
 * 
 * This function calls other print functions above depending on the paraments pid, *perProcessF, *systemWideF
 * , *vnodesF, *compositeF, and *threshold. If pid == -1, systemw-wide process FD table will be displayed 
 * by calling printPerProcess() if *perProcessF == 1 (feature flag on) and etc. for all the other flags. If 
 * all flags except for *threshold which is not included, are off, then compositeF is on and printComposite()
 * will be called. If threshold is not -1, means that user has specified a threshold for FD values, then 
 * depending on the other flags, the output will filter out the FD values over that threshold and will print 
 * them after the main output, in a section named "Offending processes: <PID> (FD)" in that format.
*******************/
void display(int pid, int *perProcessF, int *systemWideF, int *vnodesF, int *compositeF, int *threshold) {

    int array_size = -1;
    if (*threshold != -1 && pid == -1) {
        array_size = 0;
        DIR *directory = opendir("/proc");
        if (directory == NULL) {
            fprintf(stderr, "error opening /proc\n");
        }
        struct dirent *entry;

        while ((entry = readdir(directory)) != NULL) {
            int pid = atoi(entry->d_name);

            char pathname[STRING_CONSTANT];
            snprintf(pathname, sizeof(pathname), "/proc/%d/fd/", pid);

            DIR *fd_directory = opendir(pathname);
            if (fd_directory == NULL) {
                continue;
            }

            struct dirent *fd_entry;
            while ((fd_entry = readdir(fd_directory)) != NULL) {
                if (strcmp(fd_entry->d_name, ".") != 0 && strcmp(fd_entry->d_name, "..") != 0) {
                    char fd_pathname[STRING_CONSTANT*2];
                    snprintf(fd_pathname, sizeof(fd_pathname), "%s%s", pathname, fd_entry->d_name);
                    char link_buff[STRING_CONSTANT];
                    ssize_t link_value = readlink(fd_pathname, link_buff, sizeof(link_buff));
                    if (link_value == -1) {
                        fprintf(stderr, "error with readlink\n");
                    }
                    link_value = '\0';  // since readlink doesn't add a null terminator at the end
                    int fd_count = atoi(fd_entry->d_name);
                    if (fd_count > *threshold) {
                        array_size++;
                    }
                }
            }
            closedir(fd_directory);
        }
        closedir(directory);
    } else if (*threshold != -1 && pid != -1) {
        array_size = 0;

        char pathname[STRING_CONSTANT];
        snprintf(pathname, sizeof(pathname), "/proc/%d/fd/", pid);

        DIR *fd_directory = opendir(pathname);
        if (fd_directory == NULL) {
            fprintf(stderr, "error opening /proc\n");
        }
        struct dirent *fd_entry;

        while ((fd_entry = readdir(fd_directory)) != NULL) {
            if (strcmp(fd_entry->d_name, ".") != 0 && strcmp(fd_entry->d_name, "..") != 0) {
                char fd_pathname[STRING_CONSTANT*2];
                snprintf(fd_pathname, sizeof(fd_pathname), "%s%s", pathname, fd_entry->d_name);
                char link_buff[STRING_CONSTANT];
                ssize_t link_value = readlink(fd_pathname, link_buff, sizeof(link_buff));
                if (link_value == -1) {
                    fprintf(stderr, "error with readlink\n");
                }
                link_value = '\0';  // since readlink doesn't add a null terminator at the end
                int fd_count = atoi(fd_entry->d_name);
                if (fd_count > *threshold) {
                    array_size++;
                }
            }
        }    
        closedir(fd_directory);    
    }

    if (*perProcessF == 1) {
        printPerProcess(pid, array_size, *threshold);
    }

    if (*systemWideF == 1) {
        printSystemWide(pid, array_size, *threshold);
    }

    if (*vnodesF == 1) {
        printVnodes(pid, array_size, *threshold);
    }

    if ((*perProcessF == 0 && *systemWideF == 0 && *vnodesF == 0) || *compositeF == 1) { // composite flag on OR all other flags off)
        printComposite(pid, array_size, *threshold);
    }
}


/*******************
 * Function: main(int argc, char **argv)
 * Parameters: int argc, char **argv
 * Returns: void (N/A)
 * 
 * This function is that main function that handles the command line arguments given by the user and
 * the positional argument <PID>. This information is managed and sent the display() in order to 
 * display the user the correct information to the screen.
*******************/
int main(int argc, char **argv) {

    int perProcessF = 0, systemWideF = 0, vnodesF = 0, compositeF = 0;
    int pid = -1, default_threshold = -1;

    char *given_threshold;

    const char assign = '=';

    for (int i=1; i<argc; i++) {
        if (strncmp("--per-process", argv[i], 13) == 0) {
            perProcessF = 1;
        } else if (strncmp("--systemWide", argv[i], 12) == 0) {
            systemWideF = 1;
        } else if (strncmp("--Vnodes", argv[i], 8) == 0) {
            vnodesF = 1;
        } else if (strncmp("--composite", argv[i], 11) == 0) {
            compositeF = 1;
        } else if (strncmp("--", argv[i], 2) != 0) {
            pid = atoi(argv[i]);
        } else if (strncmp("--threshold", argv[i], 11) == 0) {
            given_threshold = strchr(argv[i], assign);
            if (strcmp(given_threshold, "=") == 0) {
                continue;
            } else {
                int digit;
                int j=1;
                while ((digit = atoi((given_threshold+j))) != 0) {
                    default_threshold = default_threshold + digit;
                    j++;
                }
                default_threshold++;
            }
        }
    }

    if (argc == 1) {
        compositeF = 1;
    }

    display(pid, &perProcessF, &systemWideF, &vnodesF, &compositeF, &default_threshold);

    return 0;
}
