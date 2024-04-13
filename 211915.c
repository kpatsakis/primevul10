static void exif_thumbnail_extract(image_info_type *ImageInfo,
                                   char *offset, size_t length) {
  if (ImageInfo->Thumbnail.data) {
    raise_warning("Multiple possible thumbnails");
    return; /* Should not happen */
  }
  if (!ImageInfo->read_thumbnail) {
    return; /* ignore this call */
  }
  /* according to exif2.1, the thumbnail is not supposed to be greater
     than 64K */
  if (ImageInfo->Thumbnail.size >= 65536 ||
      ImageInfo->Thumbnail.size <= 0 ||
      ImageInfo->Thumbnail.offset <= 0) {
    raise_warning("Illegal thumbnail size/offset");
    return;
  }
  /* Check to make sure we are not going to go past the ExifLength */
  if ((ImageInfo->Thumbnail.offset + ImageInfo->Thumbnail.size) > length) {
    raise_warning("Thumbnail goes IFD boundary or end of file reached");
    return;
  }
  PHP_STRNDUP(ImageInfo->Thumbnail.data, offset + ImageInfo->Thumbnail.offset,
              ImageInfo->Thumbnail.size);
  exif_thumbnail_build(ImageInfo);
}