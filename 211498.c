static struct gfxinfo *php_handle_png(const req::ptr<File>& stream) {
  struct gfxinfo *result = nullptr;
  const unsigned char *s;
  /* Width:              4 bytes
   * Height:             4 bytes
   * Bit depth:          1 byte
   * Color type:         1 byte
   * Compression method: 1 byte
   * Filter method:      1 byte
   * Interlace method:   1 byte
   */

  if (!stream->seek(8, SEEK_CUR)) return nullptr;

  String dim = stream->read(9);
  if (dim.length() < 9) return nullptr;

  s = (unsigned char *)dim.c_str();
  result = (struct gfxinfo *)IM_CALLOC(1, sizeof(struct gfxinfo));
  CHECK_ALLOC_R(result, sizeof (struct gfxinfo), nullptr);
  result->width = (((unsigned int)s[0]) << 24) +
                   (((unsigned int)s[1]) << 16) +
                   (((unsigned int)s[2]) << 8) +
                   ((unsigned int)s[3]);
  result->height = (((unsigned int)s[4]) << 24) +
                   (((unsigned int)s[5]) << 16) +
                   (((unsigned int)s[6]) << 8) +
                   ((unsigned int)s[7]);
  result->bits = (unsigned int)s[8];
  return result;
}