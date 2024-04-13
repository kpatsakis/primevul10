Variant HHVM_FUNCTION(imagecolorclosest,
    const Resource& image, int64_t red, int64_t green, int64_t blue) {
  gdImagePtr im = get_valid_image_resource(image);
  if (!im) return false;
  return gdImageColorClosest(im, red, green, blue);
}