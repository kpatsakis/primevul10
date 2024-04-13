php_image_filter_emboss(gdImagePtr im, int /*arg1*/ /* = 0 */,
                        int /*arg2*/ /* = 0 */, int /*arg3*/ /* = 0 */,
                        int /*arg4*/ /* = 0 */) {
  return gdImageEmboss(im) == 1;
}