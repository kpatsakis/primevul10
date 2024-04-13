bool TarFileReader::hasMore() {
  if (!didReadHeader) {
    SysError::clear();
    if (!readHeader(pri->filter))
      THROW("Tar file read failed: " << SysError());
    didReadHeader = true;
  }

  return !isEOF();
}