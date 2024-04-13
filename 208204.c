main_merge_func (xd3_stream* stream, main_file *no_write)
{
  int ret;

  if ((ret = xd3_whole_append_window (stream)))
    {
      return ret;
    }

  return 0;
}