static int connect_with_timeout(int fd, struct sockaddr *sa_ptr,
                                size_t sa_size, double timeout,
                                const HostURL &hosturl,
                                std::string &errstr, int &errnum) {
  if (timeout <= 0) {
    int retval = connect(fd, sa_ptr, sa_size);
    if (retval < 0) {
      errstr = "unable to connect to " + hosturl.getHostURL();
      errnum = errno;
    }
    return retval;
  }

  // set non-blocking so we can do timeouts
  long arg = fcntl(fd, F_GETFL, nullptr);
  fcntl(fd, F_SETFL, arg | O_NONBLOCK);

  int retval = connect(fd, sa_ptr, sa_size);
  if (retval < 0) {
    if (errno == EINPROGRESS) {
      struct pollfd fds[1];
      fds[0].fd = fd;
      fds[0].events = POLLOUT;
      if (poll(fds, 1, (int)(timeout * 1000))) {
        int valopt;
        socklen_t lon = sizeof(int);
        getsockopt(fd, SOL_SOCKET, SO_ERROR, (void*)(&valopt), &lon);
        if (valopt) {
          errstr = "failed to connect to " + hosturl.getHostURL();
          errnum = valopt;
        }
        retval = valopt ? -1 : 0;
      } else {
        errstr = "timed out after ";
        errstr += folly::to<std::string>(timeout);
        errstr += " seconds when connecting to " + hosturl.getHostURL();
        errnum = ETIMEDOUT;
        retval = -1;
      }
    } else {
      errstr = "unable to connect to " + hosturl.getHostURL();
      errnum = errno;
    }
  }

  // set to blocking mode
  arg = fcntl(fd, F_GETFL, nullptr);
  fcntl(fd, F_SETFL, arg & ~O_NONBLOCK);

  return retval;
}