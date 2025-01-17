static void sock_array_from_fd_set(Variant &sockets,
                                   const std::vector<pollfd>& fds,
                                   int &nfds, int &count, const short flag) {
  Array ret = Array::CreateDArray();
  assertx(sockets.isArray());
  const auto& sock_array = sockets.asCArrRef();
  IterateKVNoInc(
    sock_array.get(),
    [&](TypedValue k, TypedValue v) {
      const pollfd &fd = fds.at(nfds++);
      assertx(v.m_type == KindOfResource);
      assertx(fd.fd == static_cast<File*>(v.m_data.pres->data())->fd());
      if (fd.revents & flag) {
        ret.set(k, v);
        count++;
      }
    }
  );
  sockets = ret;
}