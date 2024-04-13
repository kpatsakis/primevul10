static int exif_read_file(image_info_type *ImageInfo, String FileName,
                          bool read_thumbnail, bool read_all) {
  struct stat st;

  /* Start with an empty image information structure. */
  memset(ImageInfo, 0, sizeof(*ImageInfo));

  ImageInfo->motorola_intel = -1; /* flag as unknown */

  ImageInfo->infile = File::Open(FileName, "rb");
  if (!ImageInfo->infile) {
    raise_warning("Unable to open file %s", FileName.c_str());
    return 0;
  }
  auto plain_file = dyn_cast<PlainFile>(ImageInfo->infile);
  if (plain_file) {
    if (stat(FileName.c_str(), &st) >= 0) {
      if ((st.st_mode & S_IFMT) != S_IFREG) {
        raise_warning("Not a file");
        return 0;
      }
    }

    /* Store file date/time. */
    ImageInfo->FileDateTime = st.st_mtime;
    ImageInfo->FileSize = st.st_size;
  } else {
    if (!ImageInfo->FileSize) {
      ImageInfo->infile->seek(0, SEEK_END);
      ImageInfo->FileSize = ImageInfo->infile->tell();
      ImageInfo->infile->seek(0, SEEK_SET);
    }
  }

  ImageInfo->FileName = HHVM_FN(basename)(FileName);
  ImageInfo->read_thumbnail = read_thumbnail;
  ImageInfo->read_all = read_all;
  ImageInfo->Thumbnail.filetype = IMAGE_FILETYPE_UNKNOWN;

  PHP_STRDUP(ImageInfo->encode_unicode,    "ISO-8859-15");
  PHP_STRDUP(ImageInfo->decode_unicode_be, "UCS-2BE");
  PHP_STRDUP(ImageInfo->decode_unicode_le, "UCS-2LE");
  PHP_STRDUP(ImageInfo->encode_jis,        "");
  PHP_STRDUP(ImageInfo->decode_jis_be,     "JIS");
  PHP_STRDUP(ImageInfo->decode_jis_le,     "JIS");

  ImageInfo->ifd_nesting_level = 0;

  /* Scan the JPEG headers. */
  auto ret = exif_scan_FILE_header(ImageInfo);

  ImageInfo->infile->close();
  return ret;
}