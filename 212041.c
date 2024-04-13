bool HHVM_FUNCTION(imagesetinterpolation,
    const Resource& image, int64_t method /*=GD_BILINEAR_FIXED*/) {
  gdImagePtr im = get_valid_image_resource(image);
  if (!im) return false;
  if (method == -1) method = GD_BILINEAR_FIXED;
  return gdImageSetInterpolationMethod(im, (gdInterpolationMethod) method);
}