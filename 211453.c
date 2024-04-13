Variant HHVM_FUNCTION(imagettfbbox, double size, double angle,
                     const String& fontfile, const String& text) {
  return php_imagettftext_common(TTFTEXT_BBOX, 0,
                                 size, angle, fontfile, text);
}