// server
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT (2015)
#define BUF_LEN (128)

int main() {

  int sockfd, infd;
  struct sockaddr_in srv;
  socklen_t socklen;
  int i = 1;

  // socket (generate socket)
  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    puts("socket error");
    exit(0);
  }
  setsockopt(sockfd, SOL_SOCKET, 0, &i, sizeof(i));

  memset(&srv, 0, sizeof(srv));
  srv.sin_family = AF_INET;
  srv.sin_port = htons(PORT);

  // bind (name socket port number & IP address)
  socklen = sizeof(srv);
  if ((bind(sockfd, (struct sockaddr *)&srv, socklen)) < 0) {
    puts("bind error");
    exit(0);
  }

  // listen (wait for client connection)
  if ((listen(sockfd, 5)) < 0) {  // generally backlog size 5
    puts("listen error");
    exit(0);
  }
  puts("TCP/IP socket available");
  printf("port %d\n", ntohs(srv.sin_port));
  printf("addr %s\n", inet_ntoa(srv.sin_addr));

  // accept (accept first connection)
  if ((infd = accept(sockfd, (struct sockaddr *)&srv, &socklen)) >= 0)
    puts("new connection granted");

  char buf[BUF_LEN];
  int buf_len = BUF_LEN;
  int bufclear_roop;
  while (1) {
    while (1) {
    // ファイルディスクリプタから読み込む
      if ((buf_len = read(infd, buf, sizeof(buf))) != -1) {
	if (strcmp(buf, "quit") == 0) {
	  close(infd);
	  close(sockfd);
	  exit(0);
	}
	printf("%s\n", buf);
	break;
      }
    }
    // キーボードから入力を受け付ける
    printf("> ");
    fgets(buf, buf_len, stdin);
    buf[strlen(buf)-1] = '\0';
    // ファイルディスクリプタに書き込む
    if (write(infd, buf, sizeof(buf)) == -1)
      printf("server write error\n");
    // quitが入力されたらプログラムを終了する
    if (strcmp(buf, "quit") == 0) {
      close(infd);
      close(sockfd);
      exit(0);
    }
    // バッファの中身をクリアする
    for (bufclear_roop = 0; bufclear_roop < sizeof(buf); bufclear_roop++)
      buf[bufclear_roop] = '\0';
  }

  close(sockfd);

  //xfer_data(infd, fileno(stdout));

  return 0;
}
