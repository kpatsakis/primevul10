test_encode_code_table (xd3_stream *stream, int ignore)
{
  int ret;
  const uint8_t *comp_data;
  usize_t comp_size;

  if ((ret = xd3_compute_alternate_table_encoding (stream, & comp_data, & comp_size)))
    {
      return ret;
    }

  stream->acache.s_near = __alternate_code_table_desc.near_modes;
  stream->acache.s_same = __alternate_code_table_desc.same_modes;

  if ((ret = xd3_apply_table_encoding (stream, comp_data, comp_size)))
    {
      return ret;
    }

  if (memcmp (stream->code_table, xd3_alternate_code_table (), sizeof (xd3_dinst) * 256) != 0)
    {
      stream->msg = "wrong code table reconstruction";
      return XD3_INTERNAL;
    }

  return 0;
}