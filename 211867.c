Variant HHVM_FUNCTION(imagecreatefromwbmp, const String& filename) {
  gdImagePtr im =
    _php_image_create_from(filename, -1, -1, -1, -1,
                           PHP_GDIMG_TYPE_WBM, "WBMP",
                           (gdImagePtr(*)())gdImageCreateFromWBMP,
                           (gdImagePtr(*)())gdImageCreateFromWBMPCtx);
  if (im == nullptr) {
    return false;
  }
  return Variant(req::make<Image>(im));
}