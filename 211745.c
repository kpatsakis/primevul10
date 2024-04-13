Variant HHVM_FUNCTION(imagesy, const Resource& image) {
  gdImagePtr im = get_valid_image_resource(image);
  if (!im) return false;
  return gdImageSY(im);
}