Variant HHVM_FUNCTION(imagecolorclosestalpha,
    const Resource& image,
    int64_t red, int64_t green, int64_t blue, int64_t alpha) {
  gdImagePtr im = get_valid_image_resource(image);
  if (!im) return false;
  return gdImageColorClosestAlpha(im, red, green, blue, alpha);
}