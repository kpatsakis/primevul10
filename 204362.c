test_urlencoding_case (unsigned int want_start,
                       unsigned int want_end,
                       const char *url_data)
{
  struct MHD_Connection connection;
  struct MHD_HTTP_Header header;
  struct MHD_PostProcessor *pp;
  unsigned int want_off = want_start;
  size_t i;
  size_t delta;
  size_t size;

  memset (&connection, 0, sizeof (struct MHD_Connection));
  memset (&header, 0, sizeof (struct MHD_HTTP_Header));
  connection.headers_received = &header;
  header.header = MHD_HTTP_HEADER_CONTENT_TYPE;
  header.value = MHD_HTTP_POST_ENCODING_FORM_URLENCODED;
  header.header_size = MHD_STATICSTR_LEN_ (MHD_HTTP_HEADER_CONTENT_TYPE);
  header.value_size = MHD_STATICSTR_LEN_ (
    MHD_HTTP_POST_ENCODING_FORM_URLENCODED);
  header.kind = MHD_HEADER_KIND;
  pp = MHD_create_post_processor (&connection,
                                  1024,
                                  &value_checker,
                                  &want_off);
  i = 0;
  size = strlen (url_data);
  while (i < size)
  {
    delta = 1 + MHD_random_ () % (size - i);
    MHD_post_process (pp,
                      &url_data[i],
                      delta);
    i += delta;
  }
  MHD_destroy_post_processor (pp);
  if (want_off != want_end)
  {
    fprintf (stderr,
             "Test failed in line %u: %u != %u\n",
             (unsigned int) __LINE__,
             want_off,
             want_end);
    return 1;
  }
  return 0;
}