test_empty_value (void)
{
  struct MHD_Connection connection;
  struct MHD_HTTP_Header header;
  struct MHD_PostProcessor *pp;
  unsigned int want_off = URL_EMPTY_VALUE_START;
  size_t i;
  size_t delta;
  size_t size;

  memset (&connection, 0, sizeof (struct MHD_Connection));
  memset (&header, 0, sizeof (struct MHD_HTTP_Header));
  connection.headers_received = &header;
  header.header = MHD_HTTP_HEADER_CONTENT_TYPE;
  header.value = MHD_HTTP_POST_ENCODING_FORM_URLENCODED;
  header.header_size = strlen (header.header);
  header.value_size = strlen (header.value);
  header.kind = MHD_HEADER_KIND;
  pp = MHD_create_post_processor (&connection,
                                  1024, &value_checker, &want_off);
  i = 0;
  size = strlen (URL_EMPTY_VALUE_DATA);
  while (i < size)
  {
    delta = 1 + MHD_random_ () % (size - i);
    MHD_post_process (pp, &URL_EMPTY_VALUE_DATA[i], delta);
    i += delta;
  }
  MHD_destroy_post_processor (pp);
  if (want_off != URL_EMPTY_VALUE_END)
  {
    fprintf (stderr,
             "Test failed in line %u at offset %d\n",
             (unsigned int) __LINE__,
             (int) want_off);
    return 8;
  }
  return 0;
}