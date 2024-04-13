test_multipart (void)
{
  struct MHD_Connection connection;
  struct MHD_HTTP_Header header;
  struct MHD_PostProcessor *pp;
  unsigned int want_off = FORM_START;
  size_t i;
  size_t delta;
  size_t size;

  memset (&connection, 0, sizeof (struct MHD_Connection));
  memset (&header, 0, sizeof (struct MHD_HTTP_Header));
  connection.headers_received = &header;
  header.header = MHD_HTTP_HEADER_CONTENT_TYPE;
  header.value =
    MHD_HTTP_POST_ENCODING_MULTIPART_FORMDATA ", boundary=AaB03x";
  header.kind = MHD_HEADER_KIND;
  header.header_size = strlen (header.header);
  header.value_size = strlen (header.value);
  pp = MHD_create_post_processor (&connection,
                                  1024, &value_checker, &want_off);
  i = 0;
  size = strlen (FORM_DATA);
  while (i < size)
  {
    delta = 1 + MHD_random_ () % (size - i);
    MHD_post_process (pp, &FORM_DATA[i], delta);
    i += delta;
  }
  MHD_destroy_post_processor (pp);
  if (want_off != FORM_END)
  {
    fprintf (stderr,
             "Test failed in line %u\n",
             (unsigned int) __LINE__);
    return 2;
  }
  return 0;
}