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

int fgid;  // sigcatch関数で使用する
int fgid_flag = 0;  // fgidに値が格納されているかどうかを判定
void sigcatch(int sig);

int main() {
  char cmd[MAX_STR];
  char args[MAX_ARGS][MAX_STR];
  char *pargs[MAX_ARGS];
  pid_t pid, pid_exit;
  int jobsid[MAX_ARGS];
  int jobsidnum = 0;
  int status;

  // 割り込みが発生したらハンドラを実行する
  if (signal(SIGINT, sigcatch) == SIG_ERR)
    printf("シグナルエラー\n");

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
    int m, n, o;
    if (strcmp(cmd, "jobs") == 0) {
      for (m = 0; m < jobsidnum; m++) {
	waitpid(jobsid[m], &status, WNOHANG);
	if (waitpid(jobsid[m], &status, WNOHANG) == -1) { // bgプロセスが消えたら
	  for (n = m; n < jobsidnum; n++)
	    jobsid[m] = jobsid[m+1]; // そのプロセスIDを消して配列を埋める
	  jobsidnum--; // プロセスが一個消えるので数を一つ減らす
	}
      }
      for (o = 0; o < jobsidnum; o++)
	printf("PID[%d] = %d\n", o, jobsid[o]);
    }
    // fgでバックグラウンドの一つをフォアグラウンドに切り替える
    int p, q;
    if (strncmp(cmd, "fg", 2) == 0) {
      fgid = atoi(pargs[1]); // コマンドの第2引数(プロセスID)を数値として格納
      fgid_flag = 1;  // fgidに値が格納されるタイミングでflagを立てる
      pid_exit = waitpid(fgid, &status, 0); // bgをfgに
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
    if (flag == 0) {
      fgid = pid;
      fgid_flag = 1;  // fgidに値が格納されるタイミングでflagを立てる
      pid_exit = waitpid(pid, &status, 0);
    }
    else if (flag == 1) { // バックグラウンドなら
      pid_exit = waitpid(-1, &status, WNOHANG);
      jobsid[jobsidnum] = pid; // プロセスIDをjobsで表示するための配列に格納
      jobsidnum++; // jobsで管理するバックグラウンドプロセスの数を一つ増やす
      flag = 0;
    }

    // バックグラウンドプロセスが終了したらjobsからプロセスIDを消す
    for (m = 0; m < jobsidnum; m++) {
      waitpid(jobsid[m], &status, WNOHANG);
      if (waitpid(jobsid[m], &status, WNOHANG) == -1) { // bgプロセスが消えたら
	for (n = m; n < jobsidnum; n++)
	  jobsid[m] = jobsid[m+1]; // そのプロセスIDを消して配列を埋める
	jobsidnum--; // プロセスが一個消えるので数を一つ減らす
      }
    }

    // コマンドを入れる文字列のリセット
    int r, s;
    // cmd reset
    for (r = 0; r < MAX_STR; r++)
      cmd[r] = '\0';
    // args reset
    for (r = 0; r < MAX_ARGS; r++)
      for (s = 0; s < MAX_STR; s++)
	args[r][s] = '\0';
    // pargs reset
    for (r = 0; r < MAX_ARGS; r++)
      pargs[r] = NULL;

  }
}

void sigcatch(int sig) {
  printf("\nsignal number: %d\n", sig);
  // fgidのflagが1なら(fgidに一度でも値が格納されれば)fgプロセスをkill
  if (fgid_flag == 1) {
    kill(fgid, SIGINT);
    printf("process %d was killed\n", fgid);
  }
}
