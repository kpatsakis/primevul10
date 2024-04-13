Variant HHVM_FUNCTION(getimagesizefromstring, const String& imagedata,
                      Array& imageinfo) {
  String data = "data://text/plain;base64,";
  data += StringUtil::Base64Encode(imagedata);
  if (auto stream = File::Open(data, "r")) {
    return getImageSize(stream, imageinfo);
  }
  return false;
}