#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/wait.h>
#include "auebsh.h"
 
int handler(int id, char *string)
{	
	int cpipe = 1;	 //Αρχικοποίηση αριθμού εντολών.
	char string2[255];
	char *token0;
	token0 = strtok(string,"\n");
	int i, j;
	int check = 1;	//Μεταβλητή ελέγχου σωστού φλοιού.

	if (strstr(token0, "-") != NULL){
		check = 3;
	}

	if (strstr(token0, "|") != NULL) {	 //Αν υπάρχει σωλήνωση μέτρα πόσες εντολές πρέπει να εκτελεστούν. 
		check = 4;
		for (i= 0; token0[i] != '\0' ; i++) {
			if (token0[i] == '|') cpipe++;
		}
	}

	if (cpipe >2) {
		check = 5;
	}

	char *piprix[cpipe];	//Πίνακας εντολών προς εκτέλεση.
	token0 = strtok(token0, "|");
	i = 0;
	while (token0 != NULL){
		piprix[i] = token0;
		token0 = strtok(NULL,"|");
		i++;
	} 
	
	int   pipes[2];
	pid_t pid;
	int   filed_in = 0;	//File descriptor.
	int status;
	for (i = 0; i < cpipe; i++) {	
		strcpy(string2,"\0");
		strcpy(string2, piprix[i]);
		status = pipe(pipes);	  //Αρχικοποίηση pipes.
		if (status == -1) {    //Έλεγχος για τα pipes.
			printf("ERROR: Pipe command failed.\n");
			return -1;
		}
		if ((pid = fork()) == -1)    //Έλεγχος για την fork.
		{
			exit(EXIT_FAILURE);
		}
		else if (pid == 0) {
			dup2(filed_in, 0);	 //Αντικατέστησε την είσοδο με την έξοδο της προηγούμενης εντολής.
			if (i < (cpipe-1))  dup2(pipes[1], 1);	//Αν υπάρχει επόμενη εντολή, αντικατέστησε την έξοδο του προγράμματος με την pipe[1]. 
			close(pipes[0]);	
				
			char *token;
			token = strtok(piprix[i]," ");
			int counter = 0;
			int in, out;
			while (token!=NULL){	//Μέτρημα ορισμάτων της εντολής και έλεγχος για ανακατεύθυνση.
				if (strstr(token, "<")!=NULL){
					if (check <2) {
						check = 2;
					}
					token = strtok(NULL," ");
					in = open (token, O_RDONLY);
					dup2(in,0);	//Ανακατεύθυνση εισόδου.
					token = strtok(NULL," ");
				}else if ((strstr(token, ">")!=NULL)){
					if (strstr(token, ">>")){
						check = 6;
					}
					if (check <2) {
						check = 2;
					}
					token = strtok(NULL," ");
					remove(token);
					if (id >= check){ 
						out = open (token, O_WRONLY|O_CREAT,S_IRWXU);
						dup2(out,1);	//Ανακατεύθυνση εξόδου.	
					}
					token = strtok(NULL," ");
				}else{	
					token = strtok(NULL," ");
					counter++;
				}
			}//while
			
			char *matrix[counter+1];	//Πίνακας ορισμάτων εντολής.
			char *tokenew = strtok(string2," ");
		
			int i = 0;
			while (tokenew!=NULL){	     //Γέμισμα πίνακα ορισμάτων.
				if  (strstr(tokenew, "<")!=NULL || strstr(tokenew, ">")!=NULL ){
					tokenew = strtok(NULL," ");
					tokenew = strtok(NULL," ");
				}else{
					matrix[i] = tokenew;
					tokenew = strtok(NULL," ");
					i++;
				}
			}
			matrix[counter] = (char*)0;

			if (id >= check) {	//Έλεγχος εγκυρότητας φλοιού.
				int ret;	
				ret = execvp(matrix[0], matrix);	  //Εκτέλεση εντολής.
			}else{
				printf("Wrong input. Try again.\n");
			}

		}else{
			wait(NULL);
			close(pipes[1]);
			filed_in = pipes[0];	//Αποθήκευση εισόδου για την επόμενη εντολή.
		}

	}//for 

	return 0;

}//handler