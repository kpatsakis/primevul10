Variant HHVM_FUNCTION(imagecolorstotal, const Resource& image) {
  gdImagePtr im = get_valid_image_resource(image);
  if (!im) return false;
  return (gdImageColorsTotal(im));
}