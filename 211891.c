TypedValue HHVM_FUNCTION(imageinterlace, const Resource& image,
                         TypedValue interlace /* = 0 */) {
  gdImagePtr im = get_valid_image_resource(image);
  if (!im) return make_tv<KindOfBoolean>(false);
  if (!tvIsNull(interlace)) {
    // has interlace argument
    gdImageInterlace(im, tvAssertInt(interlace));
  }
  return make_tv<KindOfInt64>(gdImageGetInterlaced(im));
}