int _blosc_getitem(blosc2_context* context, const void* src, int32_t srcsize,
                   int start, int nitems, void* dest) {
  uint8_t* _src = NULL;             /* current pos for source buffer */
  uint8_t flags;                    /* flags for header */
  int32_t ntbytes = 0;              /* the number of uncompressed bytes */
  int32_t nblocks;                   /* number of total blocks in buffer */
  int32_t leftover;                  /* extra bytes at end of buffer */
  int32_t* bstarts;                /* start pointers for each block */
  int32_t typesize, blocksize, nbytes;
  int32_t bsize, bsize2, ebsize, leftoverblock;
  int32_t cbytes;
  int32_t startb, stopb;
  int32_t stop = start + nitems;
  int j;

  if (srcsize < BLOSC_MIN_HEADER_LENGTH) {
    /* Not enough input to parse Blosc1 header */
    return -1;
  }
  _src = (uint8_t*)(src);

  /* Read the header block */
  flags = _src[2];                  /* flags */
  bool memcpyed = flags & (uint8_t)BLOSC_MEMCPYED;
  typesize = (int32_t)_src[3];      /* typesize */
  nbytes = sw32_(_src + 4);         /* buffer size */
  blocksize = sw32_(_src + 8);      /* block size */
  cbytes = sw32_(_src + 12);    /* compressed buffer size */

  ebsize = blocksize + typesize * (int32_t)sizeof(int32_t);

  if ((context->header_flags & BLOSC_DOSHUFFLE) &&
      (context->header_flags & BLOSC_DOBITSHUFFLE)) {
    /* Extended header */
    if (srcsize < BLOSC_EXTENDED_HEADER_LENGTH) {
      /* Not enough input to parse Blosc2 header */
      return -1;
    }
    uint8_t* filters = _src + BLOSC_MIN_HEADER_LENGTH;
    uint8_t* filters_meta = filters + 8;
    for (int i = 0; i < BLOSC2_MAX_FILTERS; i++) {
      context->filters[i] = filters[i];
      context->filters_meta[i] = filters_meta[i];
    }
    bstarts = (int32_t*)(_src + BLOSC_EXTENDED_HEADER_LENGTH);
  } else {
    /* Minimal header */
    flags_to_filters(flags, context->filters);
    bstarts = (int32_t*)(_src + BLOSC_MIN_HEADER_LENGTH);
  }

  // Some checks for malformed buffers
  if (blocksize <= 0 || blocksize > nbytes || typesize <= 0 || typesize > BLOSC_MAX_TYPESIZE) {
    return -1;
  }

  /* Compute some params */
  /* Total blocks */
  nblocks = nbytes / blocksize;
  leftover = nbytes % blocksize;
  nblocks = (leftover > 0) ? nblocks + 1 : nblocks;

  /* Check region boundaries */
  if ((start < 0) || (start * typesize > nbytes)) {
    fprintf(stderr, "`start` out of bounds");
    return -1;
  }

  if ((stop < 0) || (stop * typesize > nbytes)) {
    fprintf(stderr, "`start`+`nitems` out of bounds");
    return -1;
  }

  if (_src + srcsize < (uint8_t *)(bstarts + nblocks)) {
    /* Not enough input to read all `bstarts` */
    return -1;
  }

  for (j = 0; j < nblocks; j++) {
    bsize = blocksize;
    leftoverblock = 0;
    if ((j == nblocks - 1) && (leftover > 0)) {
      bsize = leftover;
      leftoverblock = 1;
    }

    /* Compute start & stop for each block */
    startb = start * (int)typesize - j * (int)blocksize;
    stopb = stop * (int)typesize - j * (int)blocksize;
    if ((startb >= (int)blocksize) || (stopb <= 0)) {
      continue;
    }
    if (startb < 0) {
      startb = 0;
    }
    if (stopb > (int)blocksize) {
      stopb = (int)blocksize;
    }
    bsize2 = stopb - startb;

    /* Do the actual data copy */
    if (memcpyed) {
      // Check that sizes in header are compatible, otherwise there is a header corruption
      if (nbytes + BLOSC_MAX_OVERHEAD != cbytes) {
         return -1;
      }
      if (srcsize < BLOSC_MAX_OVERHEAD + j * blocksize + startb + bsize2) {
        /* Not enough input to copy data */
        return -1;
      }
      memcpy((uint8_t*)dest + ntbytes,
             (uint8_t*)src + BLOSC_MAX_OVERHEAD + j * blocksize + startb,
             (unsigned int)bsize2);
      cbytes = (int)bsize2;
    }
    else {
      struct thread_context* scontext = context->serial_context;

      /* Resize the temporaries in serial context if needed */
      if (blocksize != scontext->tmp_blocksize) {
        my_free(scontext->tmp);
        scontext->tmp_nbytes = (size_t)3 * context->blocksize + ebsize;
        scontext->tmp = my_malloc(scontext->tmp_nbytes);
        scontext->tmp2 = scontext->tmp + blocksize;
        scontext->tmp3 = scontext->tmp + blocksize + ebsize;
        scontext->tmp4 = scontext->tmp + 2 * blocksize + ebsize;
        scontext->tmp_blocksize = (int32_t)blocksize;
      }

      // Regular decompression.  Put results in tmp2.
      // If the block is aligned and the worst case fits in destination, let's avoid a copy
      bool get_single_block = ((startb == 0) && (bsize == nitems * typesize));
      uint8_t* tmp2 = get_single_block ? dest : scontext->tmp2;
      cbytes = blosc_d(context->serial_context, bsize, leftoverblock,
                       src, srcsize, sw32_(bstarts + j),
                       tmp2, 0, scontext->tmp, scontext->tmp3);
      if (cbytes < 0) {
        ntbytes = cbytes;
        break;
      }
      if (!get_single_block) {
        /* Copy to destination */
        memcpy((uint8_t *) dest + ntbytes, tmp2 + startb, (unsigned int) bsize2);
      }
      cbytes = (int)bsize2;
    }
    ntbytes += cbytes;
  }

  return ntbytes;
}