process_multipart_headers (struct MHD_PostProcessor *pp,
                           size_t *ioffptr,
                           enum PP_State next_state)
{
  char *buf = (char *) &pp[1];
  size_t newline;

  newline = 0;
  while ( (newline < pp->buffer_pos) &&
          (buf[newline] != '\r') &&
          (buf[newline] != '\n') )
    newline++;
  if (newline == pp->buffer_size)
  {
    pp->state = PP_Error;
    return MHD_NO;              /* out of memory */
  }
  if (newline == pp->buffer_pos)
    return MHD_NO;              /* will need more data */
  if (0 == newline)
  {
    /* empty line - end of headers */
    pp->skip_rn = RN_Full;
    pp->state = next_state;
    return MHD_YES;
  }
  /* got an actual header */
  if (buf[newline] == '\r')
    pp->skip_rn = RN_OptN;
  buf[newline] = '\0';
  if (MHD_str_equal_caseless_n_ ("Content-disposition: ",
                                 buf,
                                 MHD_STATICSTR_LEN_ ("Content-disposition: ")))
  {
    try_get_value (&buf[MHD_STATICSTR_LEN_ ("Content-disposition: ")],
                   "name",
                   &pp->content_name);
    try_get_value (&buf[MHD_STATICSTR_LEN_ ("Content-disposition: ")],
                   "filename",
                   &pp->content_filename);
  }
  else
  {
    try_match_header ("Content-type: ",
                      MHD_STATICSTR_LEN_ ("Content-type: "),
                      buf,
                      &pp->content_type);
    try_match_header ("Content-Transfer-Encoding: ",
                      MHD_STATICSTR_LEN_ ("Content-Transfer-Encoding: "),
                      buf,
                      &pp->content_transfer_encoding);
  }
  (*ioffptr) += newline + 1;
  return MHD_YES;
}