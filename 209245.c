string TarFileReader::extract(ostream &out) {
  if (!hasMore()) THROW("No more tar files");

  readFile(out, pri->filter);
  didReadHeader = false;

  return getFilename();
}