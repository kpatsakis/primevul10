static bool _php_image_output_ctx(const Resource& image, const String& filename,
                                  int quality, int basefilter, int image_type,
                                  char* /*tn*/, void (*func_p)()) {
  gdImagePtr im = get_valid_image_resource(image);
  if (!im) return false;
  req::ptr<File> file;
  FILE *fp = nullptr;
  int q = quality, i;
  int f = basefilter;
  gdIOCtx *ctx;

  /* The third (quality) parameter for Wbmp stands for the threshold
     when called from image2wbmp(). The third (quality) parameter for
     Wbmp and Xbm stands for the foreground color index when called
     from imagey<type>().
   */

  if (!filename.empty()) {
    file = php_open_plain_file(filename, "wb", &fp);
    if (!file) {
      raise_warning("Unable to open '%s' for writing", filename.c_str());
      return false;
    }
    ctx = gdNewFileCtx(fp);
  } else {
    ctx = (gdIOCtx *)IM_MALLOC(sizeof(gdIOCtx));
    CHECK_ALLOC_R(ctx, sizeof(gdIOCtx), false);
    ctx->putC = _php_image_output_putc;
    ctx->putBuf = _php_image_output_putbuf;
    ctx->gd_free = _php_image_output_ctxfree;
  }

  switch(image_type) {
  case PHP_GDIMG_CONVERT_WBM:
    if (q<0||q>255) {
      raise_warning("Invalid threshold value '%d'. "
                      "It must be between 0 and 255", q);
    }
  case PHP_GDIMG_TYPE_JPG:
    ((void(*)(gdImagePtr, gdIOCtx *, int))(func_p))(im, ctx, q);
    break;
  case PHP_GDIMG_TYPE_PNG:
    ((void(*)(gdImagePtr, gdIOCtx *, int, int))(func_p))(im, ctx, q, f);
    break;
  case PHP_GDIMG_TYPE_WEBP:
    ((void(*)(gdImagePtr, gdIOCtx *, int64_t, int))(func_p))(im, ctx, q, f);
    break;
  case PHP_GDIMG_TYPE_XBM:
  case PHP_GDIMG_TYPE_WBM:
    if (q == -1) { // argc < 3
      for(i=0; i < gdImageColorsTotal(im); i++) {
        if (!gdImageRed(im, i) &&
            !gdImageGreen(im, i) &&
            !gdImageBlue(im, i)) break;
      }
      q = i;
    }
    if (image_type == PHP_GDIMG_TYPE_XBM) {
      ((void(*)(gdImagePtr, char *, int, gdIOCtx *))(func_p))
        (im, (char*)filename.c_str(), q, ctx);
    } else {
      ((void(*)(gdImagePtr, int, gdIOCtx *))(func_p))(im, q, ctx);
    }
    break;
  default:
    ((void(*)(gdImagePtr, gdIOCtx *))(func_p))(im, ctx);
    break;
  }

  ctx->gd_free(ctx);

  if (fp) {
    fflush(fp);
    file->close();
  }

  return true;
}