bool HHVM_FUNCTION(imagefilltoborder, const Resource& image,
    int64_t x, int64_t y, int64_t border, int64_t color) {
  gdImagePtr im = get_valid_image_resource(image);
  if (!im) return false;
  gdImageFillToBorder(im, x, y, border, color);
  return true;
}