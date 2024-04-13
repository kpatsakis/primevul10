Variant HHVM_FUNCTION(imagecreatefromgd2, const String& filename) {
  gdImagePtr im =
    _php_image_create_from(filename, -1, -1, -1, -1,
                           PHP_GDIMG_TYPE_GD2, "GD2",
                           (gdImagePtr(*)())gdImageCreateFromGd2,
                           (gdImagePtr(*)())gdImageCreateFromGd2Ctx);
  if (im == nullptr) {
    return false;
  }
  return Variant(req::make<Image>(im));
}