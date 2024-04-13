bool HHVM_FUNCTION(imagefilledarc, const Resource& image,
    int64_t cx, int64_t cy, int64_t width, int64_t height,
    int64_t start, int64_t end, int64_t color, int64_t style) {
  gdImagePtr im = get_valid_image_resource(image);
  if (!im) return false;
  if (end < 0) end %= 360;
  if (start < 0) start %= 360;
  gdImageFilledArc(im, cx, cy, width, height, start, end, color, style);
  return true;
}