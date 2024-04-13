test_identical_behavior (xd3_stream *stream, int ignore)
{
#define IDB_TGTSZ 10000  /* Not a power of two b/c of hard-coded expectations below. */
#define IDB_BLKSZ 512
#define IDB_WINSZ 1000
#define IDB_DELSZ 1000
#define IDB_WINCNT (IDB_TGTSZ / IDB_WINSZ)

  int ret, i;
  uint8_t buf[IDB_TGTSZ];
  uint8_t del[IDB_DELSZ];
  uint8_t rec[IDB_TGTSZ];
  xd3_source source;
  int nextencwin = 0;
  int winstarts = 0, winfinishes = 0;
  usize_t delpos = 0, recsize;
  xd3_config config;
  memset(&source, 0, sizeof(source));

  for (i = 0; i < IDB_TGTSZ; i += 1)
    {
      buf[i] = (uint8_t) mt_random (&static_mtrand);
    }

  stream->winsize = IDB_WINSZ;

  source.blksize  = IDB_BLKSZ;
  source.name     = "";
  source.curblk   = NULL;
  source.curblkno = 0;

  if ((ret = xd3_set_source (stream, & source))) { goto fail; }

  /* Compute an delta between identical source and targets. */
  for (;;)
    {
      ret = xd3_encode_input (stream);

      if (ret == XD3_INPUT)
	{
	  xd3_avail_input (stream, buf + (IDB_WINSZ * nextencwin), IDB_WINSZ);
	  nextencwin += 1;
	  continue;
	}

      if (ret == XD3_GETSRCBLK)
	{
	  source.curblkno = source.getblkno;
	  source.onblk    = IDB_BLKSZ;
	  source.curblk   = buf + source.getblkno * IDB_BLKSZ;
	  continue;
	}

      if (ret == XD3_WINSTART)
	{
	  winstarts++;
	  continue;
	}
      if (ret == XD3_WINFINISH)
	{
	  winfinishes++;
	  if (winfinishes == IDB_WINCNT)
	    {
	      break;
	    }
	  continue;
	}

      if (ret != XD3_OUTPUT) { goto fail; }

      CHECK(delpos + stream->avail_out <= IDB_DELSZ);

      memcpy (del + delpos, stream->next_out, stream->avail_out);

      delpos += stream->avail_out;

      xd3_consume_output (stream);
    }

  CHECK(winfinishes == IDB_WINCNT);
  CHECK(winstarts == IDB_WINCNT);
  CHECK(nextencwin == IDB_WINCNT);

  /* Reset. */
  memset(&source, 0, sizeof(source));
  source.blksize  = IDB_TGTSZ;
  source.onblk    = IDB_TGTSZ;
  source.curblk   = buf;
  source.curblkno = 0;

  if ((ret = xd3_close_stream (stream))) { goto fail; }
  xd3_free_stream (stream);
  xd3_init_config (& config, 0);
  if ((ret = xd3_config_stream (stream, & config))) { goto fail; }
  if ((ret = xd3_set_source_and_size (stream, & source, IDB_TGTSZ))) { goto fail; }

  /* Decode. */
  if ((ret = xd3_decode_stream (stream, del, delpos, rec, & recsize, IDB_TGTSZ))) { goto fail; }

  /* Check result size and data. */
  if (recsize != IDB_TGTSZ) { stream->msg = "wrong size reconstruction"; goto fail; }
  if (memcmp (rec, buf, IDB_TGTSZ) != 0) { stream->msg = "wrong data reconstruction"; goto fail; }

  /* Check that there was one copy per window. */
  IF_DEBUG (if (stream->n_scpy != IDB_WINCNT ||
		stream->n_add != 0 ||
		stream->n_run != 0) { stream->msg = "wrong copy count"; goto fail; });

  /* Check that no checksums were computed because the initial match
     was presumed. */
  IF_DEBUG (if (stream->large_ckcnt != 0) { stream->msg = "wrong checksum behavior"; goto fail; });

  ret = 0;
 fail:
  return ret;
}