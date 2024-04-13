Variant HHVM_FUNCTION(imagecreatefromgif, const String& filename) {
  gdImagePtr im =
    _php_image_create_from(filename, -1, -1, -1, -1,
                           PHP_GDIMG_TYPE_GIF, "GIF",
                           (gdImagePtr(*)())gdImageCreateFromGif,
                           (gdImagePtr(*)())gdImageCreateFromGifCtx);
  if (im == nullptr) {
    return false;
  }
  return Variant(req::make<Image>(im));
}