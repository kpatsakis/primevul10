bool HHVM_FUNCTION(imagecopyresized, const Resource& dst_im,
    const Resource& src_im,
    int64_t dst_x, int64_t dst_y, int64_t src_x, int64_t src_y,
    int64_t dst_w, int64_t dst_h, int64_t src_w, int64_t src_h) {
  gdImagePtr im_src = cast<Image>(src_im)->get();
  if (!im_src) return false;
  gdImagePtr im_dst = cast<Image>(dst_im)->get();
  if (!im_dst) return false;
  if (dst_w <= 0 || dst_h <= 0 || src_w <= 0 || src_h <= 0) {
    raise_warning("Invalid image dimensions");
    return false;
  }
  gdImageCopyResized(im_dst, im_src,
                     dst_x, dst_y, src_x, src_y,
                     dst_w, dst_h, src_w, src_h);
  return true;
}