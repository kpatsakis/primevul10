static int php_skip_variable(const req::ptr<File>& stream) {
  off_t length = (unsigned int)php_read2(stream);

  if (length < 2) {
    return 0;
  }
  length = length - 2;
  stream->seek(length, SEEK_CUR);
  return 1;
}