#include <stdio.h> 
#include <iostream>
#include <sys/types.h> 
#include <unistd.h> 
#include <signal.h>
#include <stdlib.h>
#include <sys/wait.h>
using namespace std;

#define READ_END 0
#define WRITE_END 1

#include <fstream>
#include <sstream>
#include <string>
#include <string.h>

#include <typeinfo>

// #include "bubble.h"

int readCSV(const char* inputFile, struct DATA **dataStruct, int* data_count, int*start);
void showData(struct DATA *dataStruct, int count);
void storeData(struct DATA *dataStruct, DATA dat[],int count);
void insertionSort(DATA data_array[], int data_count, string arg, string order);

struct DATA{ 
    int rpid; // individual ID
    char fname[15]; // first name
    char lname[15]; // last name
    int numDep; // # of dependents
    float income; // income
    int postal; // postal code
};


int main(int argc, char *argv[]){

    DATA *data = NULL;
    int start = stoi(argv[6]);
    int end = stoi(argv[7]); // SEE IF YOU SHOULD DELETE THIS ALSO FROM MYHIE.CPP
    int data_count = end - start;
    string arg;

    // Checking for attribute to sort by
    if(strncmp(argv[4],"0",1)==0) arg = "rpid";
    if(strncmp(argv[4],"3",1)==0) arg = "numDep";
    if(strncmp(argv[4],"4",1)==0) arg = "income";
    if(strncmp(argv[4],"5",1)==0) arg = "postal";

    if(readCSV(argv[3], &data, &data_count, &start)){
        DATA dat[data_count];
        //showData(data, data_count);
        storeData(data, dat, data_count);
        insertionSort(dat, data_count, arg, argv[5]);
    }
    else printf("NO WAY");

    return 0;
}


// Read data records from CSV and write into the array declared by DATA
int readCSV(const char* inputFile, struct DATA **dataStruct, int* data_count, int* start){
    //Citation: https://stackoverflow.com/questions/3501338/c-read-file-line-by-line
    //Generate a file pointer

    // 222334444 Kenan Barbieri 3 120000 20742

    // 0 1 2 3 4 5 6 7 8 9
    // [0 2 3 10]
    // 0:2 -> 0 1 DATA_COUNT = 2-0 = 2
    // 2:3 -> 2 DATA_COUNT = 3-2 = 1
    // 3:10 -> 3 4 5 6 7 8 9 DATA_COUNT 10-3 = 7

    FILE * fp = fopen(inputFile, "r");

    char * line = NULL;
    size_t len = 0;

    // This code block is to split the data
    //  according to the range that was passed
    int old_pos = ftell(fp); // save the initial position in file
    if (*start != 0){ 
        // if file should read from somewhere in the middle of csv
        // get the size of jump you need to implement to get there
        int jump = old_pos + (*start)*52;
        // set the file pointer to that position
        fseek(fp, jump, SEEK_SET);
    } 
    
    if (fp != NULL){
        if(*data_count > 0){
            // allocating memory space
            struct DATA *data = (struct DATA*)malloc(sizeof(struct DATA)*(*data_count));
            *dataStruct = data;
            int counter = 0;
            while((getline(&line, &len, fp)) != -1){
                if (counter < *data_count) {
                    //Extract the first token - the rid
                    char * token = strtok(line, " ");
                    data -> rpid = stoi(token);
                    
                    //Extract the second token - the first name
                    token = strtok(NULL, " ");
                    strcpy(data -> fname, token);
                    // data -> fname = token;
                    
                    //Extract the third token - the last name
                    token = strtok(NULL, " ");
                    strcpy(data -> lname, token);
                    // data -> lname = token;

                    //Extract the forth token - the number of dependents
                    token = strtok(NULL, " ");
                    data -> numDep = stoi(token);

                    //Extract the fifth token - the income
                    token = strtok(NULL, " ");
                    data -> income = stof(token);

                    //Extract the sixth token - the postal
                    token = strtok(NULL, " ");
                    data -> postal = stoi(token);

                    data++; // next storing location
                    counter++;
                }
            }
        }
        return 1;
    }
    //close the pointer
    fclose(fp);
    //free the memory
    if (line) free(line);

    return 0;
} 

void storeData(struct DATA *dataStruct, DATA dat[], int count){
    for (int i = 0; i<count; i++, dataStruct++){
        dat[i].rpid = dataStruct -> rpid;
        strcpy(dat[i].fname, dataStruct -> fname);
        strcpy(dat[i].lname, dataStruct -> lname);
        dat[i].numDep = dataStruct -> numDep;
        dat[i].income = dataStruct -> income;
        dat[i].postal = dataStruct -> postal;
        //printf("%d %s %s %d %f %d\n", dat[i].rpid, dat[i].fname, dat[i].lname, dat[i].numDep, dat[i].income, dat[i].postal);
    }
}

// reference: https://www.geeksforgeeks.org/insertion-sort/
void insertionSort(DATA data_array[], int data_count, string arg, string order){ 
    int i, j, key;
    DATA temp;
    for (i = 1; i < data_count; i++){ 
        if (strncmp(arg.c_str(),"rpid",4)==0){
            // Ascending order
            key = data_array[i].rpid; 
            j = i - 1;
            if (strncmp(order.c_str(),"a",1)==0){
                while (j >= 0 && data_array[j].rpid > key){ 
                    j = j - 1; 
                } 
                data_array[j + 1].rpid = key;
            }
            else {
                while (j <= 0 && data_array[j].rpid < key){ 
                    j = j - 1; 
                } 
                data_array[j + 1].rpid = key; 
            }
        }
        if (strncmp(arg.c_str(),"numDep",6)==0){
            // Ascending order
            key = data_array[i].numDep; 
            j = i - 1;
            if (strncmp(order.c_str(),"a",1)==0){
                while (j >= 0 && data_array[j].numDep > key){ 
                    j = j - 1; 
                } 
                data_array[j + 1].numDep = key; 
            }
            else{
                while (j <= 0 && data_array[j].numDep < key){ 
                    j = j - 1; 
                } 
                data_array[j + 1].numDep = key; 
            }
        }
        if (strncmp(arg.c_str(),"income",6)==0){
            // Ascending order
            key = data_array[i].income; 
            j = i - 1;
            if (strncmp(order.c_str(),"a",1)==0){
                while (j >= 0 && data_array[j].income > key){ 
                    j = j - 1; 
                } 
                data_array[j + 1].income = key; 
            }
            else {
                while (j >= 0 && data_array[j].income > key){ 
                    j = j - 1; 
                } 
                data_array[j + 1].income = key; 
            }
        }
        if (strncmp(arg.c_str(),"postal",6)==0){
            // Ascending order
            key = data_array[i].postal; 
            j = i - 1;
            if (strncmp(order.c_str(),"a",1)==0){
                while (j >= 0 && data_array[j].postal > key){ 
                    j = j - 1; 
                } 
                data_array[j + 1].postal = key; 
            }
            else {
                while (j >= 0 && data_array[j].postal > key){ 
                    j = j - 1; 
                } 
                data_array[j + 1].postal = key; 
            }
        }
    } 

    for (int i = 0; i<data_count; i++){
        string s = to_string(data_array[i].rpid);
        char const* rpid = s.c_str();
        string at = to_string(data_array[i].numDep);
        char const* numdep = at.c_str();
        string st = to_string(data_array[i].income);
        char const* inc = st.c_str();
        string e = to_string(data_array[i].postal);
        char const* post = e.c_str();
        string tt = data_array[i].fname;
        char const* first = tt.c_str();
        string oo = data_array[i].lname;
        char const* last = oo.c_str();
        printf("%s %s %s %s %s %s\n", rpid, first, last, numdep, inc, post);
    }
}