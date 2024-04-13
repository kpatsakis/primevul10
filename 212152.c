static void exif_process_COM(image_info_type *image_info, char *value,
                             size_t length) {
  exif_iif_add_tag(image_info, SECTION_COMMENT, "Comment",
                   TAG_COMPUTED_VALUE, TAG_FMT_STRING,
                   length-2, value+2);
}