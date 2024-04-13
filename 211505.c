bool HHVM_FUNCTION(imagealphablending, const Resource& image,
                                       bool blendmode) {
  gdImagePtr im = get_valid_image_resource(image);
  if (!im) return false;
  gdImageAlphaBlending(im, blendmode);
  return true;
}