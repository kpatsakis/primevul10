bool HHVM_FUNCTION(imagestring, const Resource& image,
    int64_t font, int64_t x, int64_t y,
    const String& str, int64_t color) {
  return php_imagechar(image, font, x, y, str, color, 2);
}