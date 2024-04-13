bool HHVM_FUNCTION(imagefilledellipse, const Resource& image,
    int64_t cx, int64_t cy, int64_t width, int64_t height, int64_t color) {
  gdImagePtr im = get_valid_image_resource(image);
  if (!im) return false;
  gdImageFilledEllipse(im, cx, cy, width, height, color);
  return true;
}