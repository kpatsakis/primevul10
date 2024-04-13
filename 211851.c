Variant HHVM_FUNCTION(imagecolorset, const Resource& image,
    int64_t index, int64_t red, int64_t green, int64_t blue) {
  gdImagePtr im = get_valid_image_resource(image);
  if (!im) return false;
  if (index >= 0 && index < gdImageColorsTotal(im)) {
    im->red[index] = red;
    im->green[index] = green;
    im->blue[index] = blue;
    return true;
  } else {
    return false;
  }
}