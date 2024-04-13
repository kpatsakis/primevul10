Variant HHVM_FUNCTION(exif_tagname, int64_t index) {
  char *szTemp;

  szTemp = exif_get_tagname(index, nullptr, 0, tag_table_IFD);
  if (index <0 || !szTemp || !szTemp[0]) {
    return false;
  } else {
    return String(szTemp, CopyString);
  }
}