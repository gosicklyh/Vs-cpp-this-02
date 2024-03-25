#include <arpa/inet.h>
#include <cstdio>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

int main() {
  // 1.创建监听的套接字
  int fd = socket(AF_INET, SOCK_STREAM, 0);
  if (fd == -1) {
    perror("socket");
    return -1;
  }

  // 2.绑定本地IP port
  struct sockaddr_in saddr;
  saddr.sin_family = AF_INET;
  saddr.sin_port = htons(9999);
  saddr.sin_addr.s_addr = INADDR_ANY;
  int ret = bind(fd, (struct sockaddr *)&saddr, sizeof(saddr));
  if (ret == -1) {
    perror("bind");
    return -1;
  }
  // 3.设置监听
  ret = listen(fd, 128);
  if (ret == -1) {
    perror("listen");
    return -1;
  }
  // 4.阻塞并等待客户端的连接
  struct sockaddr_in caddr;
  int addrlen = sizeof(caddr);
  int cfd = accept(fd, (struct sockaddr *)&caddr, (socklen_t *)&addrlen);
  if (cfd == -1) {
    perror("accept");
    return -1;
  }
  // 链接成功，打印客户端IP和端口信息
  char ip[32];
  std::cout << "客户端的IP:"
            << inet_ntop(AF_INET, &caddr.sin_addr.s_addr, ip, sizeof(ip))
            << ",端口 :" << ntohs(caddr.sin_port) << std::endl;
  // 5.通信
  while (1) {
    // 接受数据
    std::string buff;
    int len = recv(cfd, &buff, sizeof(buff), 0);
    if (len > 0) {
      std::cout << "client say: ," << buff;
      send(cfd, &buff, len, 0);
    } else if (len == 0) {
      std::cout << "客服端断开链接...\n";
      break;
    } else {
      perror("recv");
      break;
    }
  }
  //关闭套接字
  close(fd);
  close(cfd);
}