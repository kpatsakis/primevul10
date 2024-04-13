static void exif_process_APP1(image_info_type *ImageInfo, char *CharBuf,
                              size_t length, size_t displacement) {
  /* Check the APP1 for Exif Identifier Code */
  char *end = CharBuf + length;
  static const
  unsigned char ExifHeader[] = {0x45, 0x78, 0x69, 0x66, 0x00, 0x00};
  CHECK_BUFFER(CharBuf+2, end, 6);
  if (length <= 8 || memcmp(CharBuf+2, ExifHeader, 6)) {
    raise_warning("Incorrect APP1 Exif Identifier Code");
    return;
  }
  exif_process_TIFF_in_JPEG(ImageInfo, CharBuf + 8, length - 8,
                            displacement+8);
}