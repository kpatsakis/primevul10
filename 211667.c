bool HHVM_FUNCTION(imagewbmp, const Resource& image,
    const String& filename /* = null_string */,
    int64_t foreground /* = -1 */) {
  return _php_image_output_ctx(image, filename, foreground, -1,
                               PHP_GDIMG_TYPE_WBM, "WBMP",
                               (void (*)())gdImageWBMPCtx);
}