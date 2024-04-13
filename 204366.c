find_boundary (struct MHD_PostProcessor *pp,
               const char *boundary,
               size_t blen,
               size_t *ioffptr,
               enum PP_State next_state,
               enum PP_State next_dash_state)
{
  char *buf = (char *) &pp[1];
  const char *dash;

  if (pp->buffer_pos < 2 + blen)
  {
    if (pp->buffer_pos == pp->buffer_size)
      pp->state = PP_Error;     /* out of memory */
    /* ++(*ioffptr); */
    return MHD_NO;              /* not enough data */
  }
  if ( (0 != memcmp ("--",
                     buf,
                     2)) ||
       (0 != memcmp (&buf[2],
                     boundary,
                     blen)))
  {
    if (pp->state != PP_Init)
    {
      /* garbage not allowed */
      pp->state = PP_Error;
    }
    else
    {
      /* skip over garbage (RFC 2046, 5.1.1) */
      dash = memchr (buf,
                     '-',
                     pp->buffer_pos);
      if (NULL == dash)
        (*ioffptr) += pp->buffer_pos;     /* skip entire buffer */
      else if (dash == buf)
        (*ioffptr)++;       /* at least skip one byte */
      else
        (*ioffptr) += dash - buf;       /* skip to first possible boundary */
    }
    return MHD_NO;              /* expected boundary */
  }
  /* remove boundary from buffer */
  (*ioffptr) += 2 + blen;
  /* next: start with headers */
  pp->skip_rn = RN_Dash;
  pp->state = next_state;
  pp->dash_state = next_dash_state;
  return MHD_YES;
}