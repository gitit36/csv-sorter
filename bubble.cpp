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

#include "bubble.h"

void bubble::handle(int rpid, char* range_flag, char* inputFile, int attriNum, char* order){
    printf("rpid: %d\n", rpid);
    printf("range_flag: %s\n", range_flag);
    printf("inputFile: %s\n", inputFile);
    printf("attriNum: %d\n", attriNum);
    printf("order: %s\n", order);
}

/* A utility function to print array of size n */
void bubble::printArray(int arr[], int n) 
{ 
    for (int i = 0; i < n; ++i) 
        cout << arr[i] << " "; 
    cout << "\n"; 
} 
