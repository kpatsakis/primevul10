static struct gfxinfo *php_handle_swf(const req::ptr<File>& stream) {
  struct gfxinfo *result = nullptr;
  long bits;
  unsigned char *a;

  if (!stream->seek(5, SEEK_CUR)) return nullptr;

  String str = stream->read(32);
  if (str.length() != 32) return nullptr;
  a = (unsigned char *)str.c_str();
  result = (struct gfxinfo *)IM_CALLOC(1, sizeof (struct gfxinfo));
  CHECK_ALLOC_R(result, sizeof (struct gfxinfo), nullptr);
  bits = php_swf_get_bits (a, 0, 5);
  result->width = (php_swf_get_bits (a, 5 + bits, bits) -
    php_swf_get_bits (a, 5, bits)) / 20;
  result->height = (php_swf_get_bits (a, 5 + (3 * bits), bits) -
    php_swf_get_bits (a, 5 + (2 * bits), bits)) / 20;
  result->bits = 0;
  result->channels = 0;
  return result;
}