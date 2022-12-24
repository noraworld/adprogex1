#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>

#define MAX_STR 1024
#define MAX_ARGS 32

int main() {
  char cmd[MAX_STR];
  char args[MAX_ARGS][MAX_STR];
  char *pargs[MAX_ARGS];
  pid_t pid, pid_exit;
  int status;

  // コマンドを入力する
  while (1) {
    printf("> ");
    if (fgets(cmd, MAX_STR, stdin) == NULL)
      exit(0);
    cmd[strlen(cmd)-1] = '\0';

    // cmdの内容(コマンドラインで入力した内容)を列ごとにargsにコピーする
    int i = 0;
    int j = 0;
    int k = 0;
    while (cmd[i] != '\0') {
      while (cmd[i] != ' ' && cmd[i] != '\0') {
	args[j][k] = cmd[i];
	i++;
	k++;
      }
      args[j][i] = '\0';
      i++;
      j++;
      k = 0;
    }
    // argsの内容をpargsにコピーする
    int l;
    int flag = 0;
    for (l = 0; l < j; l++) {
      if (strcmp(args[l], "&") == 0) {
	flag = 1;
	//printf("flag = %d\n", flag);
	continue;
      }
      pargs[l] = args[l];
    }
    pargs[l] = NULL;

    // exitかquitでプログラムを終了する
    if (strcmp(cmd, "exit") == 0 || strcmp(cmd, "quit") == 0)
      exit(0);

    // コマンド実行
    if ((pid = fork()) == -1) {
      perror("fork");
      exit(1);
    }
    if (pid == 0) {
      //abc = getpid();
      execv(args, pargs);
      exit(0);
    }

    // flag == 0(&なし)なら親が子を待つ、flag == 1(&あり)なら親は子を待たない
    int m = 0;
    if (flag == 0) {
      //printf("if flag = %d\n", flag);
      pid_exit = waitpid(-1, &status, 0);
    }
    else if (flag == 1) {
      //printf("elsif flag = %d\n", flag);
      pid_exit = waitpid(-1, &status, WNOHANG);
      flag = 0;
    }

    printf("exit status=%d for pid=%d\n", status, pid_exit);

  }
}
