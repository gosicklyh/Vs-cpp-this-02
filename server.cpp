#include <arpa/inet.h>
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

//信息结构体
struct SockInfo {
  struct sockaddr_in addr;
  int fd;
};
struct SockInfo infos[512];

void *working(void *arg) {

  struct SockInfo *pinfo = (struct SockInfo *)arg;
  // 链接成功，打印客户端IP和端口信息
  char ip[32];
  std::cout << "客户端的IP:"
            << inet_ntop(AF_INET, &infos->addr.sin_addr.s_addr, ip, sizeof(ip))
            << ",端口 :" << ntohs(infos->addr.sin_port) << std::endl;
  // 5.通信
  while (1) {
    // 接受数据
    char buff[1024];
    memset(buff, 0, sizeof(buff));
    int len = recv(pinfo->fd, buff, sizeof(buff), 0);
    if (len > 0) {
      std::cout << "client say: ," << buff;
      send(pinfo->fd, buff, len, 0);
    } else if (len == 0) {
      std::cout << "客户端断开链接...\n";
      break;
    } else {
      perror("recv");
      break;
    }
  }
  //关闭文件描述符
  close(pinfo->fd);
  pinfo->fd = -1;
  return NULL;
}

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
  saddr.sin_port = htons(8888);
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

  //初始化结构体数组
  int max = sizeof(infos) / sizeof(infos[0]);
  for (int i = 0; i < max; i++) {
    memset(&infos[i], 0, sizeof(infos[i]));
    infos[i].fd = -1;
  }

  // 4.阻塞并等待客户端的连接
  int addrlen = sizeof(struct sockaddr_in);
  while (1) {
    struct SockInfo *pinfo;
    for (int i = 0; i < max; i++) {
      if (infos[i].fd == -1) {
        pinfo = &infos[i];
        break;
      }
    }
    pinfo->fd =
        accept(fd, (struct sockaddr *)&pinfo->addr, (socklen_t *)&addrlen);

    if (pinfo->fd == -1) {
      perror("accept");
      break;
    }
    // 创建子线程
    pthread_t tid;
    pthread_create(&tid, NULL, working, pinfo);
    pthread_detach(tid);
  }
  close(fd);
  return 0;
}