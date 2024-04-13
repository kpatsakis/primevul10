exif_process_unicode(image_info_type* /*ImageInfo*/, xp_field_type* xp_field,
                     int tag, char* szValuePtr, int ByteCount) {
  xp_field->tag = tag;
  xp_field->value = nullptr;

  /* Copy the comment */
  xp_field->size =
    exif_process_string_raw(&xp_field->value, szValuePtr, ByteCount);
  return xp_field->size;
}