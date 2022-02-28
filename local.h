#ifndef __LOCAL_H__
#define __LOCAL_H__

/* Coloring Codes */

#define RED    "\033[0;31m"
#define GREEN  "\033[0;32m"
#define YELLOW "\033[0;33m"
#define BLUE   "\033[0;34m"
#define WHITE  "\033[0;37m"
#define CYAN   "\x1b[36m"

/* Required includes */
#include <semaphore.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include <sys/wait.h>
#include<time.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <pthread.h>
#include <stdbool.h>

/* Reauired variables */
#define NUMBER_PRODUCTION_LINE_FIXED 10
#define NUMBER_TECH_ENGINEERS_FIXED  10  
#define TRUCKS_CAPACITY              5
#define NUMBER_TRUCKS                3

/*production Line thread structure */
typedef struct production_line{
   pthread_t thread;
   unsigned int is_ready;
   unsigned int id;
   unsigned int my_turn;
   unsigned int is_available;
   pthread_mutex_t update_mutex; 
   pthread_cond_t  finish_cond;
} * production_line_t;

/* technical engineering structure */
typedef struct technical_eng {
   production_line_t line;
   pthread_t thread;
   unsigned int keep_going;
   unsigned int cost;
   unsigned int step_is_ready;
   unsigned int step;
   
} * technical_eng_t;

/* Laptop structure */
typedef struct LAPTOP {
  production_line_t line;
  unsigned int cost_fab;
  unsigned int price_sell;
  
} *laptop;

/* storage employee structure */
typedef struct Storage_Employee{
  pthread_t       thread;
  pthread_mutex_t storage_mutex; 
  pthread_cond_t  storage_cond ;
} * Storage_Employee_t;

/* loading employee structure */
typedef struct Loading_Employee{
  pthread_t       thread;
  pthread_mutex_t load_mutex; 
  pthread_cond_t  load_cond ;
} *Loading_Employee_t;

/* truck employee structure */
typedef struct Truck_Employee{
  pthread_t       thread;
  unsigned int is_available;
  unsigned int number_travels;
  pthread_mutex_t truck_mutex; 
  pthread_cond_t  truck_cond ;
} *Truck_Employee_t;

struct parameter {                           
    int   policy;                              
    int   priority; 
    int   line_id;                          
};

/* laptop cartoon structure */
struct Laptop_Cartoon {
     struct LAPTOP *laptop_cartoon[NUMBER_PRODUCTION_LINE_FIXED];
     struct Laptop_Cartoon *next;
};

/* shared memory structure */
struct Storage_Room {
  struct Laptop_Cartoon *laptop_cartoon;
};

/* message to be sent through the message queue */
typedef struct  {
   long int mtype;          /* type of received/sent message */
   technical_eng_t tech_eng;           /* text of the message */
}MESSAGE;

/* CEO structure */
typedef struct CEO{
  pthread_t       thread;
  struct REPORT *financial_report;
  pthread_mutex_t CEO_mutex; 
  unsigned int need_to_reduce;
} *CEO_t;

/* HR structure */
typedef struct HR{
  pthread_t       thread;
 pthread_mutex_t HR_mutex; 
  struct REPORT *financial_report;
} *HR_t;

/* financial Report structure */
typedef struct REPORT{
  unsigned int number_of_lines;
  unsigned int total_laptops_fab_cost;
  unsigned int total_laptops_sales;
}*report;
#endif
