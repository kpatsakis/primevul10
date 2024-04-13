Variant HHVM_FUNCTION(socket_select,
                      Variant& read,
                      Variant& write,
                      Variant& except,
                      const Variant& vtv_sec,
                      int tv_usec /* = 0 */) {
  int count = 0;
  if (!read.isNull()) {
    if (!read.isArray()) {
      raise_warning("socket_select() expects parameter 1 to be array()");
      return init_null();
    }
    count += read.asCArrRef().size();
  }
  if (!write.isNull()) {
    if (!write.isArray()) {
      raise_warning("socket_select() expects parameter 2 to be array()");
      return init_null();
    }
    count += write.asCArrRef().size();
  }
  if (!except.isNull()) {
    if (!except.isArray()) {
      raise_warning("socket_select() expects parameter 3 to be array()");
      return init_null();
    }
    count += except.asCArrRef().size();
  }
  if (!count) {
    return false;
  }

  std::vector<pollfd> fds;
  fds.reserve(count);
  if (!read.isNull()) {
    sock_array_to_fd_set(read.asCArrRef(), fds, POLLIN);
  }
  if (!write.isNull()) {
    sock_array_to_fd_set(write.asCArrRef(), fds, POLLOUT);
  }
  if (!except.isNull()) {
    sock_array_to_fd_set(except.asCArrRef(), fds, POLLPRI);
  }
  if (fds.empty()) {
    raise_warning("no resource arrays were passed to select");
    return false;
  }

  IOStatusHelper io("socket_select");
  int timeout_ms = -1;
  if (!vtv_sec.isNull()) {
    timeout_ms = vtv_sec.toInt32() * 1000 + tv_usec / 1000;
  }

  /* slight hack to support buffered data; if there is data sitting in the
   * read buffer of any of the streams in the read array, let's pretend
   * that we selected, but return only the readable sockets */
  if (!read.isNull()) {
    // sock_array_from_fd_set can set a sparsely indexed array, so
    // we use darray everywhere.
    auto hasData = Array::CreateDArray();
    IterateVNoInc(
      read.asCArrRef().get(),
      [&](TypedValue v) {
        assertx(v.m_type == KindOfResource);
        auto file = static_cast<File*>(v.m_data.pres->data());
        if (file->bufferedLen() > 0) {
          hasData.append(v);
        }
      }
    );
    if (hasData.size() > 0) {
      write = empty_darray();
      except = empty_darray();
      read = hasData;
      return hasData.size();
    }
  }

  int retval = poll(fds.data(), fds.size(), timeout_ms);
  if (retval == -1) {
    raise_warning("unable to select [%d]: %s", errno,
                  folly::errnoStr(errno).c_str());
    return false;
  }

  count = 0;
  int nfds = 0;
  if (!read.isNull()) {
    sock_array_from_fd_set(read, fds, nfds, count, POLLIN|POLLERR|POLLHUP);
  }
  if (!write.isNull()) {
    sock_array_from_fd_set(write, fds, nfds, count, POLLOUT|POLLERR);
  }
  if (!except.isNull()) {
    sock_array_from_fd_set(except, fds, nfds, count, POLLPRI|POLLERR);
  }

  return count;
}