static struct gfxinfo *php_handle_xbm(const req::ptr<File>& stream) {
  struct gfxinfo *result;
  php_get_xbm(stream, &result);
  return result;
}