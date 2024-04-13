Variant HHVM_FUNCTION(imagecreatefromstring, const String& data) {
  gdImagePtr im;
  int imtype;
  char sig[8];

  if (data.length() < 8) {
    raise_warning("Empty string or invalid image");
    return false;
  }
  memcpy(sig, data.c_str(), 8);
  imtype = _php_image_type(sig);
  switch (imtype) {
  case PHP_GDIMG_TYPE_JPG:
#ifdef HAVE_GD_JPG
    im = _php_image_create_from_string(data, "JPEG",
      (gdImagePtr(*)())gdImageCreateFromJpegCtx);
#else
    raise_warning("No JPEG support");
    return false;
#endif
    break;

  case PHP_GDIMG_TYPE_PNG:
#ifdef HAVE_GD_PNG
    im = _php_image_create_from_string(data, "PNG",
      (gdImagePtr(*)())gdImageCreateFromPngCtx);
#else
    raise_warning("No PNG support");
    return false;
#endif
    break;

  case PHP_GDIMG_TYPE_WEBP:
#ifdef HAVE_LIBVPX
    im = _php_image_create_from_string(data, "WEBP",
      (gdImagePtr(*)())gdImageCreateFromWebpCtx);
#else
    raise_warning("No webp support (libvpx is needed)");
    return false;
#endif
    break;

  case PHP_GDIMG_TYPE_GIF:
    im = _php_image_create_from_string(data, "GIF",
      (gdImagePtr(*)())gdImageCreateFromGifCtx);
    break;

  case PHP_GDIMG_TYPE_WBM:
    im = _php_image_create_from_string(data, "WBMP",
      (gdImagePtr(*)())gdImageCreateFromWBMPCtx);
    break;

  case PHP_GDIMG_TYPE_GD2:
    im = _php_image_create_from_string(data, "GD2",
      (gdImagePtr(*)())gdImageCreateFromGd2Ctx);
    break;

  default:
    raise_warning("Data is not in a recognized format");
    return false;
  }

  if (!im) {
    raise_warning("Couldn't create GD Image Stream out of Data");
    return false;
  }
  return Variant(req::make<Image>(im));
}