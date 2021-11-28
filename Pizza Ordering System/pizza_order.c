/* Author: Stergina */

#include "pizza_order.h"

/* Declares all static variables */
int Ncook = 2; //Number of cooks
int Noven = 5; //Number of ovens
int Ndeliverer = 10; //Number of deliverers
int Torderlow = 1; //Mimumum time of next pizza order
int Torderhigh = 5; //Maximum time of next pizza order
int Norderlow = 1; //Minimum number of pizzas per order
int Norderhigh = 5; //Maximum number of pizzar per order
int Tlow = 5; //Minimum time of delivering an order
int Thigh = 15; //Maximum time of delivering an order
int Tprep = 1; //Time of pizza preparation
int Tbake = 10; //Time of pizza baking

/* Declares variables */
int Ncust; //Number of customers/orders
int * id; //Array to store orders
int * tot_time_arr; //Array to store the delivery time of each order
int * cool_time_arr; //Array to store the cooling time of each order

/* Declares mutexes, conditional variables and the random seed */
pthread_mutex_t lock_cook; //Mutex for available cooks
pthread_mutex_t lock_oven; //Mutex for available ovens
pthread_mutex_t lock_dlvr; //Mutex for available deliverers
pthread_mutex_t lock_time_cook; //Mutex for time until an available cook is found
pthread_mutex_t lock_time_oven; //Mutex for time until an available oven is found
pthread_mutex_t lock_time_dlvr; //Mutex for time until an available deliverer is found
pthread_mutex_t lock_print; //Mutex for printing message
pthread_cond_t cond_cooks; //Conditional variable to synchronize cooks with orders
pthread_cond_t cond_ovens; //Conditional variable to synchronize ovens with orders
pthread_cond_t cond_dlvr; //Conditional variable to synchronize deliverers with orders
unsigned int seed; //Seed for random numbers

/** Function for pizza order */
void * order(void * x) {
    /* Return value and order number */
    int rc;
    int id = * (int * ) x;

    /* Declares time variables */
    struct timespec start, stop;
    long wait_cook, wait_oven, wait_dlvr;

    /* Allocates a random number of pizzas per order.
     * The number of pizzas is between 1 and 5! */
    int pieces = rand_r( & seed) % (Norderhigh + 1 - Norderlow) + Norderlow;

    /* Locks time mutex for cooks and checks if it has been locked succesfully */
    rc = pthread_mutex_lock( & lock_time_cook);
    if (rc != 0) {
        printf("ERROR: return code from pthread_mutex_lock() is %d\n", rc);
        pthread_exit( & rc);
    }

    /* Starts timer until an available cook is found */
    clock_gettime(CLOCK_REALTIME, & start);

    /* Locks cook mutex and checks if it has been locked succesfully */
    rc = pthread_mutex_lock( & lock_cook);
    if (rc != 0) {
        printf("ERROR: return code from pthread_mutex_lock() is %d\n", rc);
        pthread_exit( & rc);
    }

    /* While loop in order to check the availability of a cook */
    while (Ncook == 0) {
        rc = pthread_cond_wait( & cond_cooks, & lock_cook);
        if (rc != 0) {
            printf("ERROR: return code from pthread_cond_wait() is %d\n", rc);
            pthread_exit( & rc);
        }
    }

    /* Decreases number of available cooks by one, since the cook is not available for other orders */
    Ncook--;

    /* Unlocks cook mutex and checks if it has been unlocked succesfully */
    rc = pthread_mutex_unlock( & lock_cook);
    if (rc != 0) {
        printf("ERROR: return code from pthread_mutex_unlock() is %d\n", rc);
        pthread_exit( & rc);
    }

    /* Stops timer since an available cook was found */
    clock_gettime(CLOCK_REALTIME, & stop);

    /* Unlocks time mutex for cooks and checks if it has been unlocked succesfully*/
    rc = pthread_mutex_unlock( & lock_time_cook);
    if (rc != 0) {
        printf("ERROR: return code from pthread_mutex_unlock() is %d\n", rc);
        pthread_exit( & rc);
    }

    /* Calculates waiting time till an available cook was found */
    wait_cook = stop.tv_sec - start.tv_sec;

    /* Calculates the preparation time needed for an order before baking */
    int time_prep = pieces * Tprep;

    /* Gives the needed time to a cook in order to prepare the order */
    sleep(time_prep);

    /* Locks time mutex for ovens and checks if it has been locked succesfully */
    rc = pthread_mutex_lock( & lock_time_oven);
    if (rc != 0) {
        printf("ERROR: return code from pthread_mutex_lock() is %d\n", rc);
        pthread_exit( & rc);
    }

    /* Starts timer until an available oven is found */
    clock_gettime(CLOCK_REALTIME, & start);

    /* Locks oven mutex and checks if it has been locked succesfully */
    rc = pthread_mutex_lock( & lock_oven);
    if (rc != 0) {
        printf("ERROR: return code from pthread_mutex_lock() is %d\n", rc);
        pthread_exit( & rc);
    }

    /* While loop in order to check the availability of an oven */
    while (Noven == 0) {
        rc = pthread_cond_wait( & cond_ovens, & lock_oven);
        if (rc != 0) {
            printf("ERROR: return code from pthread_cond_wait() is %d\n", rc);
            pthread_exit( & rc);
        }
    }

    /* Decreases number of available ovens by one, since the oven is not available for other orders */
    Noven--;

    /* Unlocks oven mutex and checks if it has been unlocked succesfully */
    rc = pthread_mutex_unlock( & lock_oven);
    if (rc != 0) {
        printf("ERROR: return code from pthread_mutex_unlock() is %d\n", rc);
        pthread_exit( & rc);
    }

    /* Stops timer since an available oven was found */
    clock_gettime(CLOCK_REALTIME, & stop);

    /* Unlocks time mutex for ovens and checks if it has been unlocked succesfully */
    rc = pthread_mutex_unlock( & lock_time_oven);
    if (rc != 0) {
        printf("ERROR: return code from pthread_mutex_unlock() is %d\n", rc);
        pthread_exit( & rc);
    }

    /* Calculates waiting time till an available oven was found */
    wait_oven = stop.tv_sec - start.tv_sec;

    /* Locks cook mutex and checks if it has been locked succesfully */
    rc = pthread_mutex_lock( & lock_cook);
    if (rc != 0) {
        printf("ERROR: return code from pthread_mutex_lock() is %d\n", rc);
        pthread_exit( & rc);
    }

    /* Increases number of available cooks by one, since the cook is now available for other orders */
    Ncook++;

    /* Unblocks cook thread and checks if it has been unblocked succesfully */
    rc = pthread_cond_signal( & cond_cooks);
    if (rc != 0) {
        printf("ERROR: return code from pthread_cond_signal() is %d\n", rc);
        pthread_exit( & rc);
    }

    /* Unlocks cook mutex and checks if it has been unlocked succesfully */
    rc = pthread_mutex_unlock( & lock_cook);
    if (rc != 0) {
        printf("ERROR: return code from pthread_mutex_unlock() is %d\n", rc);
        pthread_exit( & rc);
    }

    /* Gives the needed time to the oven in order to bake an order */
    sleep(Tbake);

    /* Allocates a random delivery time.
     * The time is between 5 and 15 minutes! */
    int time_dlvr = rand_r( & seed) % (Thigh + 1 - Tlow) + Tlow;

    /* Locks time mutex for deliverers and checks if it has been locked succesfully */
    rc = pthread_mutex_lock( & lock_time_dlvr);
    if (rc != 0) {
        printf("ERROR: return code from pthread_mutex_lock() is %d\n", rc);
        pthread_exit( & rc);
    }

    /* Starts timer until an available deliverer is found */
    clock_gettime(CLOCK_REALTIME, & start);

    /* Locks deliverer mutex and checks if it has been locked succesfully */
    rc = pthread_mutex_lock( & lock_dlvr);
    if (rc != 0) {
        printf("ERROR: return code from pthread_mutex_lock() is %d\n", rc);
        pthread_exit( & rc);
    }

    /* While loop in order to check the availability of a deliverer */
    while (Ndeliverer == 0) {
        rc = pthread_cond_wait( & cond_dlvr, & lock_dlvr);
        if (rc != 0) {
            printf("ERROR: return code from pthread_cond_wait() is %d\n", rc);
            pthread_exit( & rc);
        }
    }

    /* Decreases number of available deliverers by one, since the deliverer is not available for other orders */
    Ndeliverer--;

    /* Unlocks deliverer mutex and checks if it has been unlocked succesfully */
    rc = pthread_mutex_unlock( & lock_dlvr);
    if (rc != 0) {
        printf("ERROR: return code from pthread_mutex_unlock() is %d\n", rc);
        pthread_exit( & rc);
    }

    /* Stops timer since an available deliverer was found */
    clock_gettime(CLOCK_REALTIME, & stop);

    /* Unlocks time mutex for deliverers and checks if it has been unlocked succesfully */
    rc = pthread_mutex_unlock( & lock_time_dlvr);
    if (rc != 0) {
        printf("ERROR: return code from pthread_mutex_unlock() is %d\n", rc);
        pthread_exit( & rc);
    }

    /* Calculates waiting time till an available deliverer was found */
    wait_dlvr = stop.tv_sec - start.tv_sec;

    /* Locks oven mutex and checks if it has been locked succesfully */
    rc = pthread_mutex_lock( & lock_oven);
    if (rc != 0) {
        printf("ERROR: return code from pthread_mutex_lock() is %d\n", rc);
        pthread_exit( & rc);
    }

    /* Increases number of available ovens by one, since the oven is now available for other orders */
    Noven++;

    /* Unblocks oven thread and checks if it has been unblocked succesfully */
    rc = pthread_cond_signal( & cond_ovens);
    if (rc != 0) {
        printf("ERROR: return code from pthread_cond_signal() is %d\n", rc);
        pthread_exit( & rc);
    }

    /* Unlocks oven mutex and checks if it has been unlocked succesfully */
    rc = pthread_mutex_unlock( & lock_oven);
    if (rc != 0) {
        printf("ERROR: return code from pthread_mutex_unlock() is %d\n", rc);
        pthread_exit( & rc);
    }

    /* Gives the needed time to the deliverer in order to deliver an order */
    sleep(time_dlvr + time_dlvr);

    /* Locks print mutex and checks if it has been locked succesfully */
    rc = pthread_mutex_lock( & lock_print);
    if (rc != 0) {
        printf("ERROR: return code from pthread_mutex_lock() is %d\n", rc);
        pthread_exit( & rc);
    }

    /* Calculates the time needed in order to deliver the order and the time needed
     * for the order to cool and prints them */
    long tot_time = wait_cook + time_prep + wait_oven + Tbake + wait_dlvr + time_dlvr;
    long cool_time = wait_dlvr + time_dlvr;
    printf("Order with number %d has been delivered in %ld minutes and was cooling for %ld minutes.\n", id, tot_time, cool_time);

    /* Unlocks print mutex and checks if it has been unlocked succesfully */
    rc = pthread_mutex_unlock( & lock_print);
    if (rc != 0) {
        printf("ERROR: return code from pthread_mutex_unlock() is %d\n", rc);
        pthread_exit( & rc);
    }

    /* Locks deliverer mutex and checks if it has been locked succesfully */
    rc = pthread_mutex_lock( & lock_dlvr);
    if (rc != 0) {
        printf("ERROR: return code from pthread_mutex_lock() is %d\n", rc);
        pthread_exit( & rc);
    }

    /* Increases number of available deliverers by one, since the deliverer is now available for other orders */
    Ndeliverer++;

    /* Unblocks deliverer thread and checks if it has been unblocked succesfully */
    rc = pthread_cond_signal( & cond_dlvr);
    if (rc != 0) {
        printf("ERROR: return code from pthread_cond_signal() is %d\n", rc);
        pthread_exit( & rc);
    }

    /* Unlocks deliverer mutex and checks if it has been unlocked succesfully */
    rc = pthread_mutex_unlock( & lock_dlvr);
    if (rc != 0) {
        printf("ERROR: return code from pthread_mutex_unlock() is %d\n", rc);
        pthread_exit( & rc);
    }

    /* Stores delivery time of each order in an array */
    tot_time_arr[id - 1] = tot_time;

    /* Stores time that an order need to cool in an array */
    cool_time_arr[id - 1] = cool_time;

    /* Terminates calling thread */
    pthread_exit(NULL);
}

/** Main method */
int main(int argc, char * argv[]) {
    /* Return value declaration */
    int rc;

    /* Number of customers as a program argument */
    Ncust = atoi(argv[1]);

    /* Checks if the number of orders is a positive number */
    if (Ncust < 0) {
        printf("ERROR: The number of threads to run should be a positive number. Current number given %d.\n", Ncust);
        exit(-1);
    }

    /* Random seed as a program argument */
    seed = atoi(argv[2]);

    /* Creates an array for orders and checks if malloc has succeeded */
    id = malloc(Ncust * sizeof(int));
    if (id == NULL) {
        printf("Not Enough Memory!\n");
        return -1;
    }

    /* Allocates requested memory and checks if malloc has succeeded */
    tot_time_arr = malloc(Ncust * sizeof(int));
    if (tot_time_arr == NULL) {
        printf("Not Enough Memory!\n");
        return -1;
    }

    /* Allocates requested memory and checks if malloc has succeeded */
    cool_time_arr = malloc(Ncust * sizeof(int));
    if (cool_time_arr == NULL) {
        printf("Not Enough Memory!\n");
        return -1;
    }

    /* Identifies an array of threads and checks if malloc has succeeded */
    pthread_t * threads;
    threads = malloc(Ncust * sizeof(pthread_t));
    if (threads == NULL) {
        printf("Not Enough Memory!\n");
        return -1;
    }

    /* Initializes cook mutex and checks if it has been initialized succesfully */
    rc = pthread_mutex_init( & lock_cook, NULL);
    if (rc != 0) {
        printf("ERROR: return code from pthread_mutex_init() is %d\n", rc);
        exit(-1);
    }

    /* Initializes oven mutex and checks if it has been initialized succesfully */
    rc = pthread_mutex_init( & lock_oven, NULL);
    if (rc != 0) {
        printf("ERROR: return code from pthread_mutex_init() is %d\n", rc);
        exit(-1);
    }

    /* Initializes deliverer mutex and checks if it has been initialized succesfully */
    rc = pthread_mutex_init( & lock_dlvr, NULL);
    if (rc != 0) {
        printf("ERROR: return code from pthread_mutex_init() is %d\n", rc);
        exit(-1);
    }

    /* Initializes time mutex for cooks and checks if it has been initialized succesfully */
    rc = pthread_mutex_init( & lock_time_cook, NULL);
    if (rc != 0) {
        printf("ERROR: return code from pthread_mutex_init() is %d\n", rc);
        exit(-1);
    }

    /* Initializes time mutex for ovens and checks if it has been initialized succesfully */
    rc = pthread_mutex_init( & lock_time_oven, NULL);
    if (rc != 0) {
        printf("ERROR: return code from pthread_mutex_init() is %d\n", rc);
        exit(-1);
    }

    /* Initializes time mutex for deliverers and checks if it has been initialized succesfully */
    rc = pthread_mutex_init( & lock_time_dlvr, NULL);
    if (rc != 0) {
        printf("ERROR: return code from pthread_mutex_init() is %d\n", rc);
        exit(-1);
    }

    /* Initializes print mutex and checks if it has been initialized succesfully */
    rc = pthread_mutex_init( & lock_print, NULL);
    if (rc != 0) {
        printf("ERROR: return code from pthread_mutex_init() is %d\n", rc);
        exit(-1);
    }

    /* Initializes cook conditional variable and checks if it has been initialized succesfully */
    rc = pthread_cond_init( & cond_cooks, NULL);
    if (rc != 0) {
        printf("ERROR: return code from pthread_cond_init() is %d\n", rc);
        exit(-1);
    }

    /* Initializes oven conditional variable and checks if it has been initialized succesfully */
    rc = pthread_cond_init( & cond_ovens, NULL);
    if (rc != 0) {
        printf("ERROR: return code from pthread_cond_init() is %d\n", rc);
        exit(-1);
    }

    /* Initializes deliverer conditional variable and checks if it has been initialized succesfully */
    rc = pthread_cond_init( & cond_dlvr, NULL);
    if (rc != 0) {
        printf("ERROR: return code from pthread_cond_init() is %d\n", rc);
        exit(-1);
    }

    /* Creates a thread per order and checks if it has been created successfully */
    for (int i = 0; i < Ncust; i++) {
        id[i] = i + 1; //thread counter
        rc = pthread_create( & threads[i], NULL, order, & id[i]);
        if (rc != 0) {
            printf("ERROR: return code from pthread_create() is %d\n", rc);
            exit(-1);
        }

        /* Allocates a random time range between each order.
         * The time between orders varies from 1 to 5 minutes! */
        int order_time = rand_r( & seed) % (Torderhigh + 1 - Torderlow) + Torderlow;
        sleep(order_time);
    }

    /* Suspends execution of the calling thread until the target thread terminates */
    for (int i = 0; i < Ncust; i++) {
        /* Join for all threads and checks if thread has been succesfully terminated */
        rc = pthread_join(threads[i], NULL);
        if (rc != 0) {
            printf("ERROR: return code from pthread_join() is %d\n", rc);
            exit(-1);
        }
    }

    /* Destroys cook mutex and checks if it has been destroyed succesfully */
    rc = pthread_mutex_destroy( & lock_cook);
    if (rc != 0) {
        printf("ERROR: return code from pthread_mutex_destroy() is %d\n", rc);
        exit(-1);
    }

    /* Destroys oven mutex and checks if it has been destroyed succesfully */
    rc = pthread_mutex_destroy( & lock_oven);
    if (rc != 0) {
        printf("ERROR: return code from pthread_mutex_destroy() is %d\n", rc);
        exit(-1);
    }

    /* Destroys deliverer mutex and checks if it has been destroyed succesfully */
    rc = pthread_mutex_destroy( & lock_dlvr);
    if (rc != 0) {
        printf("ERROR: return code from pthread_mutex_destroy() is %d\n", rc);
        exit(-1);
    }

    /* Destroys time mutex for cooks and checks if it has been destroyed succesfully */
    rc = pthread_mutex_destroy( & lock_time_cook);
    if (rc != 0) {
        printf("ERROR: return code from pthread_mutex_destroy() is %d\n", rc);
        exit(-1);
    }

    /* Destroys time mutex for ovens and checks if it has been destroyed succesfully */
    rc = pthread_mutex_destroy( & lock_time_oven);
    if (rc != 0) {
        printf("ERROR: return code from pthread_mutex_destroy() is %d\n", rc);
        exit(-1);
    }

    /* Destroys time mutex for deliverers and checks if it has been destroyed succesfully */
    rc = pthread_mutex_destroy( & lock_time_dlvr);
    if (rc != 0) {
        printf("ERROR: return code from pthread_mutex_destroy() is %d\n", rc);
        exit(-1);
    }

    /* Destroys print mutex and checks if it has been destroyed succesfully */
    rc = pthread_mutex_destroy( & lock_print);
    if (rc != 0) {
        printf("ERROR: return code from pthread_mutex_destroy() is %d\n", rc);
        exit(-1);
    }

    /* Destroys cook conditional variable and checks if it has been destroyed succesfully */
    rc = pthread_cond_destroy( & cond_cooks);
    if (rc != 0) {
        printf("ERROR: return code from pthread_cond_destroy() is %d\n", rc);
        exit(-1);
    }

    /* Destroys oven conditional variable and checks if it has been destroyed succesfully */
    rc = pthread_cond_destroy( & cond_ovens);
    if (rc != 0) {
        printf("ERROR: return code from pthread_cond_destroy() is %d\n", rc);
        exit(-1);
    }

    /* Destroys deliverer conditional variable and checks if it has been destroyed succesfully */
    rc = pthread_cond_destroy( & cond_dlvr);
    if (rc != 0) {
        printf("ERROR: return code from pthread_cond_destroy() is %d\n", rc);
        exit(-1);
    }

    /* Initializes maximum and total delivery and cooling time */
    int max_dlvr_time = tot_time_arr[0];
    float sum_dlvr_time = 0;
    int max_cool_time = cool_time_arr[0];
    float sum_cool_time = 0;

    /* Finds the maximum time of delivering and cooling an order */
    for (int i = 0; i < Ncust; i++) {
        sum_dlvr_time += tot_time_arr[i];
        sum_cool_time += cool_time_arr[i];

        if (tot_time_arr[i] > max_dlvr_time) {
            max_dlvr_time = tot_time_arr[i];
        }

        if (cool_time_arr[i] > max_cool_time) {
            max_cool_time = cool_time_arr[i];
        }
    }

    /* Calculates the average time of delivering and cooling an order */
    float avg_dlvr_time = sum_dlvr_time / Ncust;
    float avg_cool_time = sum_cool_time / Ncust;

    /* Prints the average and maximum time of delivering and cooling an order */
    printf("The average time of delivering an order is %.2f minutes and the maximum time of delivering an order is %d minutes.\n", avg_dlvr_time, max_dlvr_time);
    printf("The average time of cooling an order is %.2f minutes and the maximum time of cooling an order is %d minutes.\n", avg_cool_time, max_cool_time);

    return 0;
}
