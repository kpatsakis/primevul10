static struct gfxinfo *php_handle_ico(const req::ptr<File>& stream) {
  struct gfxinfo *result = nullptr;
  const unsigned char *s;
  int num_icons = 0;

  String dim = stream->read(2);
  if (dim.length() != 2) {
    return nullptr;
  }

  s = (unsigned char *)dim.c_str();
  num_icons = (((unsigned int)s[1]) << 8) + ((unsigned int)s[0]);

  if (num_icons < 1 || num_icons > 255) {
    return nullptr;
  }

  result = (struct gfxinfo *)IM_CALLOC(1, sizeof(struct gfxinfo));
  CHECK_ALLOC_R(result, (sizeof(struct gfxinfo)), nullptr);

  while (num_icons > 0) {
    dim = stream->read(16);
    if (dim.length() != 16) {
      break;
    }

    s = (unsigned char *)dim.c_str();

    if ((((unsigned int)s[7]) << 8) + ((unsigned int)s[6]) >= result->bits) {
      result->width  = (unsigned int)s[0];
      result->height = (unsigned int)s[1];
      result->bits   = (((unsigned int)s[7]) << 8) + ((unsigned int)s[6]);
    }
    num_icons--;
  }

  return result;
}