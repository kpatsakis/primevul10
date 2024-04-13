bool TarFileReader::next() {
  if (didReadHeader) {
    skipFile(pri->filter);
    didReadHeader = false;
  }

  return hasMore();
}