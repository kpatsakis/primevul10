void MemFile::sweep() {
  close();
  File::sweep();
}