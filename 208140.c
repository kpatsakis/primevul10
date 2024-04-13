static int do_fail (xd3_stream *stream, const char *buf)
{
  int ret;
  ret = system (buf);
  if (! WIFEXITED (ret) || WEXITSTATUS (ret) != 1)
    {
      stream->msg = "command should have not succeeded";
      XPR(NT "command was %s\n", buf);
      return XD3_INTERNAL;
    }
  return 0;
}