static struct gfxinfo *php_handle_wbmp(const req::ptr<File>& stream) {
  struct gfxinfo *result =
   (struct gfxinfo *)IM_CALLOC(1, sizeof(struct gfxinfo));
  CHECK_ALLOC_R(result, (sizeof(struct gfxinfo)), nullptr);

  if (!php_get_wbmp(stream, &result, 0)) {
    IM_FREE(result);
    return nullptr;
  }

  return result;
}