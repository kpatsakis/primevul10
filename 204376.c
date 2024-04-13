process_value (struct MHD_PostProcessor *pp,
               const char *value_start,
               const char *value_end,
               const char *last_escape)
{
  char xbuf[XBUF_SIZE + 1];
  size_t xoff;

  mhd_assert (pp->xbuf_pos < sizeof (xbuf));
  memcpy (xbuf,
          pp->xbuf,
          pp->xbuf_pos);
  xoff = pp->xbuf_pos;
  pp->xbuf_pos = 0;
  if (NULL != last_escape)
  {
    if (((size_t) (value_end - last_escape)) < sizeof (pp->xbuf))
    {
      pp->xbuf_pos = value_end - last_escape;
      memcpy (pp->xbuf,
              last_escape,
              value_end - last_escape);
      value_end = last_escape;
    }
  }
  while ( (value_start != value_end) ||
          (pp->must_ikvi) ||
          (xoff > 0) )
  {
    size_t delta = value_end - value_start;

    if (delta > XBUF_SIZE - xoff)
      delta = XBUF_SIZE - xoff;
    /* move input into processing buffer */
    memcpy (&xbuf[xoff],
            value_start,
            delta);
    /* find if escape sequence is at the end of the processing buffer;
       if so, exclude those from processing (reduce delta to point at
       end of processed region) */
    if (delta >= XBUF_SIZE - 2)
    {
      if ((xoff + delta > 0) &&
          ('%' == xbuf[xoff + delta - 1]))
        delta--;
      else if ((xoff + delta > 1) &&
               ('%' == xbuf[xoff + delta - 2]))
        delta -= 2;
    }
    xoff += delta;
    value_start += delta;
    mhd_assert (xoff < sizeof (xbuf));
    /* unescape */
    xbuf[xoff] = '\0';        /* 0-terminate in preparation */
    MHD_unescape_plus (xbuf);
    xoff = MHD_http_unescape (xbuf);
    /* finally: call application! */
    pp->must_ikvi = false;
    if (MHD_NO == pp->ikvi (pp->cls,
                            MHD_POSTDATA_KIND,
                            (const char *) &pp[1],        /* key */
                            NULL,
                            NULL,
                            NULL,
                            xbuf,
                            pp->value_offset,
                            xoff))
    {
      pp->state = PP_Error;
      return;
    }
    pp->value_offset += xoff;
    xoff = 0;
  }
}