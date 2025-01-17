static int blosc_d(
    struct thread_context* thread_context, int32_t bsize,
    int32_t leftoverblock, const uint8_t* src, int32_t srcsize, int32_t src_offset,
    uint8_t* dest, int32_t dest_offset, uint8_t* tmp, uint8_t* tmp2) {
  blosc2_context* context = thread_context->parent_context;
  uint8_t* filters = context->filters;
  uint8_t *tmp3 = thread_context->tmp4;
  int32_t compformat = (context->header_flags & 0xe0) >> 5;
  int dont_split = (context->header_flags & 0x10) >> 4;
  //uint8_t blosc_version_format = src[0];
  int nstreams;
  int32_t neblock;
  int32_t nbytes;                /* number of decompressed bytes in split */
  int32_t cbytes;                /* number of compressed bytes in split */
  int32_t ctbytes = 0;           /* number of compressed bytes in block */
  int32_t ntbytes = 0;           /* number of uncompressed bytes in block */
  uint8_t* _dest;
  int32_t typesize = context->typesize;
  int32_t nblock = dest_offset / context->blocksize;
  const char* compname;

  if (context->block_maskout != NULL && context->block_maskout[nblock]) {
    // Do not decompress, but act as if we successfully decompressed everything
    return bsize;
  }

  if (src_offset <= 0 || src_offset >= srcsize) {
    /* Invalid block src offset encountered */
    return -1;
  }

  src += src_offset;
  srcsize -= src_offset;

  int last_filter_index = last_filter(filters, 'd');

  if ((last_filter_index >= 0) &&
          (next_filter(filters, BLOSC2_MAX_FILTERS, 'd') != BLOSC_DELTA)) {
   // We are making use of some filter, so use a temp for destination
   _dest = tmp;
  } else {
    // If no filters, or only DELTA in pipeline
   _dest = dest + dest_offset;
  }

  /* The number of compressed data streams for this block */
  if (!dont_split && !leftoverblock && !context->use_dict) {
    // We don't want to split when in a training dict state
    nstreams = (int32_t)typesize;
  }
  else {
    nstreams = 1;
  }

  neblock = bsize / nstreams;
  for (int j = 0; j < nstreams; j++) {
    if (srcsize < sizeof(int32_t)) {
      /* Not enough input to read compressed size */
      return -1;
    }
    srcsize -= sizeof(int32_t);
    cbytes = sw32_(src);      /* amount of compressed bytes */
    if (cbytes > 0) {
      if (srcsize < cbytes) {
        /* Not enough input to read compressed bytes */
        return -1;
      }
      srcsize -= cbytes;
    }
    src += sizeof(int32_t);
    ctbytes += (int32_t)sizeof(int32_t);

    /* Uncompress */
    if (cbytes <= 0) {
      // A run
      if (cbytes < -255) {
        // Runs can only encode a byte
        return -2;
      }
      uint8_t value = -cbytes;
      memset(_dest, value, (unsigned int)neblock);
      nbytes = neblock;
      cbytes = 0;  // everything is encoded in the cbytes token
    }
    else if (cbytes == neblock) {
      memcpy(_dest, src, (unsigned int)neblock);
      nbytes = (int32_t)neblock;
    }
    else {
      if (compformat == BLOSC_BLOSCLZ_FORMAT) {
        nbytes = blosclz_decompress(src, cbytes, _dest, (int)neblock);
      }
  #if defined(HAVE_LZ4)
      else if (compformat == BLOSC_LZ4_FORMAT) {
        nbytes = lz4_wrap_decompress((char*)src, (size_t)cbytes,
                                     (char*)_dest, (size_t)neblock);
      }
  #endif /*  HAVE_LZ4 */
  #if defined(HAVE_LIZARD)
      else if (compformat == BLOSC_LIZARD_FORMAT) {
        nbytes = lizard_wrap_decompress((char*)src, (size_t)cbytes,
                                        (char*)_dest, (size_t)neblock);
      }
  #endif /*  HAVE_LIZARD */
  #if defined(HAVE_SNAPPY)
      else if (compformat == BLOSC_SNAPPY_FORMAT) {
        nbytes = snappy_wrap_decompress((char*)src, (size_t)cbytes,
                                        (char*)_dest, (size_t)neblock);
      }
  #endif /*  HAVE_SNAPPY */
  #if defined(HAVE_ZLIB)
      else if (compformat == BLOSC_ZLIB_FORMAT) {
        nbytes = zlib_wrap_decompress((char*)src, (size_t)cbytes,
                                      (char*)_dest, (size_t)neblock);
      }
  #endif /*  HAVE_ZLIB */
  #if defined(HAVE_ZSTD)
      else if (compformat == BLOSC_ZSTD_FORMAT) {
        nbytes = zstd_wrap_decompress(thread_context,
                                      (char*)src, (size_t)cbytes,
                                      (char*)_dest, (size_t)neblock);
      }
  #endif /*  HAVE_ZSTD */
      else {
        compname = clibcode_to_clibname(compformat);
        fprintf(stderr,
                "Blosc has not been compiled with decompression "
                    "support for '%s' format. ", compname);
        fprintf(stderr, "Please recompile for adding this support.\n");
        return -5;    /* signals no decompression support */
      }

      /* Check that decompressed bytes number is correct */
      if (nbytes != neblock) {
        return -2;
      }

    }
    src += cbytes;
    ctbytes += cbytes;
    _dest += nbytes;
    ntbytes += nbytes;
  } /* Closes j < nstreams */

  if (last_filter_index >= 0) {
    int errcode = pipeline_d(context, bsize, dest, dest_offset, tmp, tmp2, tmp3,
                             last_filter_index);
    if (errcode < 0)
      return errcode;
  }

  /* Return the number of uncompressed bytes */
  return (int)ntbytes;
}