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

  while (1) {
    printf("> ");
    if (fgets(cmd, MAX_STR, stdin) == NULL)
      exit(0);
    cmd[strlen(cmd)-1] = '\0';

    // cmdの内容(コマンドラインで入力した内容)を列ごとにコピーする
    int i = 0;
    int j = 0;
    int k = 0;
    while (cmd[i] != '\0') {
      while (cmd[i] != ' ' && cmd[i] != '\0') {
	args[j][k] = cmd[i];
	//printf("agrs[%d][%d] = %c\n", j, k, args[j][k]);
	//printf("args[%d] = %s\n", j, args[j]);
	i++;
	k++;
      }
      args[j][i] = '\0';
      i++;
      j++;
      k = 0;
      //printf("j = %d\n", j);
    }

    // argsの内容をpargsにコピーする
    int l;
    for (l = 0; l < j; l++) {
      //printf("args[%d] = %s\n", l, args[l]);
      pargs[l] = args[l];
      //printf("pargs[%d] = %s\n", l, pargs[l]);
    }
    //printf("l = %d\n", l);
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
      execv(args, pargs);
      exit(0);
    }

    // フォアグラウンド処理とプロセスIDの表示
    pid_exit = wait(&status);
    printf("exit status=%d for pid=%d\n", status, pid_exit);
  }
}
