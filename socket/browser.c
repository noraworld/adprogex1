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

  // ブラウザ情報を送信する
  if (write(sockfd, "browser infomation", sizeof("browser infomation")) < 0)
    printf("browser write error\n");
  char buf[BUF_LEN];
  int len;
  while (1)
    if ((len = read(sockfd, buf, BUF_LEN)) > 0)
      printf("server write = %s\n", buf);

  close(sockfd);

  return 0;
}
