#ifndef SYSTEM_H
#define SYSTEM_H

#include <patlms/network/detail/system_interface.h>

namespace network
{

namespace detail
{

class System : public SystemInterface {
 public:
  virtual ~System();

  int Accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen) override;

  int Unlink(const char *pathname) override;

  int Socket(int domain, int type, int protocol) override;

  int Bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen) override;

  int Listen(int sockfd, int backlog) override;

  int Chmod(const char *pathname, mode_t mode) override;

  int Close(int socket) override;

  int Poll(struct pollfd *fds, nfds_t nfds, int timeout) override;

  ssize_t Recv(int sockfd, void *buf, size_t len, int flags) override;

  ssize_t Send(int sockfd, const void *buf, size_t len, int flags) override;

  int Getsockopt(int sockfd, int level, int optname, void *optval, socklen_t *optlen) override;

  time_t Time(time_t *t) override;

  struct tm *GMTime(const time_t *timep) override;

  int Gethostname(char *name, size_t len) override;
};

}

}

#endif /* SYSTEM_H */