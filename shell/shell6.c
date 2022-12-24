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

int fg_pid;              // sigcatch関数で使用する
int fg_pid_flag = 0;     // fg_pidに値が一度でも格納されているかどうかを判定
void sigcatch(int sig);  // シグナルハンドラ

int main() {
  char cmd[MAX_STR];
  char args[MAX_ARGS][MAX_STR];
  char *pargs[MAX_ARGS];
  pid_t pid, pid_exit;
  int jobs_pid[MAX_ARGS];  // jobsコマンドで一覧表示するプロセスid
  int jobs_pid_num = 0;    // jobsコマンドで表示するプロセスidの数
  int status;
  char sep_path[MAX_ARGS][MAX_STR];  // $PATHを一つ一つ分解したものを格納
  int and_flag = 0;
  int args_i, args_j, args_k;
  int pargs_i;
  int path_i, path_j, path_k, path_temp;
  int exec_i;
  int cmd_i, cmd_j;

  // 割り込みが発生したらハンドラを実行する
  if (signal(SIGINT, sigcatch) == SIG_ERR)
    printf("A signal error occurred\n");

  // 環境変数を取得する
  char *path;
  path = getenv("PATH");

  // "exit"か"quit"が入力されるまでループ
  while (1) {

    // コマンドを入力する
    printf("> ");
    if (fgets(cmd, MAX_STR, stdin) == NULL)
      exit(0);
    cmd[strlen(cmd)-1] = '\0';

    // cmdの内容(コマンドラインで入力した内容)を列ごとにargsにコピーする
    args_i = 0;
    args_j = 0;
    args_k = 0;
    while (cmd[args_i] != '\0') {
      while (cmd[args_i] != ' ' && cmd[args_i] != '\0') {
	args[args_j][args_k] = cmd[args_i];
	args_i++;
	args_k++;
      }
      args[args_j][args_k] = '\0';
      args_i++;
      args_j++;
      args_k = 0;
    }

    // argsの内容をpargsにコピーする
    for (pargs_i = 0; pargs_i < args_j; pargs_i++) {
      if (strcmp(args[pargs_i], "&") == 0) {  // &が含まれていたら
	and_flag = 1;  // and_flagを立てる
	continue;
      }
      pargs[pargs_i] = args[pargs_i];
    }
    pargs[pargs_i] = NULL;

    // 取得した環境変数を一つ一つ切り分ける
    for (path_i = 0; path_i < MAX_ARGS; path_i++)
      for (path_j = 0; path_j < MAX_STR; path_j++)
	sep_path[path_i][path_j] = '\0';  // sep_pathの初期化
    path_i = 0;
    path_j = 0;
    path_k = 0;
    path_temp = 0;
    while (path[path_k] != '\0') {  // $PATHが末尾になるまで
      if (path[path_k] == ':') {    // 一つのパスの終端なら
	sep_path[path_i][path_j] = '/';  // 終端に / をつけて
	path_j++;
	while (args[0][path_temp] != '\0') {  // 第一引数の末尾になるまで
	  sep_path[path_i][path_j] = args[0][path_temp];
	  // ↑第一引数(サーチパス名)を末尾の / 以下に追加
	  // ex: /usr/bin を /usr/bin/emacs にする
	  path_temp++;
	  path_j++;
	}
	sep_path[path_i][path_j] = '\0';
	path_i++;
	path_j = 0;
	path_k++;
	path_temp = 0;
      }
      sep_path[path_i][path_j] = path[path_k];  // $PATHの値をsep_pathに格納
      path_j++;
      path_k++;
    }

    // exitかquitでプログラムを終了する
    if (strcmp(cmd, "exit") == 0 || strcmp(cmd, "quit") == 0)
      exit(0);

    // jobsでバックグラウンドプロセスを一覧表示
    //int m, n, o;
    int delete_pid_i, delete_pid_j;
    int jobs_i;
    if (strcmp(cmd, "jobs") == 0) {
      for (delete_pid_i = 0; delete_pid_i < jobs_pid_num; delete_pid_i++) {
	waitpid(jobs_pid[delete_pid_i], &status, WNOHANG);
	if (waitpid(jobs_pid[delete_pid_i], &status, WNOHANG) == -1) {
	  // ↑bgプロセスが消えたら
	  for (delete_pid_j = delete_pid_i; delete_pid_j < jobs_pid_num; delete_pid_j++)
	    jobs_pid[delete_pid_j] = jobs_pid[delete_pid_j+1];
	  // ↑そのプロセスIDを消して配列を埋める
	  jobs_pid_num--; // プロセスが一個消えるので数を一つ減らす
	}
      }
      // 実行中のプロセスid一覧の表示
      for (jobs_i = 0; jobs_i < jobs_pid_num; jobs_i++)
	printf("pid[%d] = %d\n", jobs_i, jobs_pid[jobs_i]);
    }
    // fgでバックグラウンドの一つをフォアグラウンドに切り替える
    int p, q;
    if (strncmp(cmd, "fg", 2) == 0) {
      fg_pid = atoi(pargs[1]); // コマンドの第2引数(プロセスID)を数値として格納
      fg_pid_flag = 1;  // fg_pidに値が格納されるタイミングでflagを立てる
      pid_exit = waitpid(fg_pid, &status, 0); // bgをfgに
      for (p = 0; p < jobs_pid_num; p++)
	if (fg_pid == jobs_pid[p]) // jobs一覧のプロセスIDとfgにしたものが同じなら
	  for (q = p; q < jobs_pid_num; q++)
	    jobs_pid[q] = jobs_pid[q+1]; // そのプロセスIDを消して配列を埋める
      jobs_pid_num--; // プロセスが一個消えるので数を一つ減らす
    }

    // コマンド実行
    if ((pid = fork()) == -1) {
      perror("fork");
      exit(1);
    }
    if (pid == 0) {
      execv(args, pargs);  // フルパスで入力しても実行できるようにする
      // $PATHを一つずつ順番に実行する
      // ex: /bin/emacs が実行できなかったら /usr/bin/emacs を実行する
      for (exec_i = 0; exec_i < (path_i + 1); exec_i++) {
	execv(sep_path[exec_i], pargs);
      }
      exit(0);
    }

    // and_flag == 0(&なし)なら親が子を待つ、and_flag == 1(&あり)なら親は子を待たない
    if (and_flag == 0) { // フォアグラウンドなら
      fg_pid = pid;  // &なしはfgプロセスなのでfg_pidにプロセスidを格納
      fg_pid_flag = 1;  // fg_pidに値が格納されるタイミングでand_flagを立てる
      pid_exit = waitpid(pid, &status, 0);
    }
    else if (and_flag == 1) { // バックグラウンドなら
      pid_exit = waitpid(-1, &status, WNOHANG);
      jobs_pid[jobs_pid_num] = pid; // プロセスIDをjobsで表示するための配列に格納
      jobs_pid_num++; // jobsで管理するバックグラウンドプロセスの数を一つ増やす
      and_flag = 0;
    }

    // バックグラウンドプロセスが終了したらjobsからプロセスIDを消す
    for (delete_pid_i = 0; delete_pid_i < jobs_pid_num; delete_pid_i++) {
      waitpid(jobs_pid[delete_pid_i], &status, WNOHANG);
      if (waitpid(jobs_pid[delete_pid_i], &status, WNOHANG) == -1) { // bgプロセスが消えたら
	for (delete_pid_j = delete_pid_i; delete_pid_j < jobs_pid_num; delete_pid_j++)
	  jobs_pid[delete_pid_j] = jobs_pid[delete_pid_j+1];
	// ↑そのプロセスIDを消して配列を埋める
	jobs_pid_num--; // プロセスが一個消えるので数を一つ減らす
      }
    }

    // コマンドを入れる文字列のリセット
    for (cmd_i = 0; cmd_i < MAX_STR; cmd_i++)
      cmd[cmd_i] = '\0';
    for (cmd_i = 0; cmd_i < MAX_ARGS; cmd_i++)
      for (cmd_j = 0; cmd_j < MAX_STR; cmd_j++)
	args[cmd_i][cmd_j] = '\0';
    for (cmd_i = 0; cmd_i < MAX_ARGS; cmd_i++)
      pargs[cmd_i] = NULL;

  }
}

void sigcatch(int sig) {
  printf("\nSignal number: %d\n", sig);
  // fg_pid_flagが1なら(fg_pidに一度でも値が格納されれば)fgプロセスをkill
  if (fg_pid_flag == 1) {
    kill(fg_pid, SIGINT);  // fgプロセスにシグナル(SIGINT)を送る
    printf("Process %d was killed\n", fg_pid);  // SIGINTを送られたプロセスのid
  }
}
