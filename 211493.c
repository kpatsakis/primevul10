static struct gfxinfo *php_handle_psd(const req::ptr<File>& stream) {
  struct gfxinfo *result = nullptr;
  const unsigned char *s;

  if (!stream->seek(11, SEEK_CUR)) return nullptr;

  String dim = stream->read(8);
  if (dim.length() != 8) return nullptr;
  s = (unsigned char *)dim.c_str();
  result = (struct gfxinfo *)IM_CALLOC(1, sizeof(struct gfxinfo));
  CHECK_ALLOC_R(result, (sizeof(struct gfxinfo)), nullptr);
  result->height = (((unsigned int)s[0]) << 24) +
                   (((unsigned int)s[1]) << 16) +
                   (((unsigned int)s[2]) << 8) +
                   ((unsigned int)s[3]);
  result->width =  (((unsigned int)s[4]) << 24) +
                   (((unsigned int)s[5]) << 16) +
                   (((unsigned int)s[6]) << 8) +
                   ((unsigned int)s[7]);
  return result;
}