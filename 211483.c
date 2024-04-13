static int exif_file_sections_realloc(image_info_type *ImageInfo,
                                      int section_index, size_t size) {
  void *tmp;

  /* This is not a malloc/realloc check. It is a plausibility check for the
   * function parameters (requirements engineering).
   */
  if (section_index >= ImageInfo->file.count) {
    raise_warning("Illegal reallocating of undefined file section");
    return -1;
  }
  tmp = IM_REALLOC(ImageInfo->file.list[section_index].data, size);
  CHECK_ALLOC_R(tmp, size, -1);
  ImageInfo->file.list[section_index].data = (unsigned char *)tmp;
  ImageInfo->file.list[section_index].size = size;
  return 0;
}