bool HHVM_FUNCTION(jpeg2wbmp, const String& jpegname, const String& wbmpname,
    int64_t dest_height, int64_t dest_width, int64_t threshold) {
  return _php_image_convert(jpegname, wbmpname, dest_height, dest_width,
                            threshold, PHP_GDIMG_TYPE_JPG);
}