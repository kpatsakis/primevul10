Variant HHVM_FUNCTION(getimagesize, const String& filename,
                      Array& imageinfo) {
  if (auto stream = File::Open(filename, "rb")) {
    return getImageSize(stream, imageinfo);
  }
  return false;
}