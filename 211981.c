bool HHVM_FUNCTION(imagecopyresampled,
    const Resource& dst_im, const Resource& src_im,
    int64_t dst_x, int64_t dst_y, int64_t src_x, int64_t src_y,
    int64_t dst_w, int64_t dst_h, int64_t src_w, int64_t src_h) {
  gdImagePtr im_src = get_valid_image_resource(src_im);
  if (!im_src) return false;
  gdImagePtr im_dst = get_valid_image_resource(dst_im);
  if (!im_dst) return false;
  gdImageCopyResampled(im_dst, im_src, dst_x, dst_y, src_x, src_y,
                       dst_w, dst_h, src_w, src_h);
  return true;
}