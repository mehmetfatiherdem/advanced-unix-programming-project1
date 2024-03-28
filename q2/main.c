#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_DIGITS 10

int main(int argc, char *argv[]) {
  if (argc != 2) {
    fprintf(stderr, "Usage: %s <number_of_processes>\n", argv[0]);
    exit(1);
  }

  int num_processes = atoi(argv[1]);

  int pipes[2 * num_processes]; // Array of pipes for communication
  int i, status;

  // Create pipes
  for (i = 0; i < num_processes; ++i) {
    if (pipe(pipes + 2 * i) == -1) {
      perror("pipe");
      exit(1);
    }
  }

  // Create child processes
  for (i = 0; i < num_processes; ++i) {
    pid_t pid = fork();
    if (pid == -1) {
      perror("fork");
      exit(1);
    } else if (pid == 0) { // Child process
      char prev1[MAX_DIGITS], prev2[MAX_DIGITS];
      int n1, n2;

      if (i == 0) { // First child (parent process writes initial values)
        close(pipes[1]); // Close write end of first pipe
        read(STDIN_FILENO, prev1, sizeof(prev1));
        read(STDIN_FILENO, prev2, sizeof(prev2));
        n1 = atoi(prev1);
        n2 = atoi(prev2);
        printf("Process %d: %d %d\n", getpid(), n1, n2);
      } else { // Other child processes
        close(pipes[2 * i - 1]); // Close read end of previous pipe
        read(pipes[2 * i - 2], prev1, sizeof(prev1));
        read(pipes[2 * i - 2], prev2, sizeof(prev2));
        n1 = atoi(prev1);
        n2 = atoi(prev2);
      }

      int next = n1 + n2;
      close(pipes[2 * i]);  // Close read end of current pipe
      if (i == num_processes - 1) { // Last child, write to stdout
        printf("Process %d: %d %d %d\n", getpid(), n1, n2, next);
      } else { // Write to next pipe
        sprintf(prev1, "%d", n2);
        sprintf(prev2, "%d", next);
        write(pipes[2 * i + 1], prev1, sizeof(prev1));
        write(pipes[2 * i + 1], prev2, sizeof(prev2));
      }

      fprintf(stderr, "Process %d: Fibonacci (%d) : %d\n", getpid(), num_processes, next);
      exit(0);
    }
  }

  // Parent process
  close(pipes[1]); // Close write end of first pipe (after writing)
  char num1[MAX_DIGITS] = "1";
  char num2[MAX_DIGITS] = "1";
  write(pipes[0], num1, sizeof(num1)); // Write initial values to first pipe
  write(pipes[0], num2, sizeof(num2));
  
  // Wait for all child processes to finish
  for (i = 0; i < num_processes; ++i) {
    wait(&status);
  }

  // Read final result from last child's pipe (written to stdout by last child)
  read(pipes[1], num1, sizeof(num1));

  fprintf(stderr, "Parent: Fibonacci (%d) : %s\n", num_processes, num1);

  // Close remaining pipes
  for (i = 0; i < 2 * num_processes; ++i) {
    close(pipes[i]);
  }

  return 0;
}
