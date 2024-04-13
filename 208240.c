test_secondary_decode (xd3_stream         *stream,
		       const xd3_sec_type *sec,
		       usize_t              input_size,
		       usize_t              compress_size,
		       const uint8_t      *dec_input,
		       const uint8_t      *dec_correct,
		       uint8_t            *dec_output)
{
  int ret;
  xd3_sec_stream *dec_stream;
  const uint8_t *dec_input_used, *dec_input_end;
  uint8_t *dec_output_used, *dec_output_end;

  if ((dec_stream = sec->alloc (stream)) == NULL) { return ENOMEM; }

  if ((ret = sec->init (stream, dec_stream, 0)) != 0) { goto fail; }

  dec_input_used = dec_input;
  dec_input_end  = dec_input + compress_size;

  dec_output_used = dec_output;
  dec_output_end  = dec_output + input_size;

  if ((ret = sec->decode (stream, dec_stream,
			  & dec_input_used, dec_input_end,
			  & dec_output_used, dec_output_end)))
    {
      goto fail;
    }

  if (dec_input_used != dec_input_end)
    {
      stream->msg = "unused input";
      ret = XD3_INTERNAL;
      goto fail;
    }

  if (dec_output_used != dec_output_end)
    {
      stream->msg = "unfinished output";
      ret = XD3_INTERNAL;
      goto fail;
    }

  if (memcmp (dec_output, dec_correct, input_size) != 0)
    {
      stream->msg = "incorrect output";
      ret = XD3_INTERNAL;
      goto fail;
    }

 fail:
  sec->destroy (stream, dec_stream);
  return ret;
}