bool HHVM_FUNCTION(imageflip, const Resource& image, int64_t mode /* = -1 */) {
  gdImagePtr im = get_valid_image_resource(image);
  if (!im) return false;
  if (mode == -1) mode = GD_FLIP_HORINZONTAL;

  switch (mode) {
    case GD_FLIP_VERTICAL:
      gdImageFlipVertical(im);
      break;

    case GD_FLIP_HORINZONTAL:
      gdImageFlipHorizontal(im);
      break;

    case GD_FLIP_BOTH:
      gdImageFlipBoth(im);
      break;

    default:
      raise_warning("imageflip(): Unknown flip mode");
      return false;
  }

  return true;
}