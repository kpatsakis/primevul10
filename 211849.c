static struct gfxinfo *php_handle_bmp(const req::ptr<File>& stream) {
  struct gfxinfo *result = nullptr;
  const unsigned char *s;
  int size;

  if (!stream->seek(11, SEEK_CUR)) return nullptr;

  String dim = stream->read(16);
  if (dim.length() != 16) return nullptr;
  s = (unsigned char *)dim.c_str();

  size = (((unsigned int)s[3]) << 24) +
         (((unsigned int)s[2]) << 16) +
         (((unsigned int)s[1]) << 8) +
         ((unsigned int)s[0]);
  if (size == 12) {
    result = (struct gfxinfo *)IM_CALLOC(1, sizeof(struct gfxinfo));
    CHECK_ALLOC_R(result, sizeof(struct gfxinfo), nullptr);
    result->width = (((unsigned int)s[5]) << 8) + ((unsigned int)s[4]);
    result->height = (((unsigned int)s[7]) << 8) + ((unsigned int)s[6]);
    result->bits = ((unsigned int)s[11]);
  } else if (size > 12 && (size <= 64 || size == 108 || size == 124)) {
    result = (struct gfxinfo *)IM_CALLOC(1, sizeof(struct gfxinfo));
    CHECK_ALLOC_R(result, sizeof(struct gfxinfo), nullptr);
    result->width = (((unsigned int)s[7]) << 24) +
                    (((unsigned int)s[6]) << 16) +
                    (((unsigned int)s[5]) << 8) +
                    ((unsigned int)s[4]);
    result->height = (((unsigned int)s[11]) << 24) +
                     (((unsigned int)s[10]) << 16) +
                     (((unsigned int)s[9]) << 8) +
                     ((unsigned int)s[8]);
    result->height = abs((int32_t)result->height);
    result->bits = (((unsigned int)s[15]) << 8) +
                   ((unsigned int)s[14]);
  } else {
    return nullptr;
  }

  return result;
}