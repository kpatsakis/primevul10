bool HHVM_FUNCTION(imagecopymerge, const Resource& dst_im,
    const Resource& src_im,
    int64_t dst_x, int64_t dst_y, int64_t src_x, int64_t src_y,
    int64_t src_w, int64_t src_h, int64_t pct) {
  gdImagePtr im_src = cast<Image>(src_im)->get();
  if (!im_src) return false;
  gdImagePtr im_dst = cast<Image>(dst_im)->get();
  if (!im_dst) return false;
  gdImageCopyMerge(im_dst, im_src, dst_x, dst_y,
                   src_x, src_y, src_w, src_h, pct);
  return true;
}