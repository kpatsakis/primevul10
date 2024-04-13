static int exif_discard_imageinfo(image_info_type *ImageInfo) {
  int i;

  if (ImageInfo->UserComment) IM_FREE(ImageInfo->UserComment);
  if (ImageInfo->UserCommentEncoding) {
    IM_FREE(ImageInfo->UserCommentEncoding);
  }
  if (ImageInfo->Copyright) IM_FREE(ImageInfo->Copyright);
  if (ImageInfo->CopyrightPhotographer) {
    IM_FREE(ImageInfo->CopyrightPhotographer);
  }
  if (ImageInfo->CopyrightEditor) IM_FREE(ImageInfo->CopyrightEditor);
  if (ImageInfo->Thumbnail.data) IM_FREE(ImageInfo->Thumbnail.data);
  if (ImageInfo->encode_unicode) IM_FREE(ImageInfo->encode_unicode);
  if (ImageInfo->decode_unicode_be) {
    IM_FREE(ImageInfo->decode_unicode_be);
  }
  if (ImageInfo->decode_unicode_le) {
    IM_FREE(ImageInfo->decode_unicode_le);
  }
  if (ImageInfo->encode_jis) IM_FREE(ImageInfo->encode_jis);
  if (ImageInfo->decode_jis_be) IM_FREE(ImageInfo->decode_jis_be);
  if (ImageInfo->decode_jis_le) IM_FREE(ImageInfo->decode_jis_le);
  if (ImageInfo->make) IM_FREE(ImageInfo->make);
  if (ImageInfo->model) IM_FREE(ImageInfo->model);
  for (i=0; i<ImageInfo->xp_fields.count; i++) {
    if (ImageInfo->xp_fields.list[i].value) {
      IM_FREE(ImageInfo->xp_fields.list[i].value);
    }
  }
  if (ImageInfo->xp_fields.list) IM_FREE(ImageInfo->xp_fields.list);
  for (i=0; i<SECTION_COUNT; i++) {
    exif_iif_free(ImageInfo, i);
  }
  exif_file_sections_free(ImageInfo);
  memset(ImageInfo, 0, sizeof(*ImageInfo));
  return 1;
}