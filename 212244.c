static int exif_file_sections_free(image_info_type *ImageInfo) {
  int i;

  if (ImageInfo->file.count) {
    for (i=0; i<ImageInfo->file.count; i++) {
       if (ImageInfo->file.list[i].data) {
         IM_FREE(ImageInfo->file.list[i].data);
       }
    }
  }
  if (ImageInfo->file.list) IM_FREE(ImageInfo->file.list);
  ImageInfo->file.count = 0;
  return 1;
}