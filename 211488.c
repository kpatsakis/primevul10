bool HHVM_FUNCTION(imagesetthickness,
    const Resource& image, int64_t thickness) {
  gdImagePtr im = get_valid_image_resource(image);
  if (!im) return false;
  gdImageSetThickness(im, thickness);
  return true;
}