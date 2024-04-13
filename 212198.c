HHVM_FUNCTION(imagerotate, const Resource& source_image, double angle,
              int64_t bgd_color, int64_t /*ignore_transparent*/ /* = 0 */) {
  gdImagePtr im_src = get_valid_image_resource(source_image);
  if (!im_src) return false;
  gdImagePtr im_dst = gdImageRotateInterpolated(im_src, angle, bgd_color);
  if (!im_dst) return false;
  return Variant(req::make<Image>(im_dst));
}