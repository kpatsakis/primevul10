check_vcdiff_header (xd3_stream *stream,
		     const char *input,
		     const char *line_start,
		     const char *matches,
		     int yes_or_no)
{
  int ret;
  char vcmd[TESTBUFSIZE], gcmd[TESTBUFSIZE];

  snprintf_func (vcmd, TESTBUFSIZE, "%s printhdr -f %s %s",
	    program_name, input, TEST_RECON2_FILE);

  if ((ret = system (vcmd)) != 0)
    {
      XPR(NT "printhdr command: %s\n", vcmd);
      stream->msg = "printhdr cmd failed";
      return XD3_INTERNAL;
    }

  snprintf_func (gcmd, TESTBUFSIZE, "grep \"%s.*%s.*\" %s > /dev/null",
	    line_start, matches, TEST_RECON2_FILE);

  if (yes_or_no)
    {
      if ((ret = do_cmd (stream, gcmd)))
	{
	  XPR(NT "%s\n", gcmd);
	  return ret;
	}
    }
  else
    {
      if ((ret = do_fail (stream, gcmd)))
	{
	  XPR(NT "%s\n", gcmd);
	  return ret;
	}
    }

  return 0;
}