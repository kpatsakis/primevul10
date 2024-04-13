int64_t HHVM_FUNCTION(imagetypes) {
  int ret=0;
  ret = IMAGE_TYPE_GIF;
#ifdef HAVE_GD_JPG
  ret |= IMAGE_TYPE_JPEG;
#endif
#ifdef HAVE_GD_PNG
  ret |= IMAGE_TYPE_PNG;
#endif
  ret |= IMAGE_TYPE_WBMP;
#if defined(HAVE_GD_XPM) && defined(HAVE_GD_BUNDLED)
  ret |= IMAGE_TYPE_XPM;
#endif
  return ret;
}