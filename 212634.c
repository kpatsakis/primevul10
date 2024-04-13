void CopyToString(const tstring& src, tstring* dst) {
  if (src.type() == tstring::SMALL) {
    *dst = src;  // string buffer fits in the tstring object (under ~24 bytes)
  } else {
    dst->assign_as_view(src);
  }
}