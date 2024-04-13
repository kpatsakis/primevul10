Variant HHVM_FUNCTION(imagecreatefromgd2part,
    const String& filename,
    int64_t srcx, int64_t srcy, int64_t width, int64_t height) {
  gdImagePtr im =
    _php_image_create_from(filename, srcx, srcy, width, height,
                           PHP_GDIMG_TYPE_GD2PART, "GD2",
                           (gdImagePtr(*)())gdImageCreateFromGd2Part,
                           (gdImagePtr(*)())gdImageCreateFromGd2PartCtx);
  if (im == nullptr) {
    return false;
  }
  return Variant(req::make<Image>(im));
}