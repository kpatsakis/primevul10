bool HHVM_FUNCTION(imagejpeg, const Resource& image,
    const String& filename /* = null_string */, int64_t quality /* = -1 */) {
  return _php_image_output_ctx(image, filename, quality, -1,
                               PHP_GDIMG_TYPE_JPG, "JPEG",
                               (void (*)())gdImageJpegCtx);
}