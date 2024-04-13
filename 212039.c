bool HHVM_FUNCTION(imagegd2, const Resource& image,
    const String& filename /* = null_string */,
    int64_t chunk_size /* = 0 */, int64_t type /* = 0 */) {
  return _php_image_output(image, filename, chunk_size, type,
                           PHP_GDIMG_TYPE_GD2, "GD2",
                           (void (*)())gdImageGd2);
}