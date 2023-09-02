#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>  //fork
#include <sys/wait.h> //waitpid
#include <errno.h> 
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <sys/ipc.h> 
#include <sys/msg.h> 
#include <sys/types.h>
#define MAX_REGISTRANTS_PER_DAY 10
#define MAX_DAYS_PER_WEEK 7
#define MAX_NAME_LENGTH 300
#define BUS_CAPACITY 5

void handler(int signumber){
    if(signumber ==10){
    printf("Vineyard: I see bus1 is ready!\n");
    }
    else {printf("Vineyard: I see bus2 is ready!\n");}
 
} 
bool validate(char **days, int size)
{

    char *day_names[] = {"Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday"};
    char line[128];

    FILE *file = fopen("registrations.txt", "r");
    if (file == NULL)
    {
        printf("Error: could not open file.\n");
        return false;
    }

    for (int i = 0; i < size; i++)
    {
        bool validday = false;
        //iterate through given day and check if every single of them are equal to any of the strings from day_names
        for (int j = 0; j < MAX_DAYS_PER_WEEK; j++)
        {
            if (strcmp(days[i], day_names[j]) == 0)
            {
                //if som set it to true
                validday = true;
            }
        }
        //if valid day is still false
        if (!validday)
        {
            printf("%s is not a valid day of the week.\n", days[i]);
            // fclose(file);
            return false;
        }
        //go to the beginning of the file, start from 0 (from the very start)
        fseek(file, 0, SEEK_SET);
        int count = 0;
        //we get the line from the file
        while (fgets(line, sizeof(line), file))
        {
            //if the line contains our given day, increase the count to count how many of it we have
            if (strstr(line, days[i]))
            {

                count++;
            }
        }
        // printf("%d", count);


        if (count >= MAX_REGISTRANTS_PER_DAY)
        {
            printf("%s is full!", days[i]);
            fclose(file);
            return false;
        }
    }

    fclose(file);

    return true;
}

void register_applicant()
{
    char name[50];
    
    char days[57];
    printf("Enter your name: ");
    fgets(name, sizeof(name), stdin);
    name[strcspn(name, "\n")] = '\0';
    printf("Enter the days you can work (separated by spaces): ");
    fgets(days, sizeof(days), stdin);
    days[strcspn(days, "\n")] = '\0';
    char *givendays[10];
    char temp[128];
    strcpy(temp, days);
    //This line uses the strtok() function to tokenize the temp string based on 
    //space characters (" "), and sets day_str to point to the first token.
    char *day_str = strtok(temp, " ");
    int size = 0;

    while (day_str != NULL)
    {
    //This line assigns the current value of day_str to the sizeth 
    //element of the givendays array, and then increments size.
        givendays[size++] = day_str;
        day_str = strtok(NULL, " ");
    }

    if (validate(givendays, size))
    {

        FILE *file = fopen("registrations.txt", "a");
        fprintf(file, "Name: %s ", name);
        fprintf(file, "Days: %s\n", days);
        fclose(file);
        printf("Registration successful.\n");
    }
}

void modify_applicant()
{
    FILE *file = fopen("registrations.txt", "r");
    if (file == NULL)
    {
        printf("No registrations found.\n");
        return;
    }
    char line[100];
    int count = 1;
    while (fgets(line, sizeof(line), file))
    {
        if (strstr(line, "Name: ") != NULL)
        {
            printf("%d. %s", count, line);
            count++;
        }
        else if (strstr(line, "Days: ") != NULL)
        {
            printf("    %s", line);
        }
    }
    fclose(file);

    if (count == 1)
    {
        printf("No applicants found.\n");
        return;
    }

    int choice;
    do
    {
        printf("Enter the number of the applicant you want to modify (1-%d): ", count - 1);
        scanf("%d", &choice);
        getchar(); // consume newline character left by scanf
        if (choice < 1 || choice >= count)
        {
            printf("Invalid choice. Please try again.\n");
        }
    } while (choice < 1 || choice >= count);

    // get the new days input from the user
    char new_days[128];
    printf("Enter the new days you can work (separated by spaces): ");
    fgets(new_days, sizeof(new_days), stdin);
    new_days[strcspn(new_days, "\n")] = '\0';

    char *givendays[10];
    char temp[128];
    strcpy(temp, new_days);
    char *day_str = strtok(temp, " ");
    int size = 0;
    while (day_str != NULL)
    {
        givendays[size++] = day_str;
        day_str = strtok(NULL, " ");
    }

    if (validate(givendays, size))
    {
        // modify the chosen applicant's days in the file
        FILE *temp_file = fopen("temp.txt", "w");
        file = fopen("registrations.txt", "r");
        count = 1;
        int in_applicant_info = 0; // flag to keep track of whether we are in the chosen applicant's information
        while (fgets(line, sizeof(line), file))
        {
            if (strstr(line, "Name: ") != NULL)
            {
                //if the given number is not a number from the list
                if (count != choice)
                {
                    //write the whole line to the temprry file
                    fprintf(temp_file, "%s", line);
                    in_applicant_info = 0;
                }
                else //if it is the number we wanted
                {
                    //Pointer to Days:...
                    char *pos = strstr(line, " Days:");
                    char *substr;

                    // substracting from a line array starting from Days: .. so the len -> name
                    int len = pos - line;
                    // allocates mem in len + 1 size, so +1 is for null terminator
                    substr = malloc(len + 1);
                    //(destination, source, size)
                    //we copy the whole line but only in len length, so until Days: 
                    strncpy(substr, line, len);
                    //In this case, len is the length of the substring that was copied from the line array, 
                    //and adding the null terminator character to the end of the substr array ensures 
                    //that the substr array is a properly formatted string in C, with the null
                    // terminator indicating the end of the string.
                    //Without this line of code, the substr array would contain the copied substring 
                    //followed by whatever characters happened to be in memory after the end of the substring,
                    substr[len] = '\0'; // 

                    fprintf(temp_file, "%s", substr);
                    fprintf(temp_file, " Days: %s\n", new_days);
                    in_applicant_info = 1;
                }
                count++;
            }
            else if (strstr(line, " Days: ") != NULL)
            {
                //if  in_applicant_info = 1, this condition will not be executed
                if (!in_applicant_info)
                {
                    fprintf(temp_file, "  %s", line);
                }
            }
            else
            {
                fprintf(temp_file, " %s", line);
            }
        }
        fclose(file);
        fclose(temp_file);
        remove("registrations.txt");
        rename("temp.txt", "registrations.txt");

        printf("Applicant modified.\n");
    }
}
void list_applicants()
{
    FILE *file = fopen("registrations.txt", "r");
    if (file == NULL)
    {
        printf("No registrations found.\n");
        return;
    }
    char line[100];
    while (fgets(line, sizeof(line), file))
    {
        if (strstr(line, "Name: ") != NULL)
        {
            printf("%s", line);
        }
        else if (strstr(line, " Days: ") != NULL)
        {
            printf("%s", line);
        }
    }
    fclose(file);
}



char** get_applications(int day, int* count)
{
    char* day_names[] = {"Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday"};
    char** names = malloc(sizeof(char*) * 10);
    int ind = 0;

    FILE* file = fopen("registrations.txt", "r");
    if (file == NULL)
    {
        printf("No registrations found.\n");
        return NULL;
    }

    char line[100];
    while (fgets(line, sizeof(line), file))
    {
        if (strstr(line, day_names[day]) != NULL)
        {
            char* ptr = strstr(line, "Name: ");
            if (ptr != NULL)
            {
                char* name = malloc(sizeof(char) * 50);
                if (name == NULL)
                {
                    printf("Error: Failed to allocate memory for name.\n");
                    return NULL;
                }

                if (sscanf(ptr, "Name: %49[^ ]", name) == 1)
                {
                    names[ind++] = name;
                }
                else
                {
                    free(name);
                }
            }
        }
    }

    fclose(file);
    *count = ind;
    return names;
}


void delete_applicant()
{
    FILE *file = fopen("registrations.txt", "r");
    if (file == NULL)
    {
        printf("No registrations found.\n");
        return;
    }
    char line[100];
    //printing the options
    int count = 1;
    while (fgets(line, sizeof(line), file))
    {
        if (strstr(line, "Name: ") != NULL)
        {
            printf("%d. %s", count, line);
            count++;
        }
        else if (strstr(line, " Days: ") != NULL)
        {
            printf("    %s", line);
        }
    }
    fclose(file);
    ///////////////////////////

    if (count == 1)
    {
        printf("No applicants found.\n");
        return;
    }

    int choice;
    do
    {
        printf("Enter the number of the applicant you want to delete (1-%d): ", count - 1);
        scanf("%d", &choice);
        getchar(); // consume newline character left by scanf
        if (choice < 1 || choice >= count)
        {
            printf("Invalid choice. Please try again.\n");
        }
    } while (choice < 1 || choice >= count);

    // delete the chosen applicant and their days from the file
    FILE *temp_file = fopen("temp.txt", "w");
    file = fopen("registrations.txt", "r");
    count = 1;
    int in_applicant_info = 0; // flag to keep track of whether we are in the chosen applicant's information
    while (fgets(line, sizeof(line), file))
    {
        if (strstr(line, "Name: ") != NULL)
        {
            if (count != choice)
            {
                fprintf(temp_file, "%s", line);
                in_applicant_info = 0;
            }
            else
            {
                //just set it to 1 and ignore the current line
                in_applicant_info = 1;
            }
            count++;
        }
        else if (strstr(line, "Days: ") != NULL)
        {
            //will execute if we r not where we want
            if (!in_applicant_info)
            {
                fprintf(temp_file, "    %s", line);
            }
        }
        else
        {
            fprintf(temp_file, "%s", line);
        }
    }
    fclose(file);
    fclose(temp_file);
    remove("registrations.txt");
    rename("temp.txt", "registrations.txt");

    printf("Applicant deleted.\n");
}



 
     
struct busInfo { 
     long mtype;//this is a free value e.g for the address of the message
     int busid;
     int count; //people delivered
}; 

// sendig a message
int send( int mqueue, int busid, int num_sent_workers ) 
{ 
     const struct busInfo m = { 5, busid, num_sent_workers }; 
     int status; 
     
     status = msgsnd( mqueue, &m, 2*sizeof(int), 0 ); 
 //message queue identifier (mqueue), 
 //a pointer to the m structure, the size of the message 
       // a 4th parameter may be  IPC_NOWAIT, equivalent with  0-val 
     if ( status < 0 ) 
          perror("msgsnd error"); 
     return 0; 
} 
     
// receiving a message. 
int receive( int mqueue ) 
{ 
     struct busInfo m; 
     int status; 

   
     status = msgrcv(mqueue, &m, 2*sizeof(int), 5, 0 );  // 0 means it waits until message with type 5 is received. IPC_NOWAIT can replace it in case you don't want to wait for it but if its not found will send error
    // message queue identifier (mqueue), 
    // a pointer to the m structure, the size of the message 
     //(2*sizeof(int) in this case),
     // the message type to receive (5 in this case)
     if ( status < 0 ) 
          perror("msgsnd error"); 
     else
          printf( "Bus %d has brought in %d workers.\n\n", m.busid, m.count ); 
     return 0; 
}


void sendWorkers(int d){

     int pipefd[2];
     int pipefd2[2];
     char buffer1[5][MAX_NAME_LENGTH]; 
     char buffer2[5][MAX_NAME_LENGTH]; 
     int count = 0;

    //message queue/////////////////////
    // key_t key;
    // int sh_mem_id;
    // int *s;
    // key =ftok(".",1);
    // // create shared memory for reading and writing (500 bytes )
    // sh_mem_id=shmget(key,sizeof(int),IPC_CREAT|S_IRUSR|S_IWUSR);
    // // to connect the shared memory, 
    // s = shmat(sh_mem_id,NULL,0);

    // key_t key2;
    // int sh_mem_id2;
    // int *s2;
    // key2 =ftok(".",2);
    // // create shared memory for reading and writing (500 bytes )
    // sh_mem_id2=shmget(key2,sizeof(int) * 10,IPC_CREAT|S_IRUSR|S_IWUSR);
    // // to connect the shared memory, 
    // s2 = (int*)shmat(sh_mem_id2,NULL,0);
    /////////////////////////////////////////
    char** arr= get_applications(d,&count);
     //fixed part 1
     char** arr1 = malloc(BUS_CAPACITY * sizeof(char *));
    //  int arr2_size = count - BUS_CAPACITY+1;
    //  char **arr2 =  malloc(arr2_size * sizeof(char *));
     char** arr2 = malloc(BUS_CAPACITY * sizeof(char *));
     for (int i = 0; i < BUS_CAPACITY; i++) {
    arr1[i] = NULL;
     }

    for (int i = 0; i < BUS_CAPACITY; i++) {
    arr2[i] = NULL;
    }
     int arr1_count = 0;
     int arr2_count = 0;

     if (pipe(pipefd) == -1 || pipe(pipefd2)==-1) 
	 {
        perror("Opening error!");
        exit(EXIT_FAILURE);
     }

     printf("Applicants: \n");
   
    for(int i = 0; i<count;i++)
    {
        printf("%s \n",arr[i]);
    }

    ////DIVIDING LIST OF ALL APPLICANTS INTO 2 BUSES

    printf("Count : %d\n", count);
    int iter;
    if(count<BUS_CAPACITY){
        iter = count;    
    }
    else{ iter = BUS_CAPACITY;}
    for (int i = 0; i <iter; i++) {
  
        arr1[i] = arr[i];
        //printf("arr1: %s\n", arr1[i]);
        arr1_count++;
    }
     
    for (int i = BUS_CAPACITY; i < count; i++)
    {
       
        arr2[arr2_count] = arr[i];
        //printf("arr2: %s %d\n", arr2[arr2_count], arr2_count)
        arr2_count++;
    }



  struct sigaction sigact;
  sigact.sa_handler=handler; //SIG_DFL,SIG_IGN
  sigemptyset(&sigact.sa_mask); //during execution of handler these signals will be blocked plus the signal    
  //now only the arriving signal, SIGTERM will be blocked
  sigact.sa_flags=0; //nothing special behaviour
  sigaction(SIGTERM,&sigact,NULL);
  sigaction(SIGUSR1,&sigact,NULL);

     int status;

    key_t key = ftok(".", 1);
    int messg = msgget(key, 0600 | IPC_CREAT);
 
     pid_t child_pid_1, child_pid_2;

    child_pid_1 = fork();


/////////////PARENT PROCESS / VINEYARD////////////////////////////
if (child_pid_1 > 0) {   
    // This is the parent main process.
    printf("Vineyard strating the work...%i\n", getpid());
    child_pid_2=fork();
 if (child_pid_2 > 0) {
       
        sigset_t sigset;
        sigfillset(&sigset);
        sigdelset(&sigset,SIGUSR1);
        sigsuspend(&sigset);
        sigset_t sigset2;
        sigfillset(&sigset2);
        sigdelset(&sigset2,SIGTERM);
        sigsuspend(&sigset2);
        ///sending the names via pipe
        
        close(pipefd[0]); //Usually we close unused read end
        
        printf("Vineyard sending the names of workers for bus1!\n");
        for(int i =0; i<arr1_count; i++){
            write(pipefd[1], arr1[i], MAX_NAME_LENGTH + 1);
            printf("Written names for bus1: %s\n",arr1[i]);
        }

      
        close(pipefd[1]); // Closing write descriptor 
        
        fflush(NULL); 	// flushes all write buffers (not necessary)

         if(arr2_count>0)
        {
            close(pipefd2[0]);
            printf("Vineyard sending the names of workers for bus2!\n");
            for(int i =0; i<arr2_count; i++){
            write(pipefd2[1], arr2[i], MAX_NAME_LENGTH + 1);
            printf("Written names for bus2: %s\n",arr2[i]);
            }
            close(pipefd2[1]);
            //printf("Parent wrote the message to the pipe for bus2!\n");

        }

        sleep(7);	
        wait(NULL);              
        // printf("Vineyard: the number workers successfully sent by bus1: %d\n",*s);
        // if(arr2_count>0){
        // printf("Vineyard: the number workers successfully sent by bus2: %d\n",*s2);
        // }
         receive(messg);
         
	    // it releases the shared memory 
        // shmdt(s);
        // shmdt(s2);
        	   
	// IPC_RMID- to clear the shared memory
        // shmctl(sh_mem_id,IPC_RMID,NULL);
        // shmctl(sh_mem_id2,IPC_RMID,NULL);

        waitpid(child_pid_1,&status,0);

        if(arr2_count>0){
         receive(messg);}
        waitpid(child_pid_2,&status,0);
        
        ///remove the message queue
           status = msgctl( messg, IPC_RMID, NULL ); 
                if ( status < 0 ) 
                    perror("msgctl error");
    }


/////////////BUS2 / CHILD 2 /////////////////////////////////////////////
    else if (child_pid_2 == -1) 
    {
    perror("Error: Failed to create second child process.");
    exit(EXIT_FAILURE);
    }

    else if (child_pid_2 == 0) 
    {
    // This is the second child process.
    //printf(" child process 2 created...  %i\n", getpid());
    //receive signal from "parent1"
    printf("The bus2 is ready to start!\n", SIGTERM);
    sleep(5);
    kill(getppid(),SIGTERM);

    ///pipes for child2****************************
    sleep(3);
    if(arr2_count>0){
    close(pipefd2[1]);
	printf("BUS2 starts to read from the pipe!\n");
    // printf("Child read the message:\n");  
    for(int i=0; i<arr2_count; i++){
        read(pipefd2[0], buffer2[i], MAX_NAME_LENGTH + 1);
        // printf("%s\n",buffer[i]);
    }
    
    printf("BUS2 receives the name of workers:\n");
    for(int i = 0; i<arr2_count;i++)
    {
        printf("%s\n",buffer2[i]);
    }
   
    close(pipefd2[0]); // finally we close the used read end

    }

    // *s2 = arr2_count;
    // shmdt(s2);

    send(messg, 2, arr2_count);
    exit(0);
    }

//////////////////////////////////////////////////////////////////////////
}

/////////////CHILD 1 / BUS 1 ///////////////////////////////////////////////
 else if (child_pid_1 == 0) {
    // This is the first child process.
    //printf(" child process 1 created... %i\n", getpid());
    
    //receive signal from "parent1"
    printf("The bus1 is ready to start!\n", SIGUSR1);   
    sleep(3);
    kill(getppid(),SIGUSR1);

    sleep(3);
    close(pipefd[1]);
	printf("BUS1 starts to read from the pipe!\n");
    // printf("Child read the message:\n");  
    for(int i=0; i<arr1_count; i++){
        read(pipefd[0], buffer1[i], MAX_NAME_LENGTH + 1);
        // printf("%s\n",buffer[i]);
    }
    
    printf("BUS1 receives the name of workers:\n");
    for(int i = 0; i<arr1_count;i++)
    {
        printf("%s\n",buffer1[i]);
    }
   
    close(pipefd[0]); // finally we close the used read end

    // *s = arr1_count;
    // shmdt(;s)

    send(messg, 1, arr1_count);
    //wait(NULL);	   
	// IPC_RMID- to clear the shared memory
    //shmctl(sh_mem_id,IPC_RMID,NULL);
    exit(0);
} else {
    perror("Error: Failed to create first child process.");
    exit(EXIT_FAILURE);
}
//////////////////////////////////////////////////////////////////////////
}

void sendWorker(){
    int c;
    printf("Which day would you like to send the buses? Type a number of a week: ");
    scanf("%d", &c);
    sendWorkers(c-1);

}

int main(int argc,char* argv[])
{   

    int choice;
    do
    {
        printf("1. Register applicant\n");
        printf("2. List applicants\n");
        printf("3. Delete applicant\n");
        printf("4. Modify applicant\n");
        printf("5. Send Workers\n");
        printf("6. Exit\n");
        printf("Enter your choice (1-6): ");
        scanf("%d", &choice);
        getchar(); // consume newline character left by scanf
        switch (choice)
        {
        case 1:
            register_applicant();
            break;
        case 2:
            list_applicants();
            break;
        case 3:
            delete_applicant();
            break;
        case 4:
            modify_applicant();
            break;
        case 5:
            sendWorker();
            //sleep(15);
            break;

        case 6:
            printf("Goodbye!\n");
            break;

        default:
            printf("Invalid choice. Please try again.\n");
            break;
        }
    } while (choice != 6);

    
  

    return 0;
}