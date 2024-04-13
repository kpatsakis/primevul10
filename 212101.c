static int php_get_xbm(const req::ptr<File>& stream, struct gfxinfo **result) {
  String fline;
  char *iname;
  char *type;
  int value;
  unsigned int width = 0, height = 0;

  if (result) {
    *result = nullptr;
  }
  if (!stream->rewind()) {
    return 0;
  }
  while (!(fline = HHVM_FN(fgets)(Resource(stream), 0).toString()).empty()) {
    iname = (char *)IM_MALLOC(fline.size() + 1);
    CHECK_ALLOC_R(iname, (fline.size() + 1), 0);
    if (sscanf(fline.c_str(), "#define %s %d", iname, &value) == 2) {
      if (!(type = strrchr(iname, '_'))) {
        type = iname;
      } else {
        type++;
      }

      if (!strcmp("width", type)) {
        width = (unsigned int)value;
        if (height) {
          IM_FREE(iname);
          break;
        }
      }
      if (!strcmp("height", type)) {
        height = (unsigned int)value;
        if (width) {
          IM_FREE(iname);
          break;
        }
      }
    }
    IM_FREE(iname);
  }

  if (width && height) {
    if (result) {
      *result = (struct gfxinfo *)IM_CALLOC(1, sizeof(struct gfxinfo));
      CHECK_ALLOC_R(*result, sizeof(struct gfxinfo), 0);
      (*result)->width = width;
      (*result)->height = height;
    }
    return IMAGE_FILETYPE_XBM;
  }

  return 0;
}