#include<stdio.h>
#include<stdlib.h>
#include<string.h>

int main() {

  int p1, p2, p3;
  char *args[] = {""};
  // char *arg[] = {NULL};
  char command[32];
  pid_t pid_exit;
  int status;

  while (1) {
    printf("input command: ");
    scanf("%s", command);

    if ((p1 = fork()) == -1) {
      perror("fork error");
      exit(1);
    }
    
    if (p1 == 0) {
      // child
      p2 = getpid();
      printf("child: %d %d\n", p1, p2);
      execv(command, args);
      exit(0);
    }
    else {
      // parent
      p2 = getpid();
      printf("parent: %d %d\n", p1, p2);
      pid_exit = wait(&status);
      if (strcmp(command, "exit") == 0 || strcmp(command, "quit") == 0)
	exit(0);
    }
  }

  exit(0);
}
