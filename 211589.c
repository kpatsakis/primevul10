bool HHVM_FUNCTION(imagepng, const Resource& image,
    const String& filename /* = null_string */,
    int64_t quality /* = -1 */, int64_t filters /* = -1 */) {
  return _php_image_output_ctx(image, filename, quality, filters,
                               PHP_GDIMG_TYPE_PNG, "PNG",
                               (void (*)())gdImagePngCtxEx);
}