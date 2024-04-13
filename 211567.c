Variant HHVM_FUNCTION(imagecreatefromgd, const String& filename) {
  gdImagePtr im =
    _php_image_create_from(filename, -1, -1, -1, -1,
                           PHP_GDIMG_TYPE_GD, "GD",
                           (gdImagePtr(*)())gdImageCreateFromGd,
                           (gdImagePtr(*)())gdImageCreateFromGdCtx);
  if (im == nullptr) {
    return false;
  }
  return Variant(req::make<Image>(im));
}