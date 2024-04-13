bool HHVM_FUNCTION(imagegif, const Resource& image,
    const String& filename /* = null_string */) {
  return _php_image_output_ctx(image, filename, -1, -1,
                               PHP_GDIMG_TYPE_GIF, "GIF",
                               (void (*)())gdImageGifCtx);
}