Variant HHVM_FUNCTION(exif_thumbnail, const String& filename,
                         int64_t& width,
                         int64_t& height,
                         int64_t& imagetype) {
  image_info_type ImageInfo;

  memset(&ImageInfo, 0, sizeof(ImageInfo));

  int ret = exif_read_file(&ImageInfo, filename.c_str(), 1, 0);
  if (ret==0) {
    exif_discard_imageinfo(&ImageInfo);
    return false;
  }

  if (!ImageInfo.Thumbnail.data || !ImageInfo.Thumbnail.size) {
    exif_discard_imageinfo(&ImageInfo);
    return false;
  }

  if (!ImageInfo.Thumbnail.width || !ImageInfo.Thumbnail.height) {
    if (!exif_scan_thumbnail(&ImageInfo)) {
      ImageInfo.Thumbnail.width = ImageInfo.Thumbnail.height = 0;
    }
  }
  width = ImageInfo.Thumbnail.width;
  height = ImageInfo.Thumbnail.height;
  imagetype = ImageInfo.Thumbnail.filetype;
  String str(ImageInfo.Thumbnail.data, ImageInfo.Thumbnail.size, CopyString);
  exif_discard_imageinfo(&ImageInfo);
  return str;
}