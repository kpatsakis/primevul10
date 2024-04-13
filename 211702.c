Variant HHVM_FUNCTION(imagecolorat,
    const Resource& image, int64_t x, int64_t y) {
  gdImagePtr im = get_valid_image_resource(image);
  if (!im) return false;
  if (gdImageTrueColor(im)) {
    if (im->tpixels && gdImageBoundsSafe(im, x, y)) {
      return gdImageTrueColorPixel(im, x, y);
    } else {
      raise_notice("%" PRId64 ",%" PRId64 " is out of bounds", x, y);
      return false;
    }
  } else {
    if (im->pixels && gdImageBoundsSafe(im, x, y)) {
      return (im->pixels[y][x]);
    } else {
      raise_notice("%" PRId64 ",%" PRId64 " is out of bounds", x, y);
      return false;
    }
  }
}