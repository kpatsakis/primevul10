Variant HHVM_FUNCTION(imagecolormatch, const Resource& image1,
                                       const Resource& image2) {
  gdImagePtr im1 = get_valid_image_resource(image1);
  if (!im1) return false;
  gdImagePtr im2 = get_valid_image_resource(image2);
  if (!im2) return false;
  int result;

  result = gdImageColorMatch(im1, im2);
  switch (result) {
  case -1:
    raise_warning("Image1 must be TrueColor");
    return false;
  case -2:
    raise_warning("Image2 must be Palette");
    return false;
  case -3:
    raise_warning("Image1 and Image2 must be the same size");
    return false;
  case -4:
    raise_warning("Image2 must have at least one color");
    return false;
  }

  return true;
}