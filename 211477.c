static bool _php_image_convert(const String& f_org, const String& f_dest,
                               int dest_height, int dest_width,
                               int threshold, int image_type) {
  gdImagePtr im_org, im_dest, im_tmp;
  req::ptr<File> org_file, dest_file;
  FILE *org, *dest;
  int org_height, org_width;
  int white, black;
  int color, color_org, median;
  int x, y;
  float x_ratio, y_ratio;
#ifdef HAVE_GD_JPG
  // long ignore_warning;
#endif

  /* Check threshold value */
  if (threshold < 0 || threshold > 8) {
    raise_warning("Invalid threshold value '%d'", threshold);
    return false;
  }

  /* Open origin file */
  org_file = php_open_plain_file(f_org, "rb", &org);
  if (!org_file) {
    return false;
  }

  /* Open destination file */
  dest_file = php_open_plain_file(f_dest, "wb", &dest);
  if (!dest_file) {
    return false;
  }

  switch (image_type) {
  case PHP_GDIMG_TYPE_GIF:
    im_org = gdImageCreateFromGif(org);
    if (im_org == nullptr) {
      raise_warning("Unable to open '%s' Not a valid GIF file",
                      f_org.c_str());
      return false;
    }
    break;

#ifdef HAVE_GD_JPG
  case PHP_GDIMG_TYPE_JPG:
    im_org = gdImageCreateFromJpeg(org);
    if (im_org == nullptr) {
      raise_warning("Unable to open '%s' Not a valid JPEG file",
                      f_org.c_str());
      return false;
    }
    break;
#endif /* HAVE_GD_JPG */


#ifdef HAVE_GD_PNG
  case PHP_GDIMG_TYPE_PNG:
    im_org = gdImageCreateFromPng(org);
    if (im_org == nullptr) {
      raise_warning("Unable to open '%s' Not a valid PNG file",
                      f_org.c_str());
      return false;
    }
    break;
#endif /* HAVE_GD_PNG */

#ifdef HAVE_LIBVPX
  case PHP_GDIMG_TYPE_WEBP:
    im_org = gdImageCreateFromWebp(org);
    if (im_org == nullptr) {
      raise_warning("Unable to open '%s' Not a valid webp file",
                      f_org.c_str());
      return false;
    }
    break;
#endif /* HAVE_LIBVPX */

  default:
    raise_warning("Format not supported");
    return false;
  }

  org_width  = gdImageSX (im_org);
  org_height = gdImageSY (im_org);

  x_ratio = (float) org_width / (float) dest_width;
  y_ratio = (float) org_height / (float) dest_height;

  if (x_ratio > 1 && y_ratio > 1) {
    if (y_ratio > x_ratio) {
      x_ratio = y_ratio;
    } else {
      y_ratio = x_ratio;
    }
    dest_width = (int) (org_width / x_ratio);
    dest_height = (int) (org_height / y_ratio);
  } else {
    x_ratio = (float) dest_width / (float) org_width;
    y_ratio = (float) dest_height / (float) org_height;

    if (y_ratio < x_ratio) {
      x_ratio = y_ratio;
    } else {
      y_ratio = x_ratio;
    }
    dest_width = (int) (org_width * x_ratio);
    dest_height = (int) (org_height * y_ratio);
  }

  im_tmp = gdImageCreate (dest_width, dest_height);
  if (im_tmp == nullptr) {
    raise_warning("Unable to allocate temporary buffer");
    return false;
  }

  gdImageCopyResized (im_tmp, im_org, 0, 0, 0, 0,
                      dest_width, dest_height, org_width, org_height);

  gdImageDestroy(im_org);

  org_file->close();

  im_dest = gdImageCreate(dest_width, dest_height);
  if (im_dest == nullptr) {
    raise_warning("Unable to allocate destination buffer");
    return false;
  }

  white = gdImageColorAllocate(im_dest, 255, 255, 255);
  if (white == -1) {
    raise_warning("Unable to allocate the colors for "
                    "the destination buffer");
    return false;
  }

  black = gdImageColorAllocate(im_dest, 0, 0, 0);
  if (black == -1) {
    raise_warning("Unable to allocate the colors for "
                    "the destination buffer");
    return false;
  }

  threshold = threshold * 32;

  for (y = 0; y < dest_height; y++) {
    for (x = 0; x < dest_width; x++) {
      color_org = gdImageGetPixel (im_tmp, x, y);
      median = (im_tmp->red[color_org] +
                im_tmp->green[color_org] +
                im_tmp->blue[color_org]) / 3;
      if (median < threshold) {
        color = black;
      } else {
        color = white;
      }
      gdImageSetPixel(im_dest, x, y, color);
    }
  }

  gdImageDestroy(im_tmp);

  gdImageWBMP(im_dest, black , dest);

  fflush(dest);
  dest_file->close();

  gdImageDestroy(im_dest);

  return true;
}