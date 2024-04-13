bool HHVM_FUNCTION(imagelayereffect, const Resource& image, int64_t effect) {
  gdImagePtr im = get_valid_image_resource(image);
  if (!im) return false;
  gdImageAlphaBlending(im, effect);
  return true;
}