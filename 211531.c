bool HHVM_FUNCTION(imagewebp, const Resource& image,
    const String& filename /* = null_string */,
    int64_t quality /* = 80 */) {
  return _php_image_output_ctx(image, filename, quality, -1,
                               PHP_GDIMG_TYPE_WEBP, "WEBP",
                               (void (*)())gdImageWebpCtx);
}