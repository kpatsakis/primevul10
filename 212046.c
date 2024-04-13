bool HHVM_FUNCTION(imagesavealpha, const Resource& image, bool saveflag) {
  gdImagePtr im = get_valid_image_resource(image);
  if (!im) return false;
  gdImageSaveAlpha(im, saveflag);
  return true;
}