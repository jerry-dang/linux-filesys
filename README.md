# linux-filesys-processes
This is README file.

The program tool to display the tables used by the OS to keep track of open files, assignation of File Descriptors and processes is the c file in this folder named showFDtables.c. In addition, there is also a makefile to make the compiling process easier.

Some useful libraries that I used were unistd.h, string.h, sys/types.h, and dirent.h. These libraries helped with the command line argument implementation, retrieving the FD tables information, reading that information and handling the results.

## How I Solved the Problem:

A useful function I discovered during this assignment is called snprintf(), where it lets you combine a string in a specific format. I found it really interesting because I never knew about this function and it helped me with parsing the actual PID value into the pathname when I'm trying to call opendir on a specific process' FD information.

## Overview of The Functions:

Each function is documented in the code file itself but here is a brief summary of each function. Function signature is also
in the documented comments in the code.

void countFormat():
    Formats the output space between the count line and the PID value.

void pidFormat():
    Formats the output space between the PID value and the FD value.

void fdFormat():
    Formats the output space between the FD value and the Filename.

void filenameFormat():
    Formats the output space between the Filename and the Inode.

void printComposite():
    Prints a composite view of the FD table.

void printVnodes():
    Prints a Vnodes view of the FD table.

void printSystemWide():
    Prints a system-wide view of the FD table.

void printPerProcess():
    Prints a per-process view of the FD table.

void display():
    Calls printComposite(), and/or printVnodes(), and/or printSystemWide(), and/or printPerProcess() depending on user flags and positional argument.

int main():
    Main program where display() is called to print the information to the screen. Command line arguments and user flags are managed here

## How to Run/Use My Program:

To use my program, the user can choose between a couple of options for the command line arguments.

NOTE: If NO flags are used, the program will display the composite view, sample as using --composite (specifcally stated in assignment handout).

User flags: - *"--per-process": if this feature flag is present, then the user will only be able to see the process FD 
            table, which includes only the PID and the FD.
            - *"--systemWide": if this feature flag is present, then the user will only be able to see thesystem-wide FD table, which includes the PID, PD, and the Filename.
            - *"--Vnodes": if this feature flag is present, then the user will only be able to see the Vnodes FD table, which includes only the FD and the Inode (file serial number).
            - *"--composite": if this feature flag is present, then the user will only be able to see the composed FD table, which includes a line counter, PID, FD, Filename, and the Inode (Note: this flag is switched on if all the other user flags are off - if no arguments are presented, the composed view will show)
            - "--threshold=X": if this feature flag is present, then the user will be able to filter the result depending on the X value given by the user, meaning that all FD values of over X will not be shown in the direct display but instead will be displayed in a format of "PID (FD)" in the section named "Offending processes:". Note that X must be a positive value. If no value is specified (i.e. "--threshold="), then the default value of X will be NULL, meaning the user will see the display results as if the --composite flag was called if no other arguments are present. This flag can be used with any other flag combination, including the positional argument that is discussed after this section.

IMPORTANT*: Even though these flags should be used individual (starred flags) since the assignment wants an implementation where if this flag is present, then only that set of information should be displayed. However, the assignment handout states that a combination of these flags can occur and the video demo showed as well, therefore, these flags can be combined in any number of ways possible. The format of the display will be similar to the example in the assignment. If all flags are present, the program will display in this order: `--per-process, --systemWide, --Vnodes, --composite`. If only some flags are present, the order still holds. Note that these can all be used as well with a positional argument <PID> that is discussed next.

Positional argument: -<PID>: 
    If an integer is present, it will be treated as a PID and all rules and implementation still apply from the previous user flags section, just instead of the system-wide processes, only the FD associated with this <PID> will be displayed. This argument can be placed anywhere in the command line arguments, meaning it does not have to be at the start or at the end. However, any command line arguments that does not being with '--' will be treated as a <PID>, therefore only one PID is allowed for the program to function as intended. All other arguments should have '--' at the beginning of the argument, that is the way I implemented this program.

Some examples:
- GOOD: `./showFDtables --composite 1239183 --per-process`
- BAD: `./showFDtables 21832 928139 --systemWide`

- GOOD: `./showFDtables --threshold=34 218372`
- BAD: `./showFDtables --threshold=-2 218372 90123`

- GOOD: `./showFDtables --Vnodes`
- BAD: `./showFDtables Vnodes`

To run the program, I would recomend either using the classic method, typing "gcc showFDtables.c" in the terminal which will compile the file and create an executable named "a.out" in the current directory. Then, `./a.out (optional)<PID> ["--per-process"] ["--systemWide"] ["--Vnodes"] ["--composite"] ["--threshold=X"]` where the arguments in brackets are technically options, it would be the user's choice for which one they would want to use depending on what they want to see.

A second way to run the program would be type "make showFDtables" while the makefile is in the same directory. This is the same as compiling the file manually using gcc but this is an automated way of doing that. Now instead of having to type "./a.out", we can used the more intuitive name `./showFDtables (optional)<PID> ["--per-process"] ["--systemWide"] ["--Vnodes"] ["--composite"] ["--threshold=X"]` with the same implications for the command line arguments.
