#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>

#define PORT (8080)
#define SERVER_ROOT "/homer/is/14/is0243rk/adprogex1/socket/htdocs"
#define MAX_BUF (8192)
#define MAX_CONTENTS (1000000)
#define MAX_PATH (512)

#define STAT

int main() {

  int sockfd, infd, on, ret;
  struct sockaddr_in srv;
  socklen_t socklen;
  int output_flag = 1;

  while (1) {
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
    if (bind(sockfd, (struct sockaddr *)&srv, socklen) < 0) {
      puts("bind error");
      exit(0);
    }

    // listen (wait for client connection)
    if (listen(sockfd, 5) < 0) {  // generally backlog size 5
      puts("listen error");
      exit(0);
    }

    if (output_flag == 1) {
      puts("TCP/IP socket available");
      printf("port %d\n", ntohs(srv.sin_port));
      printf("addr %s\n", inet_ntoa(srv.sin_addr));
      puts("------------------------------------------");
      output_flag = 0;
    }

    // accept (accept first connection)
    if ((infd = accept(sockfd, (struct sockaddr *)&srv, &socklen)) < 0) {
      puts("accept error");
      exit(0);
    }

    /* ---------------------------------------------------- */

    // read
    char buf[MAX_BUF+1];
    int len;
    if ((len = read(infd, buf, MAX_BUF)) > 0) {
      buf[len] = '\0';
      printf("%s", buf);
    }
    if (len < 0)
      printf("read error\n");

    // real_path extract
    char real_path[MAX_PATH];
    int i = 0;
    int j = 0;
    while (buf[i] != '/')
      i++;
    while (buf[i] != ' ') {
      real_path[j] = buf[i];
      i++;
      j++;
    }
    real_path[j] = '\0';

    // full_path generate
    char full_path[MAX_PATH] = SERVER_ROOT;
    i = strlen(full_path);
    j = 0;
    while (real_path[j] != '\0') {
      full_path[i] = real_path[j];
      i++;
      j++;
    }

    // file or directory
    struct stat st;
    int isdir;
    isdir = stat(full_path, &st);

    // check whether path is directory or not
    // also check whether path is not found
#ifdef STAT
    if (isdir != 0)
      printf("%s is not found\n", full_path);
    if ((st.st_mode & S_IFMT) == S_IFDIR)
      printf("%s is directory\n", full_path);
    else
      printf("%s is not directory\n", full_path);
#endif

    // full path is directory
    if ((st.st_mode & S_IFMT) == S_IFDIR) {
      if (full_path[i-1] != '/') {
	full_path[i] = '/';
	i++;
      }
      full_path[i] = 'i'; i++;
      full_path[i] = 'n'; i++;
      full_path[i] = 'd'; i++;
      full_path[i] = 'e'; i++;
      full_path[i] = 'x'; i++;
      full_path[i] = '.'; i++;
      full_path[i] = 'h'; i++;
      full_path[i] = 't'; i++;
      full_path[i] = 'm'; i++;
      full_path[i] = 'l'; i++;
      full_path[i] = '\0';
    }
    // full path is not directory
    else
      full_path[i] = '\0';
    printf("Path: %s\n", full_path);

    /* ---------------------------------------------------- */

    // file open
    FILE *fp;
    unsigned char contents[MAX_CONTENTS];
    int isize;
    fp = fopen(full_path, "rb");

    // file not found
    if (fp == NULL) {
      // write
      write(infd, "HTTP/1.0 404 Not Found\r\n", strlen("HTTP/1.0 404 Not Found\r\n"));
      write(infd, "\r\n", strlen("\r\n"));
      write(infd, "404 Not Found\r\n", strlen("404 Not Found\r\n"));

      // output status
      puts("File not found");
      puts("------------------------------------------");

      // close
      close(sockfd);
      close(infd);
    }

    // file exist
    else {
      // write
      write(infd, "HTTP/1.0 200 OK\r\n", strlen("HTTP/1.0 200 OK\r\n"));
      write(infd, "\r\n", strlen("\r\n"));
      isize = fread(contents, sizeof(unsigned char), sizeof(contents), fp);
      write(infd, contents, isize);
      fclose(fp);

      // output status
      printf("Server wrote http to browser successfully\n");
      puts("------------------------------------------");

      // close
      close(sockfd);
      close(infd);
    }

  } // while (1)

  return 0;
}
