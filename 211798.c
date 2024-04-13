Variant HHVM_FUNCTION(imagecreatefromxpm, const String& filename) {
  gdImagePtr im =
    _php_image_create_from(filename, -1, -1, -1, -1,
                           PHP_GDIMG_TYPE_XPM, "XPM",
                           (gdImagePtr(*)())gdImageCreateFromXpm,
                           (gdImagePtr(*)())nullptr);
  if (im == nullptr) {
    return false;
  }
  return Variant(req::make<Image>(im));
}