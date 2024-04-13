Variant HHVM_FUNCTION(imagecolorallocate,
    const Resource& image,
    int64_t red, int64_t green, int64_t blue) {
  gdImagePtr im = get_valid_image_resource(image);
  if (!im) return false;
  int ct = gdImageColorAllocate(im, red, green, blue);
  if (ct < 0) {
    return false;
  }
  return ct;
}