#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>
#include<assert.h>
#include<errno.h>
#include<signal.h>
#include<sys/types.h>
#include<sys/wait.h>

#define MAX_STR 1024
#define MAX_ARGS 32

//void sigcatch(int sig);

int main() {
  char cmd[MAX_STR];
  char args[MAX_ARGS][MAX_STR];
  char *pargs[MAX_ARGS];
  pid_t pid, pid_exit;
  int jobsid[MAX_ARGS];
  int jobsidnum = 0;
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
	continue;
      }
      pargs[l] = args[l];
    }
    pargs[l] = NULL;

    // exitかquitでプログラムを終了する
    if (strcmp(cmd, "exit") == 0 || strcmp(cmd, "quit") == 0)
      exit(0);
    // jobsでバックグラウンドプロセスを一覧表示
    int o;
    if (strcmp(cmd, "jobs") == 0) {
      int m;
      for (m = 0; m < jobsidnum; m++) {
	waitpid(jobsid[m], &status, WNOHANG);
	if (waitpid(jobsid[m], &status, WNOHANG) == -1) {
	  jobsid[m] = jobsid[m+1];
	  jobsidnum--;
	}
      }
      for (o = 0; o < jobsidnum; o++)
	printf("pid = %d\n", jobsid[o]);
    }
    // fgでバックグラウンドの一つをフォアグラウンドに切り替える
    int fgid;
    int p, q;
    if (strncmp(cmd, "fg", 2) == 0) {
      fgid = atoi(pargs[1]); // コマンドの第2引数(プロセスID)を数値として格納
      pid_exit = waitpid(fgid, &status, 0); // bgをfgに
      printf("fg pid = %d\n", pid_exit);
      for (p = 0; p < jobsidnum; p++)
	if (fgid == jobsid[p]) // jobs一覧のプロセスIDとfgにしたものが同じなら
	  for (q = p; q < jobsidnum; q++)
	    jobsid[q] = jobsid[q+1]; // そのプロセスIDを消して配列を埋める
      jobsidnum--; // プロセスが一個消えるので数を一つ減らす
    }

    // コマンド実行
    if ((pid = fork()) == -1) {
      perror("fork");
      exit(1);
    }
    if (pid == 0) {
      execv(args, pargs);
      exit(0);
    }

    // flag == 0(&なし)なら親が子を待つ、flag == 1(&あり)なら親は子を待たない
    //int m;
    if (flag == 0) {
      pid_exit = waitpid(pid, &status, 0);
    }
    else if (flag == 1) { // バックグラウンドなら
      pid_exit = waitpid(-1, &status, WNOHANG);
      //for (m = 0; m < jobsidnum; m++) {
      //if (waitpid(pid_exit, &status, WNOHANG) == -1) {
      //  jobsid[m] = jobsid[m+1];
      //  jobsidnum--;
      //}
      //}
      jobsid[jobsidnum] = pid; // プロセスIDをjobsで表示するための配列に格納
      jobsidnum++; // jobsで管理するバックグラウンドプロセスの数を一つ増やす
      flag = 0;
    }
    /*
      int abc;
      for (m = 0; m < jobsidnum; m++) {
      if ((abc = waitpid(pid_exit, &status, WNOHANG)) == -1) {
      printf("bac = %d\n", abc);
      jobsid[m] = jobsid[m+1];
      jobsidnum--;
      }
      }*/

    //printf("exit status=%d for pid=%d\n", status, pid_exit);

    //if (signal(SIGCHLD, sigcatch) == SIG_ERR)
    //  printf("error!!!\n");

    // pargsのリセット
    int n;
    for (n = 0; n < 256; n++) {
      pargs[n] = NULL;
    }

  }
}

/*
void sigcatch(int sig) {
  printf("catch signal %d\n", sig);
  }*/
