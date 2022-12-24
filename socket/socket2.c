// server
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT (8080)    // port number
#define BUF_LEN (1024) // size of buf[]
#define EOT (4)        // end of transmission

int main() {

  int sockfd, infd, ret, on;
  struct sockaddr_in srv;
  socklen_t socklen;

  // socket (generate socket)
  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    puts("socket error");
    exit(0);
  }
  on = 1;
  // setsockopt() の第3引数を SO_REUSEADDR にすると
  // 2回目以降の bind error (address already in use) を回避できる
  // (TCP ソケットの TIME_WAIT 状態を回避できる)
  // http://www.ibm.com/developerworks/jp/linux/library/l-sockpit/
  ret = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

  memset(&srv, 0, sizeof(srv));
  srv.sin_family = AF_INET;
  srv.sin_port = htons(PORT);

  // bind (name socket port number & IP address)
  socklen = sizeof(srv);
  if ((ret = bind(sockfd, (struct sockaddr *)&srv, socklen)) < 0) {
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
    puts("new connection granted\n");

  char buf[BUF_LEN];
  int buf_len = BUF_LEN;
  int j = 0;
  // read (read from file discripter)
  if ((buf_len = read(infd, buf, sizeof(buf))) != -1) {

    // 1文字ずつ出力(フォーマットを考慮する)
    while (buf[j] != EOT) {  // ブラウザから送信された文字列の末尾はEOT(伝送終了)

      // CRとLFはそのまま%cで出力すると端末上の表記が乱れるのでエスケープ
      switch (buf[j]) {
      case '\r': // キャリッジリターン
	printf("buf[%03d] =   0x%02x %3d carriage return ", j, buf[j], buf[j]);
	j++;
	if ((j % 3) == 0)
	  putchar('\n');
	continue;
      case '\n': // ラインフィード
	printf("buf[%03d] =   0x%02x %3d line feed       ", j, buf[j], buf[j]);
	j++;
	if ((j % 3) == 0)
	  putchar('\n');
	continue;
      }

      // [CR, LF, それ以外の不可視文字]以外の1文字を出力
      printf("buf[%03d] = %c 0x%02x %3d ", j, buf[j], buf[j], buf[j]);

      // CR, LF以外の不可視文字の明示的に表記
      switch (buf[j]) {
      case ' '  : printf("space           "); break;
      default   : printf("                "); break;
      }

      j++;
      // 3文字分出力したら改行(見た目の設定)
      if ((j % 3) == 0)
	putchar('\n');

    } // while (buf[j] != EOT) の閉じ括弧

    // 最後に改行されてなければ改行してさらに改行(見た目の設定)
    if ((j % 3) != 0)
      putchar('\n');
    putchar('\n');

    // 文字列をまとめて出力(フォーマットを無視する)
    j = 0;
    while (buf[j] != EOT) {  // ブラウザから送信された文字列の末尾はEOT(伝送終了)
      printf("%c", buf[j]);
      j++;
    }

  }   // if ((buf_len = read(infd, buf, sizeof(buf))) != -1) の閉じ括弧

  close(infd);
  close(sockfd);

  return 0;
}
