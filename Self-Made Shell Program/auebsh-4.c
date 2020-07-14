#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include "auebsh.h"

int main() {
    int id = 4; //Αναφέρεται στον τέταρτο φλοιό.
    int done = 0;

    while (done == 0) {
        printf("auebsh4>"); //Εμφανίζει το όνομα του φλοιού ως προτροπή.
        char string[255];
        fgets(string, 255, stdin); //Ανάγνωση γραμμής εντολών από το τερματικό.

        if (sizeof(string) > 255) { //Δεν θα τρέξει για string μεγέθους > 255.
        } else if (feof(stdin)) { //Έλεγχος για end of file.
            done = 1;
        } else {
            pid_t pid, waitPid; //Ορισμός μεταβλητών της fork.
            int status;
            pid = fork(); //Σπάει το πρόγραμμα σε δυο προγράμματα.

            if (pid < 0) { //Έλεγχος για την fork.
                return -1; //Η fork απέτυχε.
            }

            if (pid == 0) { //Διεργασία παιδιού.
                done = handler(id, string); //Εκτελεί την handler για id = 4. 
                return 0; //Το παιδί σταματάει να εκτελείται.
            } else { //Διεργασία πατέρας.
                waitPid = wait( & status); //Περιμένει τον τερματισμό του παιδιού.
                if (waitPid == -1) {
                    return -1; //Το Waitpid απέτυχε.
                }
            }

        }

    } //while
    printf("\n");
    return 0;

} //main
