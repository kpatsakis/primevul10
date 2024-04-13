bool HHVM_FUNCTION(imagecharup, const Resource& image,
    int64_t font, int64_t x, int64_t y,
    const String& c, int64_t color) {
  return php_imagechar(image, font, x, y, c, color, 1);
}