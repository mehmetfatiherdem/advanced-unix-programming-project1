#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define BUF_SIZE 100

int addLastTwoNumbers(char* buf){
    printf("buf in addlasttwonumbers: %s\n", buf);
    int lastNum = atoi(&buf[strlen(buf)-1]);
    printf("lastNum: %d\n", lastNum);
    int secondLastNum = atoi(&buf[strlen(buf)-3]);
    printf("secondLastNum: %d\n", secondLastNum);
    return lastNum + secondLastNum;
}

char getLastChar(char *buf){
    int i = 0;
    int len = strlen(buf);
    if(len > 0){
        return buf[len-1];
    }
    return '\0';
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

    for(int i=0; i<numOfProcesses-1; i++){
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
            if(i < 3){
                if(close(pipeArray[i][0]) == -1){
                    printf("Error closing read end of pipe 0\n");
                    return 2;
                }
                printf("process %d closed read end of pipe %d\n", getpid(), i);
            }
            
        }

        else if(childpid == 0){

            //printf("this is child process %d\n", getpid());
            int closeRes = close(pipeArray[i][1]);
            printf("child %d closedRes %d is %d\n", getpid(), i, closeRes);

            if(closeRes == -1){
                printf("Error closing write end of pipe %d\n", i);
                return 5;

            }
            printf("child %d closed write end of pipe %d\n", getpid(), i);

            while((cnt = read(pipeArray[i][0], buf, BUF_SIZE)) > 0){

                int currFibonacci = addLastTwoNumbers(buf);

                printf("child %d  buf: %s bufLen: %d\n", getpid(), buf, (int)strlen(buf));

                printf("child %d currFibonacci: %d\n", getpid(), currFibonacci);

                // add curr fibonacci to the buffer
                appendToBuf(buf, currFibonacci);

                printf("child %d buf: %s\n", getpid(), buf);

                printf("Process %d: %s\n", getpid(), buf);

                int writeRes = write(pipeArray[i+1][1], buf, cnt);
                printf("child %d writeRes %d and cnt is %d\n", getpid(), writeRes, (int)cnt);

                if(write(pipeArray[i+1][1], buf, cnt) != cnt){
                    printf("Error writing to pipe %d\n", i+1);
                    return 6;

                   
                }
                 printf("child %d wrote to pipe %d\n", getpid(), i+1);

            }

            if(cnt == -1) {
                printf("Error reading from pipe %d\n", i);
                return 7;
            }


            break;
        }
    }
    


    // wait for all the children to finish

    while(wait(NULL) > 0);
    
    if(close(pipeArray[numOfProcesses-1][1]) == -1){
        printf("Error closing write end of pipe %d\n", numOfProcesses-1);
        return 9;
    }
    
    while((cnt = read(pipeArray[numOfProcesses-1][0], buf, BUF_SIZE)) > 0){
        printf("Process %d: Fibonacci (%d) : %d\n", getpid(), numOfProcesses, getLastChar(buf));
    }

    if(cnt == -1) return 100;
    
    return 0;
}

