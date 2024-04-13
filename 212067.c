int php_get_wbmp(const req::ptr<File>& file,
                 struct gfxinfo **result,
                 int check) {
  int i, width = 0, height = 0;

  if (!file->rewind()) {
    return 0;
  }

  /* get type */
  if (file->getc() != 0) {
    return 0;
  }

  /* skip header */
  do {
    i = file->getc();
    if (i < 0) {
      return 0;
    }
  } while (i & 0x80);

  /* get width */
  do {
    i = file->getc();
    if (i < 0) {
      return 0;
    }
    width = (width << 7) | (i & 0x7f);
  } while (i & 0x80);

  /* get height */
  do {
    i = file->getc();
    if (i < 0) {
      return 0;
    }
    height = (height << 7) | (i & 0x7f);
  } while (i & 0x80);

  // maximum valid sizes for wbmp (although 127x127 may be a
  // more accurate one)
  if (!height || !width || height > 2048 || width > 2048) {
    return 0;
  }

  if (!check) {
    (*result)->width = width;
    (*result)->height = height;
  }

  return IMAGE_FILETYPE_WBMP;
}