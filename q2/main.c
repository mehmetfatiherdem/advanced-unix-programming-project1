#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define BUF_SIZE 100

int addLastTwoNumbers(char* buf){
    int len = strlen(buf);
    if(len == 3){
        return 2;
    }
    int i = len - 1;
    while(buf[i] != ' '){
        i--;
    }
    int last = atoi(&buf[i+1]);
    i--;
    while(buf[i] != ' '){
        i--;
    }
    int secondLast = atoi(&buf[i+1]);
    return last + secondLast;
}

int getLastNumber(char* buf){
    int i = strlen(buf)-1;
    while(buf[i] != ' '){
        i--;
    }
    return atoi(&buf[i+1]);
}

char* appendToBuf(char* buf, int num){
    char temp[BUF_SIZE];
    sprintf(temp, "%d", num);
    strcat(buf, " ");
    strcat(buf, temp);
    return buf;

}

int main(int argc, char *argv[]){

    char buf[BUF_SIZE];

    ssize_t cnt;

    int childpid;

    int numOfProcesses = atoi(argv[1]);

    int pipeArray[numOfProcesses][2];

    sprintf(buf, "1 1");

    cnt = strlen(buf);

    printf("Process %d: %d %d\n", getpid(), 1, 1);

    for(int i=0; i<numOfProcesses; i++){
        if(pipe(pipeArray[i]) == -1){
            printf("Error creating pipe %d\n", i);
            return 1;
        }
    }


    if(write(pipeArray[0][1], buf, cnt) != cnt){
        printf("Error writing to pipe 0\n");
        return 3;
    }
    

    int i;

    for(i=0; i<numOfProcesses-1; i++){
        
        if((childpid = fork()) == -1){
            printf("Error creating child process %d\n", i);
            return 4;
        }

        if(childpid > 0){
            if(close(pipeArray[i][0]) == -1){
                printf("Error closing read end of pipe 0\n");
                return 2;
            }
            
        }

        else if(childpid == 0){

            if(close(pipeArray[i][1]) == -1){
                printf("Error closing write end of pipe %d\n", i);
                return 5;

            }

            while((cnt = read(pipeArray[i][0], buf, BUF_SIZE)) > 0){

                int currFibonacci = addLastTwoNumbers(buf);

                // add curr fibonacci to the buffer
                appendToBuf(buf, currFibonacci);

                cnt = strlen(buf);

                printf("Process %d: %s\n", getpid(), buf);

                if(i==2){
                    if(close(pipeArray[i+1][0]) == -1){
                        printf("Error closing read end of pipe %d\n", i+1);
                        return 8;
                    }
                }

                if(write(pipeArray[i+1][1], buf, cnt) != cnt){
                    printf("Error writing to pipe %d\n", i+1);
                    return 6;

                   
                }

                if(close(pipeArray[i][0]) == -1){
                    printf("Error closing read end of pipe %d\n", i);
                    return 8;
                }

            }

            return 42;
        }
    }

    // wait for all the children to finish

    while(wait(NULL) > 0);
        
    if(close(pipeArray[numOfProcesses-1][1]) == -1){
        printf("Error closing write end of pipe %d\n", numOfProcesses-1);
        return 9;
    }
        
    while((cnt = read(pipeArray[numOfProcesses-1][0], buf, BUF_SIZE)) > 0){
        printf("Process %d: Fibonacci (%d) : %d\n", getpid(), numOfProcesses, getLastNumber(buf));
        if(close(pipeArray[numOfProcesses-1][0]) == -1){
            printf("Error closing read end of pipe %d\n", numOfProcesses-1);
            return 10;
        }
    }

    if(cnt == -1) return 100;
    
    return 0;
}

