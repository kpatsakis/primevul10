static int hphp_gdImageConvolution(gdImagePtr src, float filter[3][3],
                                   float filter_div, float offset) {
  int x, y, i, j, new_a;
  float new_r, new_g, new_b;
  int new_pxl, pxl=0;
  gdImagePtr srcback;

  if (src==nullptr) {
    return 0;
  }

  /* We need the orinal image with each safe neoghb. pixel */
  srcback = gdImageCreateTrueColor (src->sx, src->sy);
  gdImageCopy(srcback, src,0,0,0,0,src->sx,src->sy);

  if (srcback==nullptr) {
    return 0;
  }

  for ( y=0; y<src->sy; y++) {
    for(x=0; x<src->sx; x++) {
      new_r = new_g = new_b = 0;
      new_a = gdImageAlpha(srcback, pxl);

      for (j=0; j<3; j++) {
        int yv = std::min(std::max(y - 1 + j, 0), src->sy - 1);
        for (i=0; i<3; i++) {
          pxl = gdImageGetPixel(srcback, std::min(std::max(x - 1 + i, 0),
                                                  src->sx - 1), yv);
          new_r += (float)gdImageRed(srcback, pxl) * filter[j][i];
          new_g += (float)gdImageGreen(srcback, pxl) * filter[j][i];
          new_b += (float)gdImageBlue(srcback, pxl) * filter[j][i];
        }
      }

      new_r = (new_r/filter_div)+offset;
      new_g = (new_g/filter_div)+offset;
      new_b = (new_b/filter_div)+offset;

      new_r = (new_r > 255.0f)? 255.0f : ((new_r < 0.0f)? 0.0f:new_r);
      new_g = (new_g > 255.0f)? 255.0f : ((new_g < 0.0f)? 0.0f:new_g);
      new_b = (new_b > 255.0f)? 255.0f : ((new_b < 0.0f)? 0.0f:new_b);

      new_pxl = gdImageColorAllocateAlpha(src, (int)new_r, (int)new_g,
                                          (int)new_b, new_a);
      if (new_pxl == -1) {
        new_pxl = gdImageColorClosestAlpha(src, (int)new_r, (int)new_g,
                                          (int)new_b, new_a);
      }
      gdImageSetPixel (src, x, y, new_pxl);
    }
  }
  gdImageDestroy(srcback);
  return 1;
}