bool HHVM_FUNCTION(imagesetpixel, const Resource& image,
    int64_t x, int64_t y, int64_t color) {
  gdImagePtr im = get_valid_image_resource(image);
  if (!im) return false;
  gdImageSetPixel(im, x, y, color);
  return true;
}