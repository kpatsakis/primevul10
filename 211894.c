static String exif_get_sectionname(int section) {
  switch(section) {
  case SECTION_FILE:      return s_FILE;
  case SECTION_COMPUTED:  return s_COMPUTED;
  case SECTION_ANY_TAG:   return s_ANY_TAG;
  case SECTION_IFD0:      return s_IFD0;
  case SECTION_THUMBNAIL: return s_THUMBNAIL;
  case SECTION_COMMENT:   return s_COMMENT;
  case SECTION_APP0:      return s_APP0;
  case SECTION_EXIF:      return s_EXIF;
  case SECTION_FPIX:      return s_FPIX;
  case SECTION_GPS:       return s_GPS;
  case SECTION_INTEROP:   return s_INTEROP;
  case SECTION_APP12:     return s_APP12;
  case SECTION_WINXP:     return s_WINXP;
  case SECTION_MAKERNOTE: return s_MAKERNOTE;
  }
  return empty_string();
}