static void sock_array_to_fd_set(const Array& sockets, std::vector<pollfd>& fds,
                                 const short flag) {
  IterateVNoInc(
    sockets.get(),
    [&](TypedValue v) {
      assertx(v.m_type == KindOfResource);
      auto const intfd = static_cast<File*>(v.m_data.pres->data())->fd();
      if (intfd < 0) {
        raise_warning(
          "cannot represent a stream of type user-space as a file descriptor"
        );
        return;
      }
      fds.emplace_back();
      auto& fd = fds.back();
      fd.fd = intfd;
      fd.events = flag;
      fd.revents = 0;
    }
  );
}