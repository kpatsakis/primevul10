php_image_filter_brightness(gdImagePtr im, int arg1 /* = 0 */,
                            int /*arg2*/ /* = 0 */, int /*arg3*/ /* = 0 */,
                            int /*arg4*/ /* = 0 */) {
  int brightness = arg1;
  return gdImageBrightness(im, brightness) == 1;
}