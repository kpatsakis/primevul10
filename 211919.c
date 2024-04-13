Variant HHVM_FUNCTION(imagesx, const Resource& image) {
  gdImagePtr im = get_valid_image_resource(image);
  if (!im) return false;
  return gdImageSX(im);
}