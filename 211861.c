php_next_marker(const req::ptr<File>& file, int /*last_marker*/, int ff_read) {
  int a=0, marker;

  // get marker byte, swallowing possible padding
  if (!ff_read) {
    size_t extraneous = 0;

    while ((marker = file->getc()) != 0xff) {
      if (marker == EOF) {
        return M_EOI;/* we hit EOF */
      }
      extraneous++;
    }
    if (extraneous) {
      raise_warning("corrupt JPEG data: %zu extraneous bytes before marker",
                    extraneous);
    }
  }
  a = 1;
  do {
    if ((marker = file->getc()) == EOF)
    {
      return M_EOI;/* we hit EOF */
    }
    ++a;
  } while (marker == 0xff);
  if (a < 2)
  {
    return M_EOI; /* at least one 0xff is needed before marker code */
  }
  return (unsigned int)marker;
}