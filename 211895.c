Variant HHVM_FUNCTION(image_type_to_extension,
                     int64_t imagetype, bool include_dot /*=true */) {
  switch (imagetype) {
  case IMAGE_FILETYPE_GIF:
    return include_dot ? String(".gif") : String("gif");
  case IMAGE_FILETYPE_JPEG:
    return include_dot ? String(".jpeg") : String("jpeg");
  case IMAGE_FILETYPE_PNG:
    return include_dot ? String(".png") : String("png");
  case IMAGE_FILETYPE_SWF:
  case IMAGE_FILETYPE_SWC:
    return include_dot ? String(".swf") : String("swf");
  case IMAGE_FILETYPE_PSD:
    return include_dot ? String(".psd") : String("psd");
  case IMAGE_FILETYPE_BMP:
  case IMAGE_FILETYPE_WBMP:
    return include_dot ? String(".bmp") : String("bmp");
  case IMAGE_FILETYPE_TIFF_II:
  case IMAGE_FILETYPE_TIFF_MM:
    return include_dot ? String(".tiff") : String("tiff");
  case IMAGE_FILETYPE_IFF:
    return include_dot ? String(".iff") : String("iff");
  case IMAGE_FILETYPE_JPC:
    return include_dot ? String(".jpc") : String("jpc");
  case IMAGE_FILETYPE_JP2:
    return include_dot ? String(".jp2") : String("jp2");
  case IMAGE_FILETYPE_JPX:
    return include_dot ? String(".jpx") : String("jpx");
  case IMAGE_FILETYPE_JB2:
    return include_dot ? String(".jb2") : String("jb2");
  case IMAGE_FILETYPE_XBM:
    return include_dot ? String(".xbm") : String("xbm");
  case IMAGE_FILETYPE_ICO:
    return include_dot ? String(".ico") : String("ico");
  default:
    return false;
  }
}