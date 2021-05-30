#include <stdio.h> 
#include <iostream>
#include <sys/types.h> 
#include <sys/times.h>
#include <sys/wait.h>
#include <unistd.h> 
#include <signal.h>
#include <stdlib.h>
#include <time.h>
using namespace std;

#define BUF_SIZE 65536

#define READ_END 0
#define WRITE_END 1

#define STDIN_FILENO 0
#define STDOUT_FILENO 1

#include <fstream>
#include <sstream>
#include <string>
#include <string.h>

#include <typeinfo>



//#include "handler.h"


// Variables storing command line arguments
int numWorkers;
bool range_flag;
int attriNum;
char* inputFile;
char* order;
char* outputFile;
int sig1_count = 0;
int sig2_count = 0;

// Function declaration
void Coord(struct Data *dataStruct, pid_t coord, pid_t merger, int rootPID, char* inputFile, char* outputFile, int numWorkers, bool range_flag, int attriNum, char* order);
void bubbleSort(Data data_array[], int data_count, string arg, string order, string outputFile);
void storeData(struct Data *dataStruct, Data dat[], int count);
void sorting_f(int i);
void merging_f();
void handler1(int param);
void handler2(int param);
int countData(FILE *file);


struct Data{ 
    int rpid; // individual ID
    char fname[15]; // first name
    char lname[15]; // last name
    int numDep; // # of dependents
    float income; // income
    int postal; // postal code
};

int main(int argc, char *argv[]){
    /// TIME --------------------
    double t1, t2, cpu_time;
    struct tms tb1, tb2;
    double ticspersec;
    ticspersec = (double) sysconf(_SC_CLK_TCK);
    t1 = (double) times(&tb1);
    /// -------------------------


    Data *data = NULL;

    int fd[6][2];
    int status = 0;
    char* str = (char*)malloc(20*sizeof(char));
    char* out = (char*)malloc(20*sizeof(char));
    char* ord = (char*)malloc(2*sizeof(char));
    int num;
    int attri;
    bool flag;

    for (int i=0; i<6; i++){
        if (pipe(fd[i]) < 0) {
            fprintf(stderr, "pipe failed.\n");
            exit(EXIT_FAILURE);
        }
    }

    // Boolean marker to indicate if the workers should be on "random ranges". Default is false.
    range_flag = false;

    // Declaring variables to deal with process ids
    pid_t root, coord, merger;

    // Storing the PID of ROOT for later use in SORTER and MERGER
    int rootPID = getpid();

    // Catching signals and calling handlers
    if (signal(SIGUSR1, handler1)==SIG_ERR) printf("No SIGUSR1 caught\n");
    if (signal(SIGUSR2, handler2)==SIG_ERR) printf("No SIGUSR2 caught\n");

	root = fork();
    switch(root){
        case -1:
            perror("ROOT fork failed\n");
            exit(EXIT_FAILURE);

        case 0:
        /*-------------------------------------------------*/
        /*----------------- COORD PROCESS -----------------*/
        /*-------------------------------------------------*/
            // Close writing end of pipe
            close(fd[0][WRITE_END]);
            close(fd[1][WRITE_END]);
            close(fd[2][WRITE_END]);
            close(fd[3][WRITE_END]);
            close(fd[4][WRITE_END]);
            close(fd[5][WRITE_END]);

            // Reading pipe + checking for error
            if (read(fd[0][READ_END], str, 50) < 0){
                fprintf(stderr, "Error on read to pipe: %d\n", errno);
                exit(EXIT_FAILURE);
            }
            // Reading pipe + checking for error
            if (read(fd[1][READ_END], &num, sizeof(int)) < 0){
                fprintf(stderr, "Error on read to pipe: %d\n", errno);
                exit(EXIT_FAILURE);
            }
            // Reading pipe + checking for error
            if (read(fd[2][READ_END], &flag, sizeof(bool)) < 0){
                fprintf(stderr, "Error on read to pipe: %d\n", errno);
                exit(EXIT_FAILURE);
            }
            // Reading pipe + checking for error
            if (read(fd[3][READ_END], &attri, sizeof(int)) < 0){
                fprintf(stderr, "Error on read to pipe: %d\n", errno);
                exit(EXIT_FAILURE);
            }
            // Reading pipe + checking for error
            if (read(fd[4][READ_END], ord, sizeof(char*)) < 0){
                fprintf(stderr, "Error on read to pipe: %d\n", errno);
                exit(EXIT_FAILURE);
            }
            // Reading pipe + checking for error
            if (read(fd[5][READ_END], out, 50) < 0){
                fprintf(stderr, "Error on read to pipe: %d\n", errno);
                exit(EXIT_FAILURE);
            }
            // Close reading end of pipe1, pipe2
            close(fd[0][READ_END]);
            close(fd[1][READ_END]);
            close(fd[2][READ_END]);
            close(fd[3][READ_END]);
            close(fd[4][READ_END]);
            close(fd[5][READ_END]);

            // Now passing arguments to the child processes
            Coord(data, coord, merger, rootPID, str, out, num, flag, attri, ord);
            exit(EXIT_FAILURE);

        default:
        /*------------------------------------------------*/
        /*----------------- ROOT PROCESS -----------------*/
        /*------------------------------------------------*/
            // Assigning command line arguments to variables.
            for (int i = 1; i < argc; ++i){
                string arg = argv[i];
                if (arg == "-i") inputFile = argv[i+1];
                if (arg == "-k") numWorkers = stoi(argv[i+1]);
                if (arg == "-r") range_flag = true;
                if (arg == "-a") attriNum = stoi(argv[i+1]);
                if (arg == "-o") order = argv[i+1];
                if (arg == "-s") outputFile = argv[i+1];
            }
            printf("ROOT pid = %d and ppid = %d\n", getpid(), getppid());
            
            // Close reading end of pipe
            close(fd[0][READ_END]);
            close(fd[1][READ_END]);
            close(fd[2][READ_END]);
            close(fd[3][READ_END]);
            close(fd[4][READ_END]);
            close(fd[5][READ_END]);

            // Writing "inputFile" into pipe + checking for error
            if (write(fd[0][WRITE_END], inputFile, 50) < 0){
                fprintf(stderr, "Error on write to pipe 1: %d\n", errno);
                exit(EXIT_FAILURE);
            }
            // Writing "numWorkers" into pipe + checking for error
            if (write(fd[1][WRITE_END], &numWorkers, sizeof(int)) < 0){
                fprintf(stderr, "Error on write to pipe 2: %d\n", errno);
                exit(EXIT_FAILURE);
            }
            // Writing "range_flag" into pipe + checking for error
            if (write(fd[2][WRITE_END], &range_flag, sizeof(bool)) < 0){
                fprintf(stderr, "Error on write to pipe 3: %d\n", errno);
                exit(EXIT_FAILURE);
            }
            // Writing "attriNum" into pipe + checking for error
            if (write(fd[3][WRITE_END], &attriNum, sizeof(int)) < 0){
                fprintf(stderr, "Error on write to pipe 4: %d\n", errno);
                exit(EXIT_FAILURE);
            }
            // Writing "order" into pipe + checking for error
            if (write(fd[4][WRITE_END], order, sizeof(char*)) < 0){
                fprintf(stderr, "Error on write to pipe 5: %d\n", errno);
                exit(EXIT_FAILURE);
            }
            // Writing "outputFile" into pipe + checking for error
            if (write(fd[5][WRITE_END], outputFile, 50) < 0){
                fprintf(stderr, "Error on write to pipe 6: %d\n", errno);
                exit(EXIT_FAILURE);
            }
            // Close writing end of pipe
            close(fd[0][WRITE_END]);
            close(fd[1][WRITE_END]);
            close(fd[2][WRITE_END]);
            close(fd[3][WRITE_END]);
            close(fd[4][WRITE_END]);
            close(fd[5][WRITE_END]);
    }
    while ((rootPID = wait(&status)) > 0);

    t2 = (double) times(&tb2);
    cpu_time = (double) ((tb2.tms_utime + tb2.tms_stime) - (tb1.tms_utime + tb1.tms_stime));
    printf("ENTIRE PROCESS Run time was %lf sec (REAL time) although we used the CPU for %lf sec (CPU time).\n", (t2 - t1) / ticspersec, cpu_time / ticspersec);
    printf("ROOT has seen %d SIGUSR1 from Sorters\n", sig1_count);
    printf("ROOT has seen %d SIGUSR2 from Merger\n", sig2_count);
}



void Coord(struct Data *dataStruct, pid_t coord, pid_t merger, int rootPID, char* inputFile, char* outputFile, int numWorkers, bool range_flag, int attriNum, char* order){
    /*--------------------------------
    Parent process - Coordinator node
    Child (1) processes - Sorter nodes
    Child (2) process - Merger node
    ---------------------------------*/

    /*-------------------------------------------------*/
    /*----------------- COORD PROCESS -----------------*/
    /*-------------------------------------------------*/

    int pip[numWorkers][2];
    char buf[numWorkers][BUF_SIZE]; // pipe max capacity is 65536 bytes on Linux
    int stdoutCopy[numWorkers];

    for (int x=0; x<numWorkers; x++){
        if (pipe(pip[x]) < 0) {
            fprintf(stderr, "pipe failed.\n");
            exit(EXIT_FAILURE);
        }
        memset(buf[x], 0, 4096);
        stdoutCopy[x] = dup(STDOUT_FILENO);
    }

    printf("COORD pid = %d and ppid = %d\n", getpid(), getppid());

    // Building an array that contains positions of splits / ranges for sorters
    int i, j, temp;
    FILE * file = fopen(inputFile, "r");
    int num = countData(file);
    // 1 less than numWorkers because
    // you need 1 split to make 2 subgroups, for instance.
    int * array = new int[numWorkers + 1];
    // in case "-r", we need to generate random number every iteration
    srand(time(0));
    // x, 1, 2, 3, y <-- filling in the positions where integers currently are
    // First and last elements left out because they will 0 and total # data respecively.
    // The reason is that I will be passing ranges from this array to sorters as follows:
    // (x, 1+1), (1, 2+1), (2, 3+1), (3, y+1) 
    for(int i=1; i<numWorkers; i++){
        // In case of "-r": random range
        if (range_flag == true){
            // Get random (numWorkers-1) numbers between 5 and the number of rows.
            // Chose 5 arbitrarily just to make sure it's not 0
            array[i] = rand() % num + 5;
        }
        // In case of even range
        else{
            int chunksize = (num / numWorkers);
            if (i == 0) array[i] = chunksize;
            else array[i] = array[i-1] + chunksize;
        }
    }
    array[0] = 0; // first element
    array[numWorkers] = num; // last element
    // Bubble sort this array
    for(i = 1; i<numWorkers; i++){
        for(j = i+1; j<numWorkers; j++){
            if(array[j] < array[i]){
                temp = array[i];
                array[i] = array[j];
                array[j] = temp;
            }
        }
    }

    //int array[3] = {0,180,600};
    
    // Creating SORTER nodes based on numWorkers
    for(int i=0; i<numWorkers; i++){
        /// TIME---------------
        double t1, t2, cpu_time;
        struct tms tb1, tb2;
        double ticspersec;
        ticspersec = (double) sysconf(_SC_CLK_TCK);
        t1 = (double) times(&tb1);
        ///--------------------
        int start = array[i];
        int end = array[i+1];
        coord = fork();
        switch(coord){
        case -1:
            perror("COORD fork failed\n");
            exit(EXIT_FAILURE);
        case 0:
        /*--------------------------------------------------*/
        /*----------------- SORTER PROCESS -----------------*/
        /*--------------------------------------------------*/
            printf("SORTER pid = %d and ppid = %d\n", getpid(), getppid());

            if(kill(rootPID, SIGUSR1)<0) fprintf(stderr,"[Error %d]\n",errno);

            // Converting variable to type char const* for execl() args
            string s = to_string(rootPID);
            char const* rpid = s.c_str();
            string r = to_string(range_flag);
            char const* range = r.c_str();
            string at = to_string(attriNum);
            char const* attri = at.c_str();
            string st = to_string(start);
            char const* sta = st.c_str();
            string e = to_string(end);
            char const* en = e.c_str();
            string tt = inputFile;
            char const* input = tt.c_str();
            string oo = order;
            char const* ord = oo.c_str();

            t2 = (double) times(&tb2);
            cpu_time = (double) ((tb2.tms_utime + tb2.tms_stime) - (tb1.tms_utime + tb1.tms_stime));
            printf("SORTER[%d] Run time was %lf sec (REAL time) although we used the CPU for %lf sec (CPU time).\n", i, (t2 - t1) / ticspersec, cpu_time / ticspersec);

            if(dup2(pip[i][WRITE_END], STDOUT_FILENO)==-1) perror("DUP2 FAILED");
            close(pip[i][WRITE_END]);

            // Even index sorters use Bubble-Sort
            if (i%2==0){
                execlp("./bubble", "bubble", rpid, range, input, attri, ord, sta, en, (char*)0);
                perror("execlp() failed");}
            // Odd index sorters use Heap-Sort
            else{
                execlp("./insert", "insert", rpid, range, input, attri, ord, sta, en, (char*)0);
                perror("execlp() failed");}
            exit(EXIT_SUCCESS);
        }
        wait(NULL);
        

        // reference: https://stackoverflow.com/questions/7292642/grabbing-output-from-exec
        // printf("%.*s\n", bitez, buf[i]);
        close(pip[i][WRITE_END]);
        //printf("%d\n", stdoutCopy[i]);
        if(dup2(stdoutCopy[i], STDOUT_FILENO) == -1) perror("DUP2 FAILED HERE");
        close(stdoutCopy[i]);
        //printf("BUFFER SIZE: %lu\n", sizeof(buf[i]));
        while((read(pip[i][READ_END], buf[i], BUF_SIZE))!=0);
        close(pip[i][READ_END]);
        fflush(stdout);
    }

    
    merger = fork();
    switch(merger){
        case -1:
            perror("MERGER fork failed\n");
            exit(EXIT_FAILURE);
        case 0:
        /*--------------------------------------------------*/
        /*----------------- MERGER PROCESS -----------------*/
        /*--------------------------------------------------*/

            printf("MERGER pid = %d and ppid = %d\n", getpid(), getppid());

            /// TIME ----------------------------
            double t1, t2, cpu_time;
            struct tms tb1, tb2;
            double ticspersec;
            ticspersec = (double) sysconf(_SC_CLK_TCK);
            t1 = (double) times(&tb1);
            /// ---------------------------------

            // Converting buffer content to string
            string s = "";
            for (int i=0; i<numWorkers; i++){
                s = s + buf[i];
                s = s.c_str();
            }

            //Data *loc[numWorkers];
            for (int i=0; i<numWorkers; i++){
                //printf(">>>>>> WE HERE %d:\n%s\n", i, buf[i]);
                struct Data *data = (struct Data*)malloc(sizeof(struct Data)*(num));
                dataStruct = data;
                //loc[i] = dataStruct;
                // cout << "UPDATED dataStruct: " << dataStruct << endl;
                //cout << "DUPLICATED     loc: " << loc[i] << endl;
                
                // string s = "";
                // s = s + buf[i];
                // s = s.c_str();

                // Input data records delivered from each process
                // to the data structure consructed above.
                // This is to sort them all att once.
                    
                istringstream is(s);
                string part;
                while(getline(is, part, '\n')){

                    char *dup = strdup(part.c_str());
                    char *token = strtok(dup, " ");
                    try{data -> rpid = stoi(token);}
                    catch(exception &err){cout<<"Conversion failure 1: "<< err.what() <<endl;}
                    //printf("%s ",token);

                    token = strtok(NULL, " ");
                    strcpy(data -> fname, token);
                    //printf("%s ",token);

                    token = strtok(NULL, " ");
                    strcpy(data -> lname, token);
                    //printf("%s ",token);

                    token = strtok(NULL, " ");
                    //printf("%s ",token);
                    try{data -> numDep = stoi(token);}
                    catch(exception &err){cout<<"Conversion failure 2: "<< err.what() <<endl;}

                    token = strtok(NULL, " ");
                    //printf("%s ",token);
                    try{data -> income = stof(token);}
                    catch(exception &err){cout<<"Conversion failure 3: "<< err.what() <<endl;}

                    token = strtok(NULL, " ");
                    //printf("%s\n",token);
                    try{data -> postal = stoi(token);}
                    catch(exception &err){cout<<"Conversion failure 4: "<< err.what() <<endl;}

                    data++;
                }
            }

            // Checking for attribute to sort by
            string arg;
            string oo = order;
            string tt = outputFile;
            char const* ou = tt.c_str();
            char const* ord = oo.c_str();
            if(attriNum == 0) arg = "rpid";
            if(attriNum == 3) arg = "numDep";
            if(attriNum == 4) arg = "income";
            if(attriNum == 5) arg = "postal";

            bubbleSort(dataStruct, num, arg, ord, ou);

            if(kill(rootPID, SIGUSR2)<0) fprintf(stderr,"[Error %d]\n",errno);

            t2 = (double) times(&tb2);
            cpu_time = (double) ((tb2.tms_utime + tb2.tms_stime) - (tb1.tms_utime + tb1.tms_stime));
            printf("MERGER Run time was %lf sec (REAL time) although we used the CPU for %lf sec (CPU time).\n",(t2 - t1) / ticspersec, cpu_time / ticspersec);

            exit(EXIT_SUCCESS);
    }
    wait(NULL);
}


int countData(FILE *file){
    // initial read point of a file when function is called
    int old_pos = ftell(file);
    int count = 0;
    char *temp = new char[128];
    while(fgets(temp, 128, file)!=NULL) count++;
    // restore original reading position
    fseek(file, old_pos, SEEK_SET);
    // return the number of data
    delete[] temp;
    return count;
}


void bubbleSort(Data data_array[], int data_count, string arg, string order, string outputFile){
    bool swap = false;
    Data temp;
    while (!swap){
        swap = true;
        for (int i = 0; i<data_count-1; i++){
            if (strncmp(arg.c_str(),"rpid",4)==0){
                // Ascending order
                if (strncmp(order.c_str(),"a",1)==0){
                    if (data_array[i].rpid > data_array[i+1].rpid){
                        temp = data_array[i];
                        data_array[i] = data_array[i+1];
                        data_array[i+1] = temp;
                        swap = false;
                    }
                }
                // Descending order
                else{
                    if (data_array[i].rpid < data_array[i+1].rpid){
                        temp = data_array[i];
                        data_array[i] = data_array[i+1];
                        data_array[i+1] = temp;
                        swap = false;
                    }
                }
            }
            if (strncmp(arg.c_str(),"numDep",6)==0){
                if (strncmp(order.c_str(),"a",1)==0){
                    if (data_array[i].numDep > data_array[i+1].numDep){
                        temp = data_array[i];
                        data_array[i] = data_array[i+1];
                        data_array[i+1] = temp;
                        swap = false;
                    }
                }
                else{
                    if (data_array[i].numDep < data_array[i+1].numDep){
                        temp = data_array[i];
                        data_array[i] = data_array[i+1];
                        data_array[i+1] = temp;
                        swap = false;
                    }
                }
            }
            if (strncmp(arg.c_str(),"income",6)==0){
                if (strncmp(order.c_str(),"a",1)==0){
                    if (data_array[i].income > data_array[i+1].income){
                        temp = data_array[i];
                        data_array[i] = data_array[i+1];
                        data_array[i+1] = temp;
                        swap = false;
                    }
                }
                else{
                    if (data_array[i].income < data_array[i+1].income){
                        temp = data_array[i];
                        data_array[i] = data_array[i+1];
                        data_array[i+1] = temp;
                        swap = false;
                    }
                }
            }
            if (strncmp(arg.c_str(),"postal",6)==0){
                if (strncmp(order.c_str(),"a",1)==0){
                    if (data_array[i].postal > data_array[i+1].postal){
                        temp = data_array[i];
                        data_array[i] = data_array[i+1];
                        data_array[i+1] = temp;
                        swap = false;
                    }
                }
                else{
                    if (data_array[i].postal < data_array[i+1].postal){
                        temp = data_array[i];
                        data_array[i] = data_array[i+1];
                        data_array[i+1] = temp;
                        swap = false;
                    }
                }
            }
        }
    }
     
    // Save the merged + sorted result in the output.csv
    ofstream myfile;
    //outputFile = outputFile + "csv";
    string ou = outputFile.c_str();
    myfile.open(ou);
    for (int i = 0; i<data_count; i++){
        myfile << data_array[i].rpid << "," << data_array[i].fname << "," << data_array[i].lname << "," << data_array[i].numDep << "," << data_array[i].income << "," << data_array[i].postal << endl;
        //printf("%d %s %s %d %f %d\n", data_array[i].rpid, data_array[i].fname, data_array[i].lname, data_array[i].numDep, data_array[i].income, data_array[i].postal);
    }
    myfile.close();
}

void handler1(int param){
    signal(SIGUSR1, handler1); /* re-setting signal */
    //printf("PID %d: caught SIGUSR1\n", getpid());
    sig1_count++;
}


void handler2(int param){
    signal(SIGUSR2, handler2); /* re-setting signal */
    //printf("PID %d: caught SIGUSR2\n", getpid());
    sig2_count++;
}

