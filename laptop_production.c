
#include "local.h"


int  MIN                    ;                /* Min threshold for generate a suitable sleeping time            				        */
int  MAX                    ;                /* Max threshold for generate a suitable sleeping time            				        */
int  MAX_THRESHOLD          ;                /* Max threshold of number of laptop cartoons in the storage room 				        */
int  MIN_THRESHOLD          ;                /* Min threshold, if the number of cartoons reach this number the factory should keep producing        */
int  SALES_THRESHOLD_MIN    ;                /* sales threshold to decide if we want to make suspension or not 				        */
int  SALES_THRESHOLD_MAX    ;                /* sales threshold to decide if we want to make suspension or not 				        */
int  TOTAL_GAIN_THRESHOLD   ;                /* total gain of the factory to stop the execution of the program                                      */
int  STANDARD_TECH_SALARY   ;                /* standard salary for the technical engineers and this value will change according to his effeciency  */
int  STANDARD_TRUCK_SALARY  ;                /* standard salary for the Truck employee and this value will change according to his effeciency       */
int  STANDARD_STORAGE_SALARY;                /* standard salary for the storage employee and this value will change according to his effeciency     */
int  STANDARD_LOAD_SALARY   ;                /* standard salary for the load employee and this value will change according to his effeciency        */
int  ONE_WEEK               ;                /* the value of time which represent one week of work                                                  */

int global_gain  = 0;                        /* global gain of the factory */
int global_gain_ = 0;                         

int                sleep_time_tech       [NUMBER_PRODUCTION_LINE_FIXED][NUMBER_TECH_ENGINEERS_FIXED ];  /* storing the sleep time for each tech eng              */
int                tech_employee_s       [NUMBER_PRODUCTION_LINE_FIXED][NUMBER_TECH_ENGINEERS_FIXED ];  /* storing the salary for each tech eng                  */    
production_line_t  production_lines      [NUMBER_PRODUCTION_LINE_FIXED];                      /* array of threads representing the production lines              */
int                production_lines_id   [NUMBER_PRODUCTION_LINE_FIXED];                      /* array of integers storing the threads ids                       */
technical_eng_t    technical_engineers   [NUMBER_PRODUCTION_LINE_FIXED][NUMBER_TECH_ENGINEERS_FIXED]; /* array of threads to store the tech eng                  */
int                technical_engineers_id[NUMBER_PRODUCTION_LINE_FIXED][NUMBER_TECH_ENGINEERS_FIXED]; /* array of integers storing the threads ids               */
int                visited               [NUMBER_PRODUCTION_LINE_FIXED][NUMBER_TECH_ENGINEERS_FIXED]; /* array of integers storing the  already visisted threads */
int                order                 [NUMBER_PRODUCTION_LINE_FIXED]; 		                    /* array of integers storing the threads ids               */
int                done_production       [NUMBER_PRODUCTION_LINE_FIXED];                               /* array to store the production line which finish         */
int                message_queues_id     [NUMBER_PRODUCTION_LINE_FIXED];                               /* storing the message queues ids                          */
int                visited_rand          [NUMBER_PRODUCTION_LINE_FIXED][NUMBER_TECH_ENGINEERS_FIXED];  /* this array is for generating random roles of the tech   */
struct Laptop_Cartoon     *truck         [TRUCKS_CAPACITY];                                            /* storing the cartoons in the trucks                      */
laptop             laptop_cartoon        [NUMBER_PRODUCTION_LINE_FIXED];                               /* storing laptops in the cartoon                          */
   
Storage_Employee_t storage_employee;                       /* an instance of storage employee structure */
Loading_Employee_t load_employee;                          /* an instance of load employee structure    */
Truck_Employee_t   truck_employee        [NUMBER_TRUCKS];  /* an instance of truck employee structure   */
int                truck_employee_s      [NUMBER_TRUCKS];  /* storing truck emloyees salary             */


report             financial_report;         /* financial report of the system */
HR_t               HR_employee;              /* HR manager structure           */
CEO_t              CEO_manager;              /* CEO manager structure          */


void              check_the_factory_results      (int);  /* check the factory gain after one week                        */
void              signal_catcher                 (int);  /* signal catcher for start storing the cartoons                */
void              start_production                  ();  /* this routine is for each production line thread              */
void              production_steps             (int *);  /* this routine is for the tech engineers threads               */
void              set_counters                      ();  /* setting the variables and counters                           */
void              set_counters_1                    ();  /* setting the variables and counters                           */
void              fill_into_cartoon            (int *);  /* last tech engineer thread routine                            */
void              add_cartoon_to_storage            ();  /* storage employee thread routine                              */
void              load_employee_waiting             ();  /* load employee thread routine                                 */
void              print_the_storage                 ();  /* print the storage                                            */
int               number_in_the_storage         (int );  /* number of cartoons in the storage                            */
void              delete_cartoon                    ();  /* delete a cartoon from the storage                            */
void              the_truck_move               (int *);  /* truck thread routine                                         */
int               is_ready_to_store                 ();  /* check if the cartoon is ready to store                       */
int               all_visited                    (int);  /* this function is for generate 5 - 10 steps in different order*/
void              HR_routine                        ();  /* HR thread routine                                            */
void              CEO_routine                       ();  /* CEO thread routine                                           */
void              read_file                   (char *);  /* read the file of data                                        */

int               status;                   /* stauss for ending the thread  */
int               storage_employees_salary; /* salary of the storage employee*/
int               CEO_salary;               /* salary of the CEO  manager    */
int               HR_salary;                /* salary of the HR manager      */
int               load_employees_salary;    /* salary of the load employee   */


int               number_of_suspension_lines  = 0;   /* number of suspension production lines */
int               number_of_suspension_lines_ = 0;


production_line_t get_production_line      (pthread_t);  /* as the production lines have same routines, this function return a specific running thread */
technical_eng_t   get_technical_eng  (pthread_t , int);  /* as the tech engineers have same routines, this function return a specific running thread   */

struct Storage_Room *storage;   /* shared memory */
struct Laptop_Cartoon *P;       
int ret;

key_t key;                     /* key for the message queues */
int n;               
char f_char = 'A';
int shmid;
int current_number_in_storage;
int storage_status;

MESSAGE msg;               /* message queue structure */

pthread_mutex_t global_mutex        = PTHREAD_MUTEX_INITIALIZER;   /* global mutex                                              */
struct msqid_ds buf; 
struct shmid_ds shm_buf;                                            /* Structure of message queue information to use with msgctl */

int main(int argc , char *argv[]){
   /* check if the data file is passed as an argument to the program */
   if(argc != 2){
     fprintf(stderr, "Usage: %s No text file &\n", argv[0]);
     return 1;
   }
   printf("\n\n\n%s====>=====>====>======>=====>=====>======>=====>=====>======>======>====>\n",RED);
   printf("%s--> Enlarge your screen to see better output, Regards... :-)\n",RED);
   printf("%s====>=====>====>======>=====>=====>======>=====>=====>======>======>====>\n",RED);   
   printf("\n\n\n");
   printf("%s                           <><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><> \n",CYAN);
   printf("%s                           <<<-----<>----<>- %s< Our Factory is START Working >%s -<>------<>--->>>\n",CYAN,YELLOW,CYAN);
   printf("%s                           <><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><>\n\n\n",CYAN);
   /* read the text file data.txt */
   read_file(argv[1]);
   /* our system is sensitive to an alarm which send a SIG alarm after each week to make the factory calculations */
   if (sigset(SIGALRM, check_the_factory_results) == SIG_ERR ) {
         perror("Sigset can not set SIGALRM");
         exit(2);
    }
    /* generate different random numbers */
    srand((unsigned)(getpid())); 
    /* 
    * message queues build, each production line has a message queue, to keep the system working 
    * and all the tech engineeres keep working, on the other hand, when the production reach to the final step
    * after that, eahc step will produce a new laptop 
    */ 
    for(unsigned int i = 0 ; i < NUMBER_PRODUCTION_LINE_FIXED ; i++ , f_char++){
       key = ftok("." , f_char);
       if((message_queues_id[i] = msgget(key, IPC_CREAT | 0660)) == -1){
         perror("Message Queue create");
         exit(3);
       }
    }
    /* build the shared memory */
    if ( (shmid = shmget(ftok(".", 'Z'), 4096, IPC_CREAT | 0644)) < 0 ) {
      perror("shmget fail");
      exit(4);
    }
    
    /*
    * attach to the shared memory
    */
    if ( (P = (struct Laptop_Cartoon *) shmat(shmid, 0, 0)) == (struct Laptop_Cartoon *) -1 ) {
      perror("shmat: parent");
      exit(5);
    }
    /* reference to the shared memory */
    storage = (struct Laptop_Cartoon *) P; 
    struct parameter param;
    /* build an instance of the storage employee structure */
    storage_employee = (Storage_Employee_t)malloc(sizeof(struct Storage_Employee));
    /* mutex initialization */
    if(pthread_mutex_init(&(storage_employee -> storage_mutex) , NULL) != 0){
        perror("production line mutex ");
        exit(6);
    }
    /* conditional variable initialization */
    if(pthread_cond_init(&(storage_employee  -> storage_cond)  , NULL) != 0){
        perror("production line cond ");
        exit(7);
     }
    /* build an instance of the load employee structure */
    load_employee = (Loading_Employee_t)malloc(sizeof(struct Loading_Employee));
    if(pthread_mutex_init(&(load_employee -> load_mutex) , NULL) != 0){
        perror("production line mutex ");
        exit(8);
     }
     if(pthread_cond_init(&(load_employee  -> load_cond)  , NULL) != 0){
        perror("production line cond ");
        exit(9);
     }


    /* storing the CEO and HR manager salaries */
    CEO_salary               = rand() % 500 + 2500;  
    CEO_salary /= 100;
    CEO_salary *= 100;
    HR_salary                = rand() % 500 + 1500;
    HR_salary /= 100;
    HR_salary *= 100;
    /* set the cpunters */
    set_counters();
    /* build an instances of the truck  structures */
    for(unsigned int i = 0 ; i < TRUCKS_CAPACITY ; i++){
      truck[i] = (struct Laptop_Cartoon*) malloc(sizeof(struct Laptop_Cartoon)); 
    }
    /* build the production lines */
    for(unsigned int i = 0 ; i < NUMBER_PRODUCTION_LINE_FIXED ; i++){
      production_lines[i] = (production_line_t)malloc(sizeof(struct production_line));
      production_lines[i] -> is_available = 1;
    }
    /* build the truck emplyees structure and initaliz its variable */
    for(int i = 0 ; i < NUMBER_TRUCKS; i++){
      truck_employee[i] = (Truck_Employee_t)malloc(sizeof(struct Truck_Employee));
      truck_employee[i] -> is_available = 1;
      truck_employee[i] -> number_travels = 0;   
     }
    /* start the production lines */
    for(unsigned int i = 0 ; i < NUMBER_PRODUCTION_LINE_FIXED; i++){
         production_lines[i] -> id           = i;   /* the id of the production line     */
         production_lines[i] -> is_ready     = 0;   /* check if the line is ready or not */
         /* build its mutex */
         if(pthread_mutex_init(&(production_lines[i] -> update_mutex) , NULL) != 0){
            perror("production line mutex ");
            exit(10);
         }
         /* create the thread */
         if(pthread_create(&(production_lines[i] -> thread) , NULL , (void *)start_production , NULL) != 0){
            perror("pthread create");
            exit(11);
         }
      }

      /* 
      * our system is sensitive to SIGUSR1, we used it to send a signal when all the lines finish 
      * producing their laptop 
      */
      if (sigset(SIGUSR1, signal_catcher) == SIG_ERR ) {
         perror("Sigset can not set SIGUSR1");
         exit(12);
      }
  

      /* waiting for the production lines threads to end */
      for(unsigned int i = 0 ; i < NUMBER_PRODUCTION_LINE_FIXED ; i++){
        if(pthread_join(production_lines[i] -> thread , (void *) &status) != 0){
           perror("pthread join");
           exit(13);
         }
      }        
     printf("\n\n\n");
     /* once the simulation is end */
     sleep(3);
     printf("%s   !===!===!====!====!====! End of Simulation !===!===!====!====!====!\n",GREEN); 
     printf("%s      Number Of suspensioned lines : %d\n",YELLOW,number_of_suspension_lines_);
     printf("%s      Total factory gains          : %d\n",YELLOW, global_gain_);
     printf("%s   !===!===!====!====!====!===!===!====!===!==!===!===!====!====!====!\n\n\n",GREEN);
     if(pthread_join(storage_employee -> thread , (void *) &status) != 0){
         perror("pthread__join__storage");
         exit(14);
     }
     if(pthread_join(load_employee -> thread , (void *) &status) != 0){
         perror("pthread__join__load");
         exit(15);
    }
    for(unsigned int i = 0 ; i < NUMBER_PRODUCTION_LINE_FIXED ; i++){
      msgctl(message_queues_id[i], IPC_RMID, &buf);
    }
    if(shmctl(shmid , IPC_RMID , &shm_buf) == -1){
        perror("shmem remove");
        exit(16);
    }
  }

/* Get a specific production line */
production_line_t get_production_line(pthread_t production_line){
   for(unsigned int i = 0 ; i < NUMBER_PRODUCTION_LINE_FIXED ; i++){
      if(production_lines[i] -> is_available == 1 && production_lines[i] -> thread == production_line){
         return production_lines[i];
      }
    }
   return NULL;
}
/* Get a specific tech engineer */
technical_eng_t get_technical_eng(pthread_t tech_engineer , int id){
  for(unsigned int i = 0 ; i < NUMBER_TECH_ENGINEERS_FIXED ; i++){
     if(visited[id][i] != 1 && technical_engineers[id][i] -> thread == tech_engineer){
         return technical_engineers[id][i];
     }
  }
  return NULL;
}
/* production line thread routine */
void start_production(){
    int status;
    production_line_t current_production_line; /* build a production line instance */
    struct sched_param param;
    pthread_t thread = pthread_self();                    /* determinning the currently running thread     */
    current_production_line = get_production_line(thread);/* determinning whivh production line is running */
       printf("%s   =====> Production Line %d has been created  \n",GREEN,current_production_line -> id + 1);
       /* build its technical engineers structures */
       for(unsigned int i = 0 ; i < NUMBER_TECH_ENGINEERS_FIXED ; i++){
          technical_engineers[current_production_line -> id][i] = (technical_eng_t)malloc(sizeof(struct technical_eng));
        }
       /* setting the counters */ 
       set_counters();
       /*
       * sccording to the project description, first 5 tech engineers should run in order, so we seperate 
       * their running from the one that can run in any order as the following 
       */
       
       for(unsigned int i = 0 ; i < NUMBER_TECH_ENGINEERS_FIXED - 5 ; i++){
     	  MESSAGE msg;
     	  /* storing the tech engineer informtion */
          technical_engineers[current_production_line -> id][i] -> step_is_ready = 0;
          technical_engineers[current_production_line -> id][i] -> line          = current_production_line;
          technical_engineers[current_production_line -> id][i] -> step          = i; 
          visited_rand       [current_production_line -> id][i]                  = 1;
          /* storing the tech engineer structure to send it threough its message queue line */
          msg.mtype = 1;
          msg.tech_eng  = technical_engineers[current_production_line -> id][i];
          if(msgsnd(message_queues_id[current_production_line -> id] , &msg , sizeof(msg) , 0) == -1){
             perror("pthread create");
             pthread_exit(&ret);
          }
     	  pthread_mutex_unlock(&(current_production_line -> update_mutex));
       }
       /* now we are going to generate the last 5 randomly */
       while(all_visited(current_production_line -> id) == 0){
          int rand_ = (rand() % (NUMBER_TECH_ENGINEERS_FIXED - 5)) + NUMBER_TECH_ENGINEERS_FIXED - 5;
          if(visited_rand[current_production_line -> id][rand_] == 0){
             MESSAGE msg;
             technical_engineers[current_production_line -> id][rand_] -> step_is_ready = 0;
             technical_engineers[current_production_line -> id][rand_] -> line          = current_production_line;
    	     technical_engineers[current_production_line -> id][rand_] -> step          = rand_; 
             pthread_mutex_lock(&(current_production_line -> update_mutex));
             msg.mtype = 1;
             msg.tech_eng  = technical_engineers[current_production_line -> id][rand_];
             if(msgsnd(message_queues_id[current_production_line -> id] , &msg , sizeof(msg) , 0) == -1){
                perror("pthread create");
                pthread_exit(&ret);
             }
             visited_rand[current_production_line -> id][rand_] = 1;
     	     pthread_mutex_unlock(&(current_production_line -> update_mutex));   
           }
         }
      int i = 0;
      int total_inc = 0;
     /* start the one week alarm */
     if(current_production_line -> id == 0){
        alarm(ONE_WEEK);
     } 
      
     /* main loop of th program */
      
    while(number_of_suspension_lines_ < NUMBER_PRODUCTION_LINE_FIXED / 2 && global_gain_ < TOTAL_GAIN_THRESHOLD){
      /* first we check if the current line is available and nt suspened */
      /*
      * the idea is that, when the 10 tech engineers start their steps, each step after that will produce a new laptop 
      * to store it in the cartoon
      */
      if(current_production_line -> is_available == 1){
        if(total_inc == 9){
            pthread_mutex_lock(&(global_mutex));
            set_counters_1();
            pthread_mutex_unlock(&(global_mutex));
        }
        /* message to reciev from the message queue for each line */
        MESSAGE msg;
        if((n = msgrcv(message_queues_id[current_production_line -> id], &msg, sizeof(MESSAGE), 1 ,0)) == -1 ) {
           perror("msgrcv");
           exit (19);
        }else if(n == 0){
          pthread_exit(&ret);
        }
       /* if we still didn't give each tech engineer his work */
        if(total_inc < 9 - number_of_suspension_lines){
           /* each one of them should make a production step */
           if(pthread_create(&(msg.tech_eng -> thread), NULL , (void *)production_steps , &(current_production_line -> id)) != 0){
              perror("pthread create");
              pthread_exit(&ret);
            }
          /* wait for him to finish his work */
           if(pthread_join(msg.tech_eng -> thread , (void *) &status) != 0){
              perror("pthread join");
              pthread_exit(&ret);
           }  
       }else{
         /* the last tech employee should fill the the laptop into the cartoon */
         if(pthread_create(&(msg.tech_eng -> thread), NULL , (void *)fill_into_cartoon , &(current_production_line -> id)) != 0){
            perror("pthread create");
            pthread_exit(&ret);
          } 
        /* wait for him to finish */
        if(pthread_join(msg.tech_eng -> thread , (void *) &status) != 0){
           perror("pthread join");
           pthread_exit(&ret);
        }    
        /* the production is done */     
        done_production[current_production_line -> id]++;

        if(is_ready_to_store()){
          kill(getpid() , SIGUSR1);
          /* if the number of cartoons in the storage has exceeded a specific threshold */
          current_number_in_storage = number_in_the_storage(0);
          if(current_number_in_storage > MAX_THRESHOLD){
             printf("%s==!===!====!======!====!\n",RED);
             printf("The storage is full !\n");
             printf("%s==!===!====!======!====!\n",RED);
             sleep(5);
             while(current_number_in_storage > MAX_THRESHOLD){
               if(current_number_in_storage > 0){
                  /* we stop the tech engineers and enforce the storage employee to start collecting the cartoons */
                  if(pthread_create(&(load_employee -> thread) , NULL , (void *)load_employee_waiting , NULL) != 0){
                      perror("pthread create");
                      pthread_exit(&ret);
                  }
                }
             current_number_in_storage = number_in_the_storage(0);
            }
           }           
            set_counters();
         }
      }
     
     pthread_mutex_lock(&(global_mutex));
     i++;
     if(total_inc >= 9){
        total_inc = 9 ;
     }else{
        total_inc++;
     }
      pthread_mutex_unlock(&(global_mutex));
      visited[current_production_line -> id][msg.tech_eng -> step] = 1;   
    }          
  }
}
/* setting counters function */
void set_counters(){
  for(unsigned int i = 0 ; i < NUMBER_PRODUCTION_LINE_FIXED ; i++){
      done_production[i] = 0;
      order[i]           = 0;

   }
  for(unsigned int i = 0 ; i < NUMBER_PRODUCTION_LINE_FIXED ; i++){
    for(int j = 0 ; j < NUMBER_TECH_ENGINEERS_FIXED ; j++){
       visited[i][j] = 0;
    }
  }
}
void set_counters_1(){
  for(unsigned int i = 0 ; i < NUMBER_PRODUCTION_LINE_FIXED ; i++){
    for(int j = 0 ; j < NUMBER_TECH_ENGINEERS_FIXED ; j++){
       visited[i][j]      = 0;
       visited_rand[i][j] = 0;
    }
  }
}

/*tech engineer production step */
void production_steps(int *id){
  int step_time; 
  /* loch the mutex */
  pthread_mutex_lock(&(production_lines[*id] -> update_mutex));
  /* message to be sent through the message queue */
  MESSAGE msg;
  pthread_t thread = pthread_self();
  technical_eng_t current_tech_eng = get_technical_eng(thread ,  *id);
  step_time = (rand() % (MAX + 1 - MIN)) + MIN;
  /* give the tech employee a proper salary according to his time of working */
  tech_employee_s[*id][current_tech_eng -> step] = STANDARD_TECH_SALARY - (20 * (step_time - 1));
  sleep(step_time);
  printf("%s   =====> Step #%d in %sProduction Line %d \n",YELLOW ,  current_tech_eng -> step + 1 , GREEN , *id + 1);
  
  msg.tech_eng = current_tech_eng;
  msg.mtype    = 1;
  if(msgsnd(message_queues_id[*id] , &msg , sizeof(msg) , 0) == -1){
        perror("pthread create");
        pthread_exit(&ret);
  }
  pthread_mutex_unlock(&(production_lines[*id] -> update_mutex));
}

/* fill the laptop into cartoon */
void fill_into_cartoon(int *id){
  int step_time; 
  pthread_mutex_lock(&(production_lines[*id] -> update_mutex));
  MESSAGE msg;
  pthread_t thread = pthread_self();
  step_time = (rand() % (MAX + 1 - MIN)) + MIN;
  technical_eng_t current_tech_eng = get_technical_eng(thread ,  *id);
  tech_employee_s[*id][current_tech_eng -> step] = STANDARD_TECH_SALARY - (20 * (step_time - 1));
  sleep(step_time);
  msg.tech_eng = current_tech_eng;
  msg.mtype    = 1;
  if(msgsnd(message_queues_id[*id] , &msg , sizeof(msg) , 0) == -1){
        perror("pthread create");
        pthread_exit(&ret);
  }
  
  /* insert the laptop to the cartoon */
  laptop_cartoon[*id] = (laptop)malloc(sizeof(struct LAPTOP));
  laptop_cartoon[*id] -> line = production_lines[*id];
  
  pthread_mutex_unlock(&(production_lines[*id] -> update_mutex)); 
}

/* add the cartoon to the storage */
void add_cartoon_to_storage(){
  pthread_mutex_lock(&(storage_employee -> storage_mutex));
  int sleep_time;
  /* attach to the shared memory */
  if ( (P = (struct Laptop_Cartoon *) shmat(shmid, 0, 0)) == (struct Laptop_Cartoon *) -1 ) {
      perror("shmat: parent");
      pthread_exit(&ret);
    }
  storage = (struct Laptop_Cartoon *) P; 
  /* insert cartoon to the shared memory */
  struct Laptop_Cartoon *link = (struct Laptop_Cartoon*) malloc(sizeof(struct Laptop_Cartoon)); 
  memcpy(&link->laptop_cartoon , &laptop_cartoon , sizeof(laptop_cartoon));
  link -> next = storage -> laptop_cartoon;
  for(int i = 0 ; i < NUMBER_PRODUCTION_LINE_FIXED ; i++){
    if(production_lines[i]->is_available == 0){
      for(int j = 0 ; j < NUMBER_PRODUCTION_LINE_FIXED ; j++){
         if(production_lines[j]->is_available == 1){
           link -> laptop_cartoon[i] -> line -> id = j;
           break;
         }
      }
    }
    /* determining the laptop cost fab as well as its price sell */
    link -> laptop_cartoon[i] -> cost_fab = rand() % 100 + 250;
    link -> laptop_cartoon[i] -> cost_fab /= 10;
    link -> laptop_cartoon[i] -> cost_fab *= 10;
    
    link -> laptop_cartoon[i] -> price_sell  = rand() % 1501 + 3500;
    link -> laptop_cartoon[i] -> price_sell /= 100;
    link -> laptop_cartoon[i] -> price_sell *= 100;
    
  }  
  storage -> laptop_cartoon = link;
  printf("%s   <><><><><><><><><><><><><><>><><><><><><><><><><><><><><><><><><>\n",BLUE);
  printf("%s   The storage employee has moved the cartoon to the Storage room.. \n",YELLOW);
  printf("%s   <><><><><><><><><><><><><><>><><><><><><><><><><><><><><><><><><>\n\n\n",BLUE);
  sleep_time = (rand() % (MAX + 1 - MIN)) + MIN;
  sleep(sleep_time);
  /* give the storage employee salary according to his time of work */
  storage_employees_salary = STANDARD_STORAGE_SALARY - ((sleep_time - 1) * 20);
  shmdt((void *)shmid);
  pthread_mutex_unlock(&(storage_employee -> storage_mutex));
  print_the_storage();
}

/* print the content of the storage */
void print_the_storage(){
  pthread_mutex_lock(&(storage_employee -> storage_mutex));
  if ( (P = (struct Laptop_Cartoon *) shmat(shmid, 0, 0)) == (struct Laptop_Cartoon *) -1 ) {
      perror("shmat: parent");
      pthread_exit(&ret);
    }
  storage = (struct Laptop_Cartoon *) P;
  struct Laptop_Cartoon *link = (struct Laptop_Cartoon*) malloc(sizeof(struct Laptop_Cartoon)); 
  link = storage -> laptop_cartoon;
  int i = 0;
  printf("%s   ====!====!=====!======!=====< The Storage >==!======!====!=====!========!==!\n",GREEN);
  while(link  != NULL){
   struct Laptop_Cartoon *lap_cartoon = (struct Laptop_Cartoon*) malloc(sizeof(struct Laptop_Cartoon));
   memcpy(&lap_cartoon -> laptop_cartoon , &link -> laptop_cartoon , sizeof(link -> laptop_cartoon));

   printf("%s   Cartoon #%d : \n",YELLOW, i + 1);
  for(int i = 0 ; i < NUMBER_PRODUCTION_LINE_FIXED ; i++){
    if(i < 9)
       printf("%s       LAPTOP: %d,  from Line:  %d, its fab cost: %d, its sell price: %d\n",CYAN,i + 1,lap_cartoon -> laptop_cartoon[i] -> line -> id + 1 , lap_cartoon -> laptop_cartoon[i] -> cost_fab , lap_cartoon -> laptop_cartoon[i] -> price_sell );
  else
       printf("%s       LAPTOP: %d, from Line: %d, its fab cost: %d, its sell price: %d\n",CYAN,i + 1,lap_cartoon -> laptop_cartoon[i] -> line -> id + 1 , lap_cartoon -> laptop_cartoon[i] -> cost_fab , lap_cartoon -> laptop_cartoon[i] -> price_sell );
  }
  
  link = link -> next;
  i++;
  }
  printf("%s   ====!====!=====!======!=====!======!====!=====!========!=====!======!====!===!\n\n",GREEN);
  shmdt((void *)shmid);
  pthread_mutex_unlock(&(storage_employee -> storage_mutex));  
}

/* number of cartoons in the storage */
int number_in_the_storage(int id){
  pthread_mutex_lock(&(storage_employee -> storage_mutex));
  storage = (struct Laptop_Cartoon *) P;
  int count = 0;
  struct Laptop_Cartoon *link = (struct Laptop_Cartoon*) malloc(sizeof(struct Laptop_Cartoon)); 
  link = storage -> laptop_cartoon;

  while(link  != NULL){
    struct Laptop_Cartoon *lap_cartoon = (struct Laptop_Cartoon*) malloc(sizeof(struct Laptop_Cartoon));
    memcpy(&lap_cartoon -> laptop_cartoon , &link -> laptop_cartoon , sizeof(link -> laptop_cartoon));
    count++;
    link = link -> next;
  }
  shmdt((void *)shmid);
  pthread_mutex_unlock(&(storage_employee -> storage_mutex));  
  return count;

}

/* load the cartoons to the struck */
void load_employee_waiting(){
  int current_number , i = 0 , counter = 0 , j = 0;
  int total_cost = 0 , total_sale = 0 , sleep_time;
  pthread_mutex_lock(&(load_employee -> load_mutex));
  pthread_mutex_lock(&(storage_employee -> storage_mutex)); 
   printf("\n\n");
   printf("%s   <-><-><-><-><-><-><-><-><-><-><-><-><-><-><-><-><-><-><-><-><-><-><-><->\n",GREEN);
   printf("%s   Now the load employee start moving the cartoons to the trucks .. \n",YELLOW );
   printf("%s   <-><-><-><-><-><-><-><-><-><-><-><-><-><-><-><-><-><-><-><-><-><-><-><->\n",GREEN);
   if((P = (struct Laptop_Cartoon *) shmat(shmid, 0, 0)) == (struct Laptop_Cartoon *) -1 ) {
      perror("shmat: parent");
      pthread_exit(&ret);
    }
  storage = (struct Laptop_Cartoon *) P;
  struct Laptop_Cartoon *link = (struct Laptop_Cartoon*) malloc(sizeof(struct Laptop_Cartoon)); 
  struct Laptop_Cartoon *link1 = (struct Laptop_Cartoon*) malloc(sizeof(struct Laptop_Cartoon)); 
  struct Laptop_Cartoon *link2 = (struct Laptop_Cartoon*) malloc(sizeof(struct Laptop_Cartoon));   
  link = storage -> laptop_cartoon;
  link1= link;
  link2= link;
  while(link2 != NULL){
    counter++;
    link2 = link2 -> next;
  }
  if(counter >= TRUCKS_CAPACITY){
    while(j < TRUCKS_CAPACITY && link1 != NULL){
       for(int i = 0 ; i < NUMBER_PRODUCTION_LINE_FIXED ; i++){
          total_cost += link1 -> laptop_cartoon[i] -> cost_fab;
           total_sale += link1 -> laptop_cartoon[i] -> price_sell;
        }
       link1 = link1 -> next;
       j++;
    }
    j = 0;
    /* storing the financila report of the laptops */
    financial_report = (report)malloc(sizeof(struct REPORT));
    financial_report -> number_of_lines        = NUMBER_PRODUCTION_LINE_FIXED;
    financial_report -> total_laptops_fab_cost = total_cost;
    financial_report -> total_laptops_sales    = total_sale;
    HR_employee = (HR_t) malloc(sizeof(struct HR));
    HR_employee -> financial_report = financial_report;
    if(link != NULL){
       while(i < TRUCKS_CAPACITY && link  != NULL){
          i++;
          delete_cartoon();
          link = link -> next;
        }
       i = 0;
       sleep_time = (rand() % (MAX + 1 - MIN)) + MIN + 1;
       sleep(sleep_time);
       load_employees_salary = STANDARD_LOAD_SALARY - ((sleep_time - 1) * 20); 
     /* 
      * start truck Employee which is available right now, and make the least 
      * number of travels
      */
      int mn = 1e5;
      int which_one;
      for(int i = 0 ; i < NUMBER_TRUCKS ; i++){
         if(truck_employee[i] -> is_available == 1){
            if(truck_employee[i] -> number_travels < mn){
                mn = truck_employee[i] -> number_travels;
            }
          }
       }     
      for(int i = 0 ; i < NUMBER_TRUCKS ; i++){
         if(truck_employee[i] -> is_available && truck_employee[i] -> number_travels == mn){
            which_one = i;
            break;
         }
       }
      truck_employee[which_one]-> is_available = 0;
      printf("\n\n"); 
      printf("%s   <-><-><-><-><-><-><-><-><-><-><-><-><-><-><-><-><-><-><-><-><-><-><-><->\n",GREEN);
      printf("%s   All cartoons now are stored in %s truck %d%s, it can move \n",YELLOW ,RED ,  which_one,YELLOW );
      printf("%s   <-><-><-><-><-><-><-><-><-><-><-><-><-><-><-><-><-><-><-><-><-><-><-><->\n",GREEN);
     /* starting the truck employee thread */
     if(pthread_mutex_init(&(truck_employee[which_one]-> truck_mutex) , NULL) != 0){
        perror("production line mutex ");
        pthread_exit(&ret);
     }
    /* create the thread */
     if(pthread_create(&(truck_employee[which_one] -> thread) , NULL , (void *) the_truck_move , &which_one) != 0){
       perror("pthread create");
       pthread_exit(&ret);
     }
     /* waiting for him to finish */
    if(pthread_join(truck_employee[which_one] -> thread , (void *) &status) != 0){
       perror("pthread join");
       pthread_exit(&ret);
     }
     
     truck_employee[which_one]-> number_travels ++;
     truck_employee[which_one]-> is_available = 1;
     
     /* HR manager thread */
    
     if(pthread_mutex_init(&(HR_employee -> HR_mutex) , NULL) != 0){
        perror("HR mutex ");
        pthread_exit(&ret);
    }
     if(pthread_create(&(HR_employee -> thread) , NULL , (void *) HR_routine , NULL) != 0){
       perror("pthread create");
       pthread_exit(&ret);
     }
     if(pthread_join(HR_employee -> thread , (void *) &status) != 0){
       perror("pthread join");
       pthread_exit(&ret);
     }
   }
 }

  shmdt((void *)shmid);
  pthread_mutex_unlock(&(storage_employee -> storage_mutex));
  pthread_mutex_unlock(&(load_employee -> load_mutex));  
}

/* truck employee routine */
void the_truck_move(int *id){
  printf("\n\n");
  int sleep_time = (rand() % (MAX + 1 - MIN)) + MIN + 1;
  printf("%s    ===>===>====>====>====>====>====>====>====>====>====>===>===>\n",YELLOW);
  printf("%s    The truck %d Contains The cartoons, and it starts moving..\n",CYAN,*id);
  printf("%s    ===>===>====>====>====>====>====>====>====>====>====>===>===>\n",YELLOW);
  sleep(sleep_time);
  truck_employee_s[*id] = STANDARD_TRUCK_SALARY - ((sleep_time - 1) * 20);
  printf("%s    <====<====<====<====<====<====<====<====<====<===<===<===<===\n",YELLOW);
  printf("%s    The truck %d arrive .. !\n" , CYAN,*id);
  printf("%s    <====<====<====<====<====<====<====<====<====<===<====<===<==\n",YELLOW);
}

/* delete cartoon from the storage */
void delete_cartoon(){
    struct Laptop_Cartoon  *temp;
    struct Laptop_Cartoon  *prev;
    temp = storage->laptop_cartoon;
    if(temp != NULL){
       storage->laptop_cartoon = temp -> next;
       free(temp);
       return;
    }
       while(temp != NULL){
          prev = temp;
          temp = temp -> next;
       }
       if(temp == NULL){
          return;
       }
       prev -> next = temp -> next;
       free(temp);
}

/* once all the production lines are fininshed producing laptops */
void signal_catcher(int the_sig){
    if(pthread_create(&(storage_employee -> thread) , NULL , (void *)add_cartoon_to_storage , NULL) != 0){
         perror("pthread create");
         pthread_exit(&ret);
    } 
    current_number_in_storage = number_in_the_storage(0);
    if(current_number_in_storage >= TRUCKS_CAPACITY){
        if(pthread_create(&(load_employee -> thread) , NULL , (void *)load_employee_waiting , NULL) != 0){
           perror("pthread create");
           pthread_exit(&ret);
        }
     }
}

/* check if the laptop is ready to store */
int is_ready_to_store(){
  int counter = 0;
  for(int i = 0 ; i < NUMBER_PRODUCTION_LINE_FIXED ; i++){
     counter += done_production[i] > 0;
  }
  return (counter == NUMBER_PRODUCTION_LINE_FIXED - number_of_suspension_lines) ? 1 : 0;
}

/* this function is for making the last 5 steps of production in random order */
int all_visited(int id){
  int counter = 0;
  for(int i = 0 ; i < NUMBER_TECH_ENGINEERS_FIXED; i++){
     counter += visited_rand[production_lines[id]->id][i] == 1;
  }
  return (counter == NUMBER_TECH_ENGINEERS_FIXED) ? 1 : 0;
}

/* HR routine */
void HR_routine(){
    pthread_mutex_lock(&(HR_employee -> HR_mutex));
    int total_tech_cost    = 0;
    /* calculating the total cost */
    for(unsigned int i = 0 ; i < NUMBER_PRODUCTION_LINE_FIXED - number_of_suspension_lines ; i++){
      for(unsigned int j = 0 ; j < NUMBER_TECH_ENGINEERS_FIXED ; j++){
        total_tech_cost += tech_employee_s[i][j];
      }
    }
    total_tech_cost *= TRUCKS_CAPACITY;
    int total_storage_cost = storage_employees_salary;
    int total_load_cost    = load_employees_salary   ;
    int total_truck_cost   = 0;
    for(unsigned int i = 0 ; i < NUMBER_TRUCKS ; i++){
      total_truck_cost += truck_employee_s[i];
    }
    int total_ceo_cost     = CEO_salary;
    int total_hr_cost      = HR_salary;
    int total_gain         = HR_employee -> financial_report -> total_laptops_sales - (total_tech_cost + total_storage_cost + total_load_cost + total_truck_cost + total_ceo_cost + total_hr_cost );
    printf("\n\n");
    printf("%s    !=======!=======!=======!======= < Financial Calculation > =======!=======!=======!=======!\n",YELLOW);
    printf("%s           Laptops fab cost                 = %d              \n",GREEN , HR_employee -> financial_report -> total_laptops_fab_cost);
    printf("%s           Laptops technical engineers cost = %d              \n",GREEN , total_tech_cost);
    printf("%s           Laptops storage   employee  cost = %d              \n",GREEN , total_storage_cost);
    printf("%s           Laptops load      employee  cost = %d              \n",GREEN , total_load_cost);
    printf("%s           Laptops truck     employee  cost = %d              \n",GREEN , total_truck_cost);
    printf("%s           CEO       cost = %d                 \n",GREEN , total_ceo_cost);
    printf("%s           HR        cost = %d                 \n",GREEN , total_hr_cost);
    printf("%s           total sales    = %d                 \n",RED   , HR_employee -> financial_report -> total_laptops_sales);
    printf("%s           <>---<>--<>--<>--<>--<>--<>--<>--<> \n",GREEN); 
    printf("%s           total factory gain               = %d              \n",BLUE   , total_gain);
    global_gain += total_gain;
    global_gain_+= total_gain;
    printf("%s           total factory gain until now     = %d              \n",BLUE   , global_gain_);

    printf("%s    !=======!=======!=======!=======!=======!========!=======!========!=======!=======!=======!\n",YELLOW);
    sleep(1);
    pthread_mutex_unlock(&(HR_employee -> HR_mutex));
}

/* CEO routine */
/* CEO is deciding for suspension or cancelling the suspension */
void CEO_routine(){
  pthread_mutex_lock(&(CEO_manager -> CEO_mutex));
  if(CEO_manager -> need_to_reduce == 1){
    for(unsigned int i = 0 ; i < NUMBER_PRODUCTION_LINE_FIXED ; i++){
      if(production_lines[i] -> is_available == 1){
        production_lines[i] -> is_available = 0;
        number_of_suspension_lines++;
        number_of_suspension_lines_++;
        break;
      }
    }
  }else{
    for(unsigned int i = 0 ; i < NUMBER_PRODUCTION_LINE_FIXED ; i++){
      if(production_lines[i] -> is_available == 0){
        production_lines[i] -> is_available = 1;
        production_lines[i] -> id = i;
        number_of_suspension_lines--;
        break;
      }
    }
  }
  pthread_mutex_unlock(&(CEO_manager -> CEO_mutex));  
}

/* check the factory result after one week of execution */
void check_the_factory_results(int the_sig){
    printf("\n\n\n");
    printf("%s    ====>=====>====>======>=====>=====>======>=====>=====>======>======>====>===>===>====>====>\n",RED);
    printf("%s    After one day of factory working, both the CEO and the HR need to make some calculations\n",YELLOW);
    printf("%s    ====>=====>====>======>=====>=====>======>=====>=====>======>======>====>===>===>====>====>\n",RED);
    printf("\n\n");
    CEO_manager = (CEO_t) malloc(sizeof(struct CEO));
    CEO_manager -> financial_report = financial_report;
    printf("%s    !=======!=======!=======!======= < according to the HR and the CEO > =======!=======!=======!=======!\n",CYAN);
    printf("%s           The factory total gain in one week             = %d              \n",GREEN,  global_gain);
    printf("%s           The expected gain from the factory in one week = %d              \n",GREEN , SALES_THRESHOLD_MIN);
    if(global_gain >= SALES_THRESHOLD_MIN){
       if(number_of_suspension_lines != 0 && global_gain >= SALES_THRESHOLD_MAX){
           printf("%s           The gain is good, so the suspension need to be cancelled\n",YELLOW   );
           CEO_manager -> need_to_reduce = 2;
           if(pthread_mutex_init(&(CEO_manager -> CEO_mutex) , NULL) != 0){
              perror("HR mutex ");
              pthread_exit(&ret);
           }
          if(pthread_create(&(CEO_manager -> thread) , NULL , (void *) CEO_routine , NULL) != 0){
             perror("pthread create");
             pthread_exit(&ret);
           }
          if(pthread_join(CEO_manager -> thread , (void *) &status) != 0){
             perror("pthread join");
             pthread_exit(&ret);
          } 
       }else{
          printf("%s           No Need for suspension                   \n",CYAN   );
       }
    }else{
           printf("%s           Me, as an HR manager I'll call the CEO to make suspension as the factory gain is not very good\n",YELLOW   );
           CEO_manager -> need_to_reduce = 1;
           if(pthread_mutex_init(&(CEO_manager -> CEO_mutex) , NULL) != 0){
             perror("HR mutex ");
             pthread_exit(&ret);
           }
          if(pthread_create(&(CEO_manager -> thread) , NULL , (void *) CEO_routine , NULL) != 0){
            perror("pthread create");
            pthread_exit(&ret);
          }
         if(pthread_join(CEO_manager -> thread , (void *) &status) != 0){
            perror("pthread join");
            pthread_exit(&ret);
        } 
      }
   printf("%s    !=======!=======!=======!=======!======!=====!=========!=======!=======!=======!=======!=======!=======!\n\n\n",CYAN);
   global_gain = 0;
   /* resetting the alarm */
   alarm(ONE_WEEK);
}

/* read file of input data */
void read_file(char *data){
   FILE *file;
   char MIN_S                    [10]; 
   char MAX_S                    [10];
   char MAX_THRESHOLD_S          [20];
   char MIN_THRESHOLD_S          [20];
   char SALES_THRESHOLD_MIN_S    [20];
   char SALES_THRESHOLD_MAX_S    [20];
   char TOTAL_GAIN_THRESHOLD_S   [20];
   char STANDARD_TECH_SALARY_S   [20];
   char STANDARD_TRUCK_SALARY_S  [20];
   char STANDARD_STORAGE_SALARY_S[20];
   char STANDARD_LOAD_SALARY_S   [20];
   char ONE_WEEK_S               [20];  
   /* Opening the files */
   file = fopen(data , "r");
   if(file == NULL){
     perror("Open file");
     pthread_exit(&ret);
   }
   fscanf(file, "%s", MIN_S);
   MIN = atoi(MIN_S);
   fscanf(file, "%s", MAX_S);
   MAX = atoi(MAX_S);
   fscanf(file, "%s", MAX_THRESHOLD_S);
   MAX_THRESHOLD = atoi(MAX_THRESHOLD_S);
   fscanf(file, "%s", MIN_THRESHOLD_S);
   MIN_THRESHOLD = atoi(MIN_THRESHOLD_S);
   fscanf(file, "%s", SALES_THRESHOLD_MIN_S);
   SALES_THRESHOLD_MIN = atoi(SALES_THRESHOLD_MIN_S);
   fscanf(file, "%s", SALES_THRESHOLD_MAX_S);
   SALES_THRESHOLD_MAX = atoi(SALES_THRESHOLD_MAX_S);
   fscanf(file, "%s", TOTAL_GAIN_THRESHOLD_S);
   TOTAL_GAIN_THRESHOLD = atoi(TOTAL_GAIN_THRESHOLD_S);
   fscanf(file, "%s", STANDARD_TECH_SALARY_S);
   STANDARD_TECH_SALARY = atoi(STANDARD_TECH_SALARY_S);
   fscanf(file, "%s", STANDARD_TRUCK_SALARY_S);
   STANDARD_TRUCK_SALARY = atoi(STANDARD_TRUCK_SALARY_S);
   fscanf(file, "%s", STANDARD_STORAGE_SALARY_S);
   STANDARD_STORAGE_SALARY = atoi(STANDARD_STORAGE_SALARY_S);
   fscanf(file, "%s", STANDARD_LOAD_SALARY_S);
   STANDARD_LOAD_SALARY = atoi(STANDARD_LOAD_SALARY_S);
   fscanf(file, "%s", ONE_WEEK_S);
   ONE_WEEK = atoi(ONE_WEEK_S);
   if(fclose(file) == EOF){
     perror("Close file");
     pthread_exit(&ret);
   }
  
}
