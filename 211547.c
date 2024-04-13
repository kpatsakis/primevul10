String HHVM_FUNCTION(image_type_to_mime_type, int64_t imagetype) {
  switch (imagetype) {
    case IMAGE_FILETYPE_GIF:
      return "image/gif";
    case IMAGE_FILETYPE_JPEG:
      return "image/jpeg";
    case IMAGE_FILETYPE_PNG:
      return "image/png";
    case IMAGE_FILETYPE_SWF:
    case IMAGE_FILETYPE_SWC:
      return "application/x-shockwave-flash";
    case IMAGE_FILETYPE_PSD:
      return "image/psd";
    case IMAGE_FILETYPE_BMP:
      return "image/x-ms-bmp";
    case IMAGE_FILETYPE_TIFF_II:
    case IMAGE_FILETYPE_TIFF_MM:
      return "image/tiff";
    case IMAGE_FILETYPE_IFF:
      return "image/iff";
    case IMAGE_FILETYPE_WBMP:
      return "image/vnd.wap.wbmp";
    case IMAGE_FILETYPE_JPC:
      return "application/octet-stream";
    case IMAGE_FILETYPE_JP2:
      return "image/jp2";
    case IMAGE_FILETYPE_XBM:
      return "image/xbm";
    case IMAGE_FILETYPE_ICO:
      return "image/vnd.microsoft.icon";
    default:
    case IMAGE_FILETYPE_UNKNOWN:
      return "application/octet-stream"; /* suppose binary format */
  }
}