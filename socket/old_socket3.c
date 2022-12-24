#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT (8080)
#define BUF_LEN (1024)

int main() {

  int sockfd, infd, on, ret;
  struct sockaddr_in srv;
  socklen_t socklen;

  // socket (generate socket)
  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    puts("socket error");
    exit(0);
  }
  on = 1;
  ret = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

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

  FILE *fp;
  char str[BUF_LEN];
  /*
  if ((fp = fopen("./htdocs/exist/index.html", "r")) == NULL) {
    printf("ファイルをオープンできません\n");
    exit(0);
    }*/
  /*
    while (fgets(str, sizeof(str), fp) != NULL) {
    printf("%s", str);
    }
    fclose(fp);
  */
  //int bufclear_roop;
  //while (1) {
  while (1) {
    // ファイルディスクリプタから読み込む
    if ((buf_len = read(infd, buf, sizeof(buf))) != -1) {
      printf("%s\n", buf);
      break;
    }
  }
  if (write(infd, "HTTP/1.0 200 OK\r\n", 128) >= 0)
    printf("サーバは正しく書き込みをしている\n");
  //write(infd, "HTTP/1.0 404 Not Found\r\n", sizeof("HTTP/1.0 404 Not Found\r\n"));
  if (write(infd, "\r\n", 128) >= 0)
    printf("サーバは正しく書き込みをしている\n");
  if (write(infd, "HTML It works!!", 128) >= 0)
    printf("サーバは正しく書き込みをしている\n");
  /*
    FILE *fp;
    if ((fp = fopen("./htdocs/index.html", "r")) == NULL) {
    printf("ファイルをオープンできません\n");
    exit(0);
    }
    while (fgets(buf, sizeof(buf)-1, fp) != NULL) {
    printf("%s", buf);
    }*/

  // ファイルディスクリプタに書き込む
  /*while (fgets(str, sizeof(str), fp) != NULL) {
    printf("%s", str);
    if (write(infd, str, sizeof(str)) < 0)
    printf("server write error\n");
    }*/


  // バッファの中身をクリアする
  //for (bufclear_roop = 0; bufclear_roop < sizeof(buf); bufclear_roop++)
  //  buf[bufclear_roop] = '\0';
  //}

  close(infd);
  close(sockfd);

  return 0;
}
