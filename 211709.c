bool HHVM_FUNCTION(imagedestroy, const Resource& image) {
  auto img_res = cast<Image>(image);
  gdImagePtr im = img_res->get();
  if (!im) return false;
  img_res->reset();
  return true;
}