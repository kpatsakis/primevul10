struct gfxinfo *php_handle_jpeg(const req::ptr<File>& file, Array& info) {
  struct gfxinfo *result = nullptr;
  unsigned int marker = M_PSEUDO;
  unsigned short length, ff_read=1;

  for (;;) {
    marker = php_next_marker(file, marker, ff_read);
    ff_read = 0;
    switch (marker) {
    case M_SOF0:
    case M_SOF1:
    case M_SOF2:
    case M_SOF3:
    case M_SOF5:
    case M_SOF6:
    case M_SOF7:
    case M_SOF9:
    case M_SOF10:
    case M_SOF11:
    case M_SOF13:
    case M_SOF14:
    case M_SOF15:
      if (result == nullptr) {
        /* handle SOFn block */
        result = (struct gfxinfo *)IM_CALLOC(1, sizeof(struct gfxinfo));
        CHECK_ALLOC_R(result, sizeof (struct gfxinfo), nullptr);
        length = php_read2(file);
        result->bits = file->getc();
        result->height = php_read2(file);
        result->width = php_read2(file);
        result->channels = file->getc();
        if (info.isNull() || length < 8) {
          /* if we don't want an extanded info -> return */
          return result;
        }
        if (!file->seek(length - 8, SEEK_CUR)) {
          /* file error after info */
          return result;
        }
      } else {
        if (!php_skip_variable(file)) {
          return result;
        }
      }
      break;

    case M_APP0:
    case M_APP1:
    case M_APP2:
    case M_APP3:
    case M_APP4:
    case M_APP5:
    case M_APP6:
    case M_APP7:
    case M_APP8:
    case M_APP9:
    case M_APP10:
    case M_APP11:
    case M_APP12:
    case M_APP13:
    case M_APP14:
    case M_APP15:
      if (!info.isNull()) {
        if (!php_read_APP(file, marker, info)) {
          /* read all the app markes... */
          return result;
        }
      } else {
        if (!php_skip_variable(file)) {
          return result;
        }
      }
      break;

    case M_SOS:
    case M_EOI:
      /* we're about to hit image data, or are at EOF. stop processing. */
      return result;

    default:
      if (!php_skip_variable(file)) {
        /* anything else isn't interesting */
        return result;
      }
      break;
    }
  }

  return result; /* perhaps image broken -> no info but size */
}