static int php_getimagetype(const req::ptr<File>& file) {
  String fileType = file->read(3);
  if (fileType.length() != 3) {
    raise_notice("Read error!");
    return IMAGE_FILETYPE_UNKNOWN;
  }

  /* BYTES READ: 3 */
  if (!memcmp(fileType.c_str(), php_sig_gif, 3)) {
    return IMAGE_FILETYPE_GIF;
  } else if (!memcmp(fileType.c_str(), php_sig_jpg, 3)) {
    return IMAGE_FILETYPE_JPEG;
  } else if (!memcmp(fileType.c_str(), php_sig_png, 3)) {
    String data = file->read(5);
    if (data.length() != 5) {
      raise_notice("Read error!");
      return IMAGE_FILETYPE_UNKNOWN;
    }
    if (!memcmp((fileType + data).c_str(), php_sig_png, 8)) {
      return IMAGE_FILETYPE_PNG;
    } else {
      raise_warning("PNG file corrupted by ASCII conversion");
      return IMAGE_FILETYPE_UNKNOWN;
    }
  } else if (!memcmp(fileType.c_str(), php_sig_swf, 3)) {
    return IMAGE_FILETYPE_SWF;
  } else if (!memcmp(fileType.c_str(), php_sig_swc, 3)) {
    return IMAGE_FILETYPE_SWC;
  } else if (!memcmp(fileType.c_str(), php_sig_psd, 3)) {
    return IMAGE_FILETYPE_PSD;
  } else if (!memcmp(fileType.c_str(), php_sig_bmp, 2)) {
    return IMAGE_FILETYPE_BMP;
  } else if (!memcmp(fileType.c_str(), php_sig_jpc, 3)) {
    return IMAGE_FILETYPE_JPC;
  }

  String data = file->read(1);
  if (data.length() != 1) {
    raise_notice("Read error!");
    return IMAGE_FILETYPE_UNKNOWN;
  }

  /* BYTES READ: 4 */
  fileType += data;
  if (!memcmp(fileType.c_str(), php_sig_tif_ii, 4)) {
    return IMAGE_FILETYPE_TIFF_II;
  } else if (!memcmp(fileType.c_str(), php_sig_tif_mm, 4)) {
    return IMAGE_FILETYPE_TIFF_MM;
  } else if (!memcmp(fileType.c_str(), php_sig_iff, 4)) {
    return IMAGE_FILETYPE_IFF;
  } else if (!memcmp(fileType.c_str(), php_sig_ico, 4)) {
    return IMAGE_FILETYPE_ICO;
  }

  data = file->read(8);
  if (data.length() != 8) {
    raise_notice("Read error!");
    return IMAGE_FILETYPE_UNKNOWN;
  }

  /* BYTES READ: 12 */
  fileType += data;
  if (!memcmp(fileType.c_str(), php_sig_jp2, 12)) {
    return IMAGE_FILETYPE_JP2;
  }

  /* AFTER ALL ABOVE FAILED */
  if (php_get_wbmp(file, nullptr, 1)) {
    return IMAGE_FILETYPE_WBMP;
  }
  if (php_get_xbm(file, nullptr)) {
    return IMAGE_FILETYPE_XBM;
  }
  return IMAGE_FILETYPE_UNKNOWN;
}