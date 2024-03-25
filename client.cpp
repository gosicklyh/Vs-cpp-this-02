#include <arpa/inet.h>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <pthread.h>
#include <sstream>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
int main() {
  // 1.创建通信的套接字
  int fd = socket(AF_INET, SOCK_STREAM, 0);
  if (fd == -1) {
    perror("socket");
    return -1;
  }

  // 2.链接服务器IP port
  struct sockaddr_in saddr;
  saddr.sin_family = AF_INET;
  saddr.sin_port = htons(8888);
  inet_pton(AF_INET, "192.168.7.127", &saddr.sin_addr.s_addr);
  int ret = connect(fd, (struct sockaddr *)&saddr, sizeof(saddr));
  if (ret == -1) {
    perror("bind");
    return -1;
  }

  int number = 0;
  // 3.通信
  while (1) {
    // 发送数据
    char buff[1024];

    sprintf(buff, "你好,hello world,%d...\n", number++);
    send(fd, buff, strlen(buff), 0);

    // 接受数据
    memset(buff, 0, sizeof(buff));
    int len = recv(fd, buff, sizeof(buff), 0);
    if (len > 0) {
      std::cout << "server say: ," << buff;
    } else if (len == 0) {
      std::cout << "服务器端断开链接...\n";
      break;
    } else {
      perror("recv");
      break;
    }
    sleep(1);
  }
  //关闭套接字
  close(fd);
}