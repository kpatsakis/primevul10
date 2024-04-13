int LibuvStreamWrap::GetFD() {
#ifdef _WIN32
  return fd_;
#else
  int fd = -1;
  if (stream() != nullptr)
    uv_fileno(reinterpret_cast<uv_handle_t*>(stream()), &fd);
  return fd;
#endif
}