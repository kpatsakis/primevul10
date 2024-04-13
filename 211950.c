Variant HHVM_FUNCTION(imageftbbox, double size, double angle,
    const String& font_file, const String& text,
    const Array& extrainfo /*=[] */) {
  return php_imagettftext_common(TTFTEXT_BBOX, 1,
                                 size, angle, font_file, text, extrainfo);
}