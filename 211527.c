Variant HHVM_FUNCTION(imagecreatefromwebp, const String& filename) {
  gdImagePtr im =
    _php_image_create_from(filename, -1, -1, -1, -1,
                           PHP_GDIMG_TYPE_WEBP, "WEBP",
                           (gdImagePtr(*)())gdImageCreateFromWebp,
                           (gdImagePtr(*)())gdImageCreateFromWebpCtx);
  if (im == nullptr) {
    return false;
  }
  return Variant(req::make<Image>(im));
}