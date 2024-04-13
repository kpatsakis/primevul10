Variant HHVM_FUNCTION(imagecreatetruecolor, int64_t width, int64_t height) {
  gdImagePtr im;

  if (width <= 0 || height <= 0 || width >= INT_MAX || height >= INT_MAX) {
    raise_warning("Invalid image dimensions");
    return false;
  }

  im = gdImageCreateTrueColor(width, height);

  if (!im) {
    return false;
  }
  return Variant(req::make<Image>(im));
}