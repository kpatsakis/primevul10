Variant HHVM_FUNCTION(imagettftext, const Resource& image,
    const Variant& size, const Variant& angle,
    int64_t x, int64_t y, int64_t color,
    const String& fontfile, const String& text) {
  return php_imagettftext_common(TTFTEXT_DRAW, 0,
                                 image, size.toDouble(), angle.toDouble(),
                                 x, y, color, fontfile, text);
}