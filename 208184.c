test_decompress_text (xd3_stream *stream, uint8_t *enc, usize_t enc_size, usize_t test_desize)
{
  xd3_config cfg;
  char decoded[sizeof (test_text)];
  uint8_t *apphead;
  usize_t apphead_size;
  usize_t decoded_size;
  const char *msg;
  int  ret;
  usize_t pos = 0;
  int flags = stream->flags;
  usize_t take;

 input:
  /* Test decoding test_desize input bytes at a time */
  take = min (enc_size - pos, test_desize);
  CHECK(take > 0);

  xd3_avail_input (stream, enc + pos, take);
 again:
  ret = xd3_decode_input (stream);

  pos += take;
  take = 0;

  switch (ret)
    {
    case XD3_OUTPUT:
      break;
    case XD3_WINSTART:
    case XD3_GOTHEADER:
      goto again;
    case XD3_INPUT:
      if (pos < enc_size) { goto input; }
      /* else fallthrough */
    case XD3_WINFINISH:
    default:
      goto fail;
    }

  CHECK(ret == XD3_OUTPUT);
  CHECK(pos == enc_size);

  if (stream->avail_out != sizeof (test_text))
    {
      stream->msg = "incorrect output size";
      ret = XD3_INTERNAL;
      goto fail;
    }

  decoded_size = stream->avail_out;
  memcpy (decoded, stream->next_out, stream->avail_out);

  xd3_consume_output (stream);

  if ((ret = xd3_get_appheader (stream, & apphead, & apphead_size))) { goto fail; }

  if (apphead_size != strlen ((char*) test_apphead) ||
      memcmp (apphead, test_apphead, strlen ((char*) test_apphead)) != 0)
    {
      stream->msg = "incorrect appheader";
      ret = XD3_INTERNAL;
      goto fail;
    }

  if ((ret = xd3_decode_input (stream)) != XD3_WINFINISH ||
      (ret = xd3_close_stream (stream)) != 0)
    {
      goto fail;
    }

  if (decoded_size != sizeof (test_text) ||
      memcmp (decoded, test_text, sizeof (test_text)) != 0)
    {
      stream->msg = "incorrect output text";
      ret = EIO;
    }

 fail:
  msg = stream->msg;
  xd3_free_stream (stream);
  xd3_init_config (& cfg, flags);
  xd3_config_stream (stream, & cfg);
  stream->msg = msg;

  return ret;
}