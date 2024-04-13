bool HHVM_FUNCTION(imagefilledrectangle, const Resource& image,
    int64_t x1, int64_t y1, int64_t x2, int64_t y2, int64_t color) {
  gdImagePtr im = get_valid_image_resource(image);
  if (!im) return false;
  gdImageFilledRectangle(im, x1, y1, x2, y2, color);
  return true;
}