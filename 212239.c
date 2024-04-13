Variant HHVM_FUNCTION(exif_imagetype, const String& filename) {
  auto stream = File::Open(filename, "rb");
  if (!stream) {
    return false;
  }
  int itype = php_getimagetype(stream);
  stream->close();
  if (itype == IMAGE_FILETYPE_UNKNOWN) return false;
  return itype;
}