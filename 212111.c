Variant HHVM_FUNCTION(imagecreatefrompng, const String& filename) {
  gdImagePtr im =
    _php_image_create_from(filename, -1, -1, -1, -1,
                           PHP_GDIMG_TYPE_PNG, "PNG",
                           (gdImagePtr(*)())gdImageCreateFromPng,
                           (gdImagePtr(*)())gdImageCreateFromPngCtx);
  if (im == nullptr) {
    return false;
  }
  return Variant(req::make<Image>(im));
}