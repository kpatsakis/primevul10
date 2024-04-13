main_recode_copy (xd3_stream* stream,
		  xd3_output* output,
		  xd3_desect* input)
{
  int ret;

  XD3_ASSERT(output != NULL);
  XD3_ASSERT(output->next_page == NULL);

  if ((ret = xd3_decode_allocate (recode_stream,
				  input->size,
				  &output->base,
				  &output->avail)))
    {
      XPR(NT XD3_LIB_ERRMSG (stream, ret));
      return ret;
    }

  memcpy (output->base,
	  /* Note: decoder advances buf, so get base of buffer with
	   * buf_max - size */
	  input->buf_max - input->size,
	  input->size);
  output->next = input->size;
  return 0;
}