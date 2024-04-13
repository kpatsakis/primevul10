  FSReqBuffer& Init(const char* syscall, size_t len,
                    enum encoding encoding) {
    syscall_ = syscall;
    encoding_ = encoding;

    buffer_.AllocateSufficientStorage(len + 1);
    has_data_ = false;  // so that the data does not show up in error messages
    return buffer_;
  }