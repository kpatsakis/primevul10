bool HHVM_FUNCTION(png2wbmp, const String& pngname, const String& wbmpname,
    int64_t dest_height, int64_t dest_width, int64_t threshold) {
  return _php_image_convert(pngname, wbmpname, dest_height, dest_width,
                            threshold, PHP_GDIMG_TYPE_PNG);
}