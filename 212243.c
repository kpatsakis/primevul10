Variant HHVM_FUNCTION(imagecolortransparent, const Resource& image,
                                             int64_t color /* = -1 */) {
  gdImagePtr im = get_valid_image_resource(image);
  if (!im) return false;
  if (color != -1) {
    // has color argument
    gdImageColorTransparent(im, color);
  }
  return gdImageGetTransparent(im);
}