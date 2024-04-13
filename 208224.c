main_format_bcnt (xoff_t r, shortbuf *buf)
{
  static const char* fmts[] = { "B", "KiB", "MiB", "GiB", "TiB", "PiB", "EiB" };
  usize_t i;

  for (i = 0; i < SIZEOF_ARRAY(fmts) - 1; i += 1)
    {
      xoff_t new_r;

      if (r == 0)
	{
	  short_sprintf (*buf, "0 %s", fmts[i]);
	  return buf->buf;
	}

      if (r >= 1 && r < 10)
	{
	  short_sprintf (*buf, "%.2f %s", (double) r, fmts[i]);
	  return buf->buf;
	}

      if (r >= 10 && r < 100)
	{
	  short_sprintf (*buf, "%.1f %s", (double) r, fmts[i]);
	  return buf->buf;
	}

      if (r >= 100 && r < 1000)
	{
	  short_sprintf (*buf, "%"Q"u %s", r, fmts[i]);
	  return buf->buf;
	}

      new_r = r / 1024;

      if (new_r < 10)
	{
	  short_sprintf (*buf, "%.2f %s", (double) r / 1024.0, fmts[i + 1]);
	  return buf->buf;
	}

      if (new_r < 100)
	{
	  short_sprintf (*buf, "%.1f %s", (double) r / 1024.0, fmts[i + 1]);
	  return buf->buf;
	}

      r = new_r;
    }
  XD3_ASSERT (0);
  return "";
}