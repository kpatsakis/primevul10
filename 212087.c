static gdImagePtr _php_image_create_from(const String& filename,
                                         int srcX, int srcY,
                                         int width, int height,
                                         int image_type, char *tn,
                                         gdImagePtr(*func_p)(),
                                         gdImagePtr(*ioctx_func_p)()) {
  VMRegAnchor _;
  gdImagePtr im = nullptr;
#ifdef HAVE_GD_JPG
  // long ignore_warning;
#endif

  if (image_type == PHP_GDIMG_TYPE_GD2PART) {
    if (width < 1 || height < 1) {
      raise_warning("Zero width or height not allowed");
      return nullptr;
    }
  }
  auto file = File::Open(filename, "rb");
  if (!file) {
    raise_warning("failed to open stream: %s", filename.c_str());
    return nullptr;
  }

  FILE *fp = nullptr;
  auto plain_file = dyn_cast<PlainFile>(file);
  if (plain_file) {
    fp = plain_file->getStream();
  } else  if (ioctx_func_p) {
    /* we can create an io context */
    gdIOCtx* io_ctx;

    // copy all
    String buff = file->read(8192);
    String str;
    do {
      str = file->read(8192);
      buff += str;
    } while (!str.empty());

    if (buff.empty()) {
      raise_warning("Cannot read image data");
      return nullptr;
    }

    io_ctx = gdNewDynamicCtxEx(buff.length(), (char *)buff.c_str(), 0);
    if (!io_ctx) {
      raise_warning("Cannot allocate GD IO context");
      return nullptr;
    }

    if (image_type == PHP_GDIMG_TYPE_GD2PART) {
      im =
        ((gdImagePtr(*)(gdIOCtx *, int, int, int, int))(ioctx_func_p))
          (io_ctx, srcX, srcY, width, height);
    } else {
      im = ((gdImagePtr(*)(gdIOCtx *))(ioctx_func_p))(io_ctx);
    }
    io_ctx->gd_free(io_ctx);
  }
  else {
    /* TODO: try and force the stream to be FILE* */
    assertx(false);
  }

  if (!im && fp) {
    switch (image_type) {
    case PHP_GDIMG_TYPE_GD2PART:
      im = ((gdImagePtr(*)(FILE *, int, int, int, int))(func_p))
             (fp, srcX, srcY, width, height);
      break;
#if defined(HAVE_GD_XPM) && defined(HAVE_GD_BUNDLED)
    case PHP_GDIMG_TYPE_XPM:
      im = gdImageCreateFromXpm(filename);
      break;
#endif

#ifdef HAVE_GD_JPG
    case PHP_GDIMG_TYPE_JPG:
      im = gdImageCreateFromJpeg(fp);
      break;
#endif

    default:
      im = ((gdImagePtr(*)(FILE*))(func_p))(fp);
      break;
    }

    fflush(fp);
  }

  if (im) {
    file->close();
    return im;
  }

  raise_warning("'%s' is not a valid %s file", filename.c_str(), tn);
  file->close();
  return nullptr;
}