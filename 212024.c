static int exif_file_sections_add(image_info_type *ImageInfo, int type,
                                  size_t size, unsigned char *data) {
  file_section *tmp;
  int count = ImageInfo->file.count;
  size_t realloc_size = (count+1) * sizeof(file_section);
  tmp = (file_section *)IM_REALLOC(ImageInfo->file.list, realloc_size);
  CHECK_ALLOC_R(tmp, realloc_size, -1);
  ImageInfo->file.list = tmp;
  ImageInfo->file.list[count].type = 0xFFFF;
  ImageInfo->file.list[count].data = nullptr;
  ImageInfo->file.list[count].size = 0;
  ImageInfo->file.count = count+1;
  if (!size) {
    data = nullptr;
  } else if (data == nullptr) {
    data = (unsigned char *)IM_MALLOC(size);
    if (data == nullptr) IM_FREE(tmp);
    CHECK_ALLOC_R(data, size, -1);
  }
  ImageInfo->file.list[count].type = type;
  ImageInfo->file.list[count].data = data;
  ImageInfo->file.list[count].size = size;
  return count;
}