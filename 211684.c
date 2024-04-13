Variant HHVM_FUNCTION(imagecreatefromjpeg, const String& filename) {
  gdImagePtr im =
    _php_image_create_from(filename, -1, -1, -1, -1,
                           PHP_GDIMG_TYPE_JPG, "JPEG",
                           (gdImagePtr(*)())gdImageCreateFromJpeg,
                           (gdImagePtr(*)())gdImageCreateFromJpegCtx);
  if (im == nullptr) {
    return false;
  }
  return Variant(req::make<Image>(im));
}