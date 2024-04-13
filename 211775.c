bool HHVM_FUNCTION(imageellipse, const Resource& image,
    int64_t cx, int64_t cy, int64_t width, int64_t height, int64_t color) {
  gdImagePtr im = get_valid_image_resource(image);
  if (!im) return false;
  color = SetupAntiAliasedColor(im, color);
  gdImageArc(im, cx, cy, width, height, 0, 360, color);
  return true;
}