// client
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT (8080)
#define BUF_LEN (128)

int main(int argc, char *argv[]) {

  int sockfd, infd;
  struct sockaddr_in cli;
  socklen_t socklen;

  if (argc != 2) {
    puts("USAGE: wrnet <ip address>");
    exit(0);
  }

  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    puts("socket error");
    exit(0);
  }

  memset(&cli, 0, sizeof(cli));
  cli.sin_family = AF_INET;
  cli.sin_port = htons(PORT);
  if (!(inet_aton(argv[1], &cli.sin_addr))) {
    puts("inet_aton error");
    exit(0);
  }

  socklen = sizeof(cli);
  if (infd = connect(sockfd, (struct sockaddr *)&cli, socklen)) {
    puts("connect error");
    exit(0);
  }
  puts("connected to TCP/IP socket");
  printf("port %d\n", ntohs(cli.sin_port));
  printf("addr %d\n", inet_ntoa(cli.sin_addr));

  char buf[BUF_LEN];
  int buf_len = BUF_LEN;
  int bufclear_roop;
  while (1) {
    // キーボードからの入力を受け付ける
    printf("> ");
    fgets(buf, buf_len, stdin);
    buf[strlen(buf)-1] = '\0';
    // ファイルディスクリプタに書き込む
    if (write(sockfd, buf, sizeof(buf)) == -1)
      printf("client write error\n");
    // quitが入力されたらプログラムを終了する
    if (strcmp(buf, "quit") == 0) {
      close(sockfd);
      exit(0);
    }
    // バッファの中身をクリアする
    for (bufclear_roop = 0; bufclear_roop < sizeof(buf); bufclear_roop++)
      buf[bufclear_roop] = '\0';
    while (1) {
    // ファイルディスクリプタから読み込む
      if ((buf_len = read(sockfd, buf, sizeof(buf))) != -1) {
	if (strcmp(buf, "quit") == 0) {
	  close(sockfd);
	  exit(0);
	}
	printf("%s\n", buf);
	break;
      }
    }
  }

  close(sockfd);

  return 0;
}
