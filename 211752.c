Variant HHVM_FUNCTION(imagefttext,
    const Resource& image,
    const Variant& size, const Variant& angle,
    int64_t x, int64_t y, int64_t col,
    const String& font_file, const String& text,
    const Array& extrainfo) {
  return php_imagettftext_common(TTFTEXT_DRAW, 1,
    image, size, angle, x, y, col, font_file, text, extrainfo);
}