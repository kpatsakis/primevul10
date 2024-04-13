static struct gfxinfo *php_handle_jpc(const req::ptr<File>& file) {
  struct gfxinfo *result = nullptr;
  int highest_bit_depth, bit_depth;
  unsigned char first_marker_id;
  unsigned int i;

  /* JPEG 2000 components can be vastly different from one another.
     Each component can be sampled at a different resolution, use
     a different colour space, have a separate colour depth, and
     be compressed totally differently! This makes giving a single
     "bit depth" answer somewhat problematic. For this implementation
     we'll use the highest depth encountered. */

  /* Get the single byte that remains after the file type indentification */
  first_marker_id = file->getc();

  /* Ensure that this marker is SIZ (as is mandated by the standard) */
  if (first_marker_id != JPEG2000_MARKER_SIZ) {
    raise_warning("JPEG2000 codestream corrupt(Expected SIZ marker "
                    "not found after SOC)");
    return nullptr;
  }

  result = (struct gfxinfo *)IM_CALLOC(1, sizeof(struct gfxinfo));
  CHECK_ALLOC_R(result, sizeof (struct gfxinfo), nullptr);

  php_read2(file); /* Lsiz */
  php_read2(file); /* Rsiz */
  result->width = php_read4(file); /* Xsiz */
  result->height = php_read4(file); /* Ysiz */

#if MBO_0
  php_read4(file); /* XOsiz */
  php_read4(file); /* YOsiz */
  php_read4(file); /* XTsiz */
  php_read4(file); /* YTsiz */
  php_read4(file); /* XTOsiz */
  php_read4(file); /* YTOsiz */
#else
  if (!file->seek(24, SEEK_CUR)) {
    IM_FREE(result);
    return nullptr;
  }
#endif

  result->channels = php_read2(file); /* Csiz */
  if (result->channels > 256) {
    IM_FREE(result);
    return nullptr;
  }

  /* Collect bit depth info */
  highest_bit_depth = bit_depth = 0;
  for (i = 0; i < result->channels; i++) {
    bit_depth = file->getc(); /* Ssiz[i] */
    bit_depth++;
    if (bit_depth > highest_bit_depth) {
      highest_bit_depth = bit_depth;
    }

    file->getc(); /* XRsiz[i] */
    file->getc(); /* YRsiz[i] */
  }

  result->bits = highest_bit_depth;

  return result;
}