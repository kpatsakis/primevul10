Variant HHVM_FUNCTION(imagepalettecopy,
    const Resource& dst,
    const Resource& src) {
  gdImagePtr dstim = cast<Image>(dst)->get();
  gdImagePtr srcim = cast<Image>(src)->get();
  if (!dstim || !srcim)
    return false;
  gdImagePaletteCopy(dstim, srcim);
  return true;
}