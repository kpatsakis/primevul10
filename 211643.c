static int exif_scan_FILE_header(image_info_type *ImageInfo) {
  unsigned char *file_header;
  int ret = 0;

  ImageInfo->FileType = IMAGE_FILETYPE_UNKNOWN;

  if (ImageInfo->FileSize >= 2) {
    ImageInfo->infile->seek(0, SEEK_SET);
    String fileHeader = ImageInfo->infile->read(2);
    if (fileHeader.length() != 2) {
      return 0;
    }
    file_header = (unsigned char *)fileHeader.c_str();
    if ((file_header[0]==0xff) && (file_header[1]==M_SOI)) {
      ImageInfo->FileType = IMAGE_FILETYPE_JPEG;
      if (exif_scan_JPEG_header(ImageInfo)) {
        ret = 1;
      } else {
        raise_warning("Invalid JPEG file");
      }
    } else if (ImageInfo->FileSize >= 8) {
      String str = ImageInfo->infile->read(6);
      if (str.length() != 6) {
        return 0;
      }
      fileHeader += str;
      file_header = (unsigned char *)fileHeader.c_str();
      if (!memcmp(file_header, "II\x2A\x00", 4)) {
        ImageInfo->FileType = IMAGE_FILETYPE_TIFF_II;
        ImageInfo->motorola_intel = 0;
        ImageInfo->sections_found |= FOUND_IFD0;
        if (exif_process_IFD_in_TIFF(ImageInfo,
            php_ifd_get32u(file_header + 4, ImageInfo->motorola_intel),
                           SECTION_IFD0)) {
          ret = 1;
        } else {
          raise_warning("Invalid TIFF file");
        }
      } else if (!memcmp(file_header, "MM\x00\x2a", 4)) {
        ImageInfo->FileType = IMAGE_FILETYPE_TIFF_MM;
        ImageInfo->motorola_intel = 1;
        ImageInfo->sections_found |= FOUND_IFD0;
        if (exif_process_IFD_in_TIFF(ImageInfo,
            php_ifd_get32u(file_header + 4, ImageInfo->motorola_intel),
                           SECTION_IFD0)) {
          ret = 1;
        } else {
          raise_warning("Invalid TIFF file");
        }
      } else {
        raise_warning("File not supported");
        return 0;
      }
    }
  } else {
    raise_warning("File too small (%lu)", ImageInfo->FileSize);
  }
  return ret;
}