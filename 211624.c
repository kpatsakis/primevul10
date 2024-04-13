gdImagePtr _php_image_create_from_string(const String& image, char *tn,
                                         gdImagePtr (*ioctx_func_p)()) {
  VMRegAnchor _;
  gdIOCtx *io_ctx;

  io_ctx = gdNewDynamicCtxEx(image.length(), (char *)image.c_str(), 0);

  if (!io_ctx) {
    return nullptr;
  }

  gdImagePtr im = (*(gdImagePtr (*)(gdIOCtx *))ioctx_func_p)(io_ctx);
  if (!im) {
    raise_warning("Passed data is not in '%s' format", tn);
    io_ctx->gd_free(io_ctx);
    return nullptr;
  }

  io_ctx->gd_free(io_ctx);

  return im;
}