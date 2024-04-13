php_image_filter_colorize(gdImagePtr im, int arg1 /* = 0 */, int arg2 /* = 0 */,
                          int arg3 /* = 0 */, int /*arg4*/ /* = 0 */) {
  int r = arg1;
  int g = arg2;
  int b = arg3;
  int a = arg1;
  return gdImageColor(im, r, g, b, a) == 1;
}