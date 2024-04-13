php_image_filter_contrast(gdImagePtr im, int arg1 /* = 0 */,
                          int /*arg2*/ /* = 0 */, int /*arg3*/ /* = 0 */,
                          int /*arg4*/ /* = 0 */) {
  int contrast = arg1;
  return gdImageContrast(im, contrast) == 1;
}