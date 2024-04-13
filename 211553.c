bool f_imageistruecolor(const Resource& image) {
  gdImagePtr im = get_valid_image_resource(image);
  if (!im) return false;
  return im->trueColor;
}