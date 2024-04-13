Variant HHVM_FUNCTION(imagescale, const Resource& image, int64_t newwidth,
  int64_t newheight /* =-1 */, int64_t method /*=GD_BILINEAR_FIXED*/) {
  gdImagePtr im = get_valid_image_resource(image);
  if (!im) return false;
  gdImagePtr imscaled = nullptr;
  gdInterpolationMethod old_method;
  if (method == -1) method = GD_BILINEAR_FIXED;

  if (newheight < 0) {
    /* preserve ratio */
    long src_x, src_y;
    src_x = gdImageSX(im);
    src_y = gdImageSY(im);
    if (src_x) {
      newheight = newwidth * src_y / src_x;
    }
  }
  if (newheight <= 0 || newheight > INT_MAX || newwidth <= 0 || newwidth > INT_MAX) {
    return false;
  }

  old_method = im->interpolation_id;
  if (gdImageSetInterpolationMethod(im, (gdInterpolationMethod) method)) {
    imscaled = gdImageScale(im, newwidth, newheight);
  }
  gdImageSetInterpolationMethod(im, old_method);

  if (imscaled == nullptr) {
    return false;
  }
  return Variant(req::make<Image>(imscaled));
}