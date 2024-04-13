bool HHVM_FUNCTION(imagegammacorrect, const Resource& image,
    double inputgamma, double outputgamma) {
  gdImagePtr im = get_valid_image_resource(image);
  if (!im) return false;
  if (inputgamma <= 0.0 || outputgamma <= 0.0) {
    raise_warning("Gamma values should be positive");
    return false;
  }
  if (gdImageTrueColor(im))   {
    int x, y, c;

    for (y = 0; y < gdImageSY(im); y++) {
      for (x = 0; x < gdImageSX(im); x++) {
        c = gdImageGetPixel(im, x, y);
        gdImageSetPixel(im, x, y,
          gdTrueColor((int)((pow((pow((gdTrueColorGetRed(c)/255.0),
                      inputgamma)),1.0/outputgamma)*255) + .5),
                      (int)((pow((pow((gdTrueColorGetGreen(c)/255.0),
                      inputgamma)),1.0/outputgamma) * 255) + .5),
                      (int)((pow((pow((gdTrueColorGetBlue(c)/255.0),
                      inputgamma)),1.0/outputgamma) * 255) + .5)));
      }
    }
    return true;
  }
  for (int i = 0; i < gdImageColorsTotal(im); i++) {
    im->red[i] = (int)((pow((pow((im->red[i]/255.0), inputgamma)),
                        1.0/outputgamma)*255) + .5);
    im->green[i] = (int)((pow((pow((im->green[i]/255.0), inputgamma)),
                          1.0/outputgamma)*255) + .5);
    im->blue[i] = (int)((pow((pow((im->blue[i]/255.0), inputgamma)),
                         1.0/outputgamma)*255) + .5);
  }

  return true;
}