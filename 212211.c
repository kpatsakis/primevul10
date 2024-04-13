static struct gfxinfo *php_handle_gif(const req::ptr<File>& stream) {
  struct gfxinfo *result = nullptr;
  const unsigned char *s;

  if (!stream->seek(3, SEEK_CUR)) return nullptr;
  String dim = stream->read(5);
  if (dim.length() != 5) return nullptr;
  s = (unsigned char *)dim.c_str();
  result = (struct gfxinfo *)IM_CALLOC(1, sizeof(struct gfxinfo));
  CHECK_ALLOC_R(result, (sizeof(struct gfxinfo)), nullptr);
  result->width = (unsigned int)s[0] | (((unsigned int)s[1])<<8);
  result->height = (unsigned int)s[2] | (((unsigned int)s[3])<<8);
  result->bits = s[4]&0x80 ? ((((unsigned int)s[4])&0x07) + 1) : 0;
  result->channels = 3; /* always */
  return result;
}