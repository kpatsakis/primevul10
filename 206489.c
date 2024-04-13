int LibuvStreamWrap::ReadStop() {
  return uv_read_stop(stream());
}