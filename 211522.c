bool HHVM_FUNCTION(imagefilter, const Resource& res,
    int64_t filtertype,
    const Variant& arg1 /*=0*/, const Variant& arg2 /*=0*/,
    const Variant& arg3 /*=0*/, const Variant& arg4 /*=0*/) {
  gdImagePtr im = get_valid_image_resource(res);
  if (!im) return false;

/* Exists purely to mirror PHP5's invalid arg logic for this function */
#define IMFILT_TYPECHK(n) \
  if (!arg##n.isBoolean() && !arg##n.isNumeric(true)) { \
    raise_warning("imagefilter() expected boolean/numeric for argument %d", \
                  (n+2)); \
    return false; \
  }
  IMFILT_TYPECHK(1)
  IMFILT_TYPECHK(2)
  IMFILT_TYPECHK(3)
  IMFILT_TYPECHK(4)
#undef IMFILT_TYPECHECK

  using image_filter = bool (*)(gdImagePtr, int, int, int, int);
  image_filter filters[] = {
    php_image_filter_negate,
    php_image_filter_grayscale,
    php_image_filter_brightness,
    php_image_filter_contrast,
    php_image_filter_colorize,
    php_image_filter_edgedetect,
    php_image_filter_emboss,
    php_image_filter_gaussian_blur,
    php_image_filter_selective_blur,
    php_image_filter_mean_removal,
    php_image_filter_smooth,
    php_image_filter_pixelate,
  };
  auto const num_filters = sizeof(filters) / sizeof(image_filter);
  if (filtertype >= 0 && filtertype < num_filters) {
    return filters[filtertype](im, arg1.toInt64(), arg2.toInt64(),
                                          arg3.toInt64(), arg4.toInt64());
  }
  return false;
}