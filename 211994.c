bool HHVM_FUNCTION(imageantialias, const Resource& image, bool on) {
  gdImagePtr im = get_valid_image_resource(image);
  if (!im) return false;
  SetAntiAliased(im, on);
  return true;
}