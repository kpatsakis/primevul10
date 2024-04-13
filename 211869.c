Variant HHVM_FUNCTION(imagecreatefromxbm, const String& filename) {
  gdImagePtr im =
    _php_image_create_from(filename, -1, -1, -1, -1,
                           PHP_GDIMG_TYPE_XBM, "XBM",
                           (gdImagePtr(*)())gdImageCreateFromXbm,
                           (gdImagePtr(*)())nullptr);
  if (im == nullptr) {
    return false;
  }
  return Variant(req::make<Image>(im));
}