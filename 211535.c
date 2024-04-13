Variant HHVM_FUNCTION(imagetruecolortopalette, const Resource& image,
    bool dither, int64_t ncolors) {
  gdImagePtr im = get_valid_image_resource(image);
  if (!im) return false;

  if (ncolors <= 0 || ncolors >= INT_MAX) {
    raise_warning("Number of colors has to be greater than zero");
    return false;
  }
  gdImageTrueColorToPalette(im, dither, ncolors);
  return true;
}