# Self-Made Shell

**Objective**:  
Implementing a shell that imitates the Linux shell. More specifically, the program is capable of repeating the following steps:  
1. prompt
2. command line reading from the terminal and 
3. creating appropriate processes to execute the command line<br/><br/>

**Implementation**:
* auebsh1: This shell accepts simple individual commands. Specifically: a) it displays its name and the character ">" as a prompt, b) it reads the name of a program from the terminal, c) it creates a new process to execute the program which is read and d) it waits for the new process to end. Once the new process is terminated, shell will read and execute the next command.
* auebsh2: This shell extends shell auebsh1 but also accepts redirect to standard I/O from and to files for the commands it executes.
* auebsh3: This shell extends shell auebsh2 but also accepts parameters for the commands it executes.
* auebsh4: This shell extends shell auebsh3 but also executes command sequences with a single pipeline.
* auebsh5: This shell extends shell auebsh4 but also executes command sequences without limiting the number of pipelines.
