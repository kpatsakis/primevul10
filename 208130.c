main_write_output (xd3_stream* stream, main_file *ofile)
{
  int ret;

  if (option_no_output)
    {
      return 0;
    }

  if (stream->avail_out > 0 &&
      (ret = main_file_write (ofile, stream->next_out,
			      stream->avail_out, "write failed")))
    {
      return ret;
    }

  return 0;
}