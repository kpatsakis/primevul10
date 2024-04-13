php_image_filter_smooth(gdImagePtr im, int arg1 /* = 0 */,
                        int /*arg2*/ /* = 0 */, int /*arg3*/ /* = 0 */,
                        int /*arg4*/ /* = 0 */) {
  int weight = arg1;
  return gdImageSmooth(im, weight) == 1;
}