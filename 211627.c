static int php_read_APP(const req::ptr<File>& stream,
                        unsigned int marker,
                        Array& info) {
  unsigned short length;
  unsigned char markername[16];

  length = php_read2(stream);
  if (length < 2) {
    return 0;
  }
  length -= 2;                /* length includes itself */

  String buffer;
  if (length == 0) {
    // avoid stream reads of length 0, they trigger a notice
    buffer = empty_string();
  } else {
    buffer = stream->read(length);
  }
  if (buffer.length() != length) {
    return 0;
  }

  snprintf((char*)markername, sizeof(markername), "APP%d", marker - M_APP0);

  if (!info.exists(String((const char *)markername))) {
    /* XXX we only catch the 1st tag of it's kind! */
    info.set(String((char*)markername, CopyString), buffer);
  }

  return 1;
}