static struct gfxinfo *php_handle_iff(const req::ptr<File>& stream) {
  struct gfxinfo * result;
  char *a;
  int chunkId;
  int size;
  short width, height, bits;

  String str = stream->read(8);
  if (str.length() != 8) return nullptr;
  a = (char *)str.c_str();
  if (strncmp(a+4, "ILBM", 4) && strncmp(a+4, "PBM ", 4)) {
    return nullptr;
  }

  /* loop chunks to find BMHD chunk */
  do {
    str = stream->read(8);
    if (str.length() != 8) return nullptr;
    a = (char *)str.c_str();
    chunkId = php_ifd_get32s(a+0, 1);
    size = php_ifd_get32s(a+4, 1);
    if (size < 0) return nullptr;
    if ((size & 1) == 1) {
      size++;
    }
    if (chunkId == 0x424d4844) { /* BMHD chunk */
      if (size < 9) return nullptr;
      str = stream->read(9);
      if (str.length() != 9) return nullptr;
      a = (char *)str.c_str();
      width = php_ifd_get16s(a+0, 1);
      height = php_ifd_get16s(a+2, 1);
      bits = a[8] & 0xff;
      if (width > 0 && height > 0 && bits > 0 && bits < 33) {
        result = (struct gfxinfo *)IM_CALLOC(1, sizeof(struct gfxinfo));
        CHECK_ALLOC_R(result, sizeof (struct gfxinfo), nullptr);
        result->width = width;
        result->height = height;
        result->bits = bits;
        result->channels = 0;
        return result;
      }
    } else {
      if (!stream->seek(size, SEEK_CUR)) return nullptr;
    }
  } while (1);
}