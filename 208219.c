main_format_millis (long millis, shortbuf *buf)
{
  if (millis < 1000)
    { 
      short_sprintf (*buf, "%lu ms", millis); 
    }
  else if (millis < 10000) 
    {
      short_sprintf (*buf, "%.1f sec", millis / 1000.0);
    }
  else
    {
      short_sprintf (*buf, "%lu sec", millis / 1000L); 
    }
  return buf->buf;
}