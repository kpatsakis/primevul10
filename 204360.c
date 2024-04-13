test_multipart_garbage (void)
{
  struct MHD_Connection connection;
  struct MHD_HTTP_Header header;
  struct MHD_PostProcessor *pp;
  unsigned int want_off;
  size_t size = strlen (FORM_DATA);
  size_t splitpoint;
  char xdata[size + 3];

  /* fill in evil garbage at the beginning */
  xdata[0] = '-';
  xdata[1] = 'x';
  xdata[2] = '\r';
  memcpy (&xdata[3], FORM_DATA, size);
  size += 3;
  for (splitpoint = 1; splitpoint < size; splitpoint++)
  {
    want_off = FORM_START;
    memset (&connection, 0, sizeof (struct MHD_Connection));
    memset (&header, 0, sizeof (struct MHD_HTTP_Header));
    connection.headers_received = &header;
    header.header = MHD_HTTP_HEADER_CONTENT_TYPE;
    header.value =
      MHD_HTTP_POST_ENCODING_MULTIPART_FORMDATA ", boundary=AaB03x";
    header.header_size = MHD_STATICSTR_LEN_ (MHD_HTTP_HEADER_CONTENT_TYPE);
    header.value_size = MHD_STATICSTR_LEN_ (
      MHD_HTTP_POST_ENCODING_MULTIPART_FORMDATA ", boundary=AaB03x");
    header.kind = MHD_HEADER_KIND;
    pp = MHD_create_post_processor (&connection,
                                    1024, &value_checker, &want_off);
    MHD_post_process (pp, xdata, splitpoint);
    MHD_post_process (pp, &xdata[splitpoint], size - splitpoint);
    MHD_destroy_post_processor (pp);
    if (want_off != FORM_END)
    {
      fprintf (stderr,
               "Test failed in line %u at point %d\n",
               (unsigned int) __LINE__,
               (int) splitpoint);
      return (int) splitpoint;
    }
  }
  return 0;
}