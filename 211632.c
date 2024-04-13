php_image_filter_pixelate(gdImagePtr im, int arg1 /* = 0 */, int arg2 /* = 0 */,
                          int /*arg3*/ /* = 0 */, int /*arg4*/ /* = 0 */) {
  int blocksize = arg1;
  unsigned mode = arg2;
  return gdImagePixelate(im, blocksize, mode) == 1;
}