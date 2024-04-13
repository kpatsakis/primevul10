test_overflow ()
{
  struct MHD_Connection connection;
  struct MHD_HTTP_Header header;
  struct MHD_PostProcessor *pp;
  size_t i;
  size_t j;
  size_t delta;
  char *buf;

  memset (&connection, 0, sizeof (struct MHD_Connection));
  memset (&header, 0, sizeof (struct MHD_HTTP_Header));
  connection.headers_received = &header;
  header.header = MHD_HTTP_HEADER_CONTENT_TYPE;
  header.value = MHD_HTTP_POST_ENCODING_FORM_URLENCODED;
  header.header_size = strlen (header.header);
  header.value_size = strlen (header.value);
  header.kind = MHD_HEADER_KIND;
  for (i = 128; i < 1024 * 1024; i += 1024)
  {
    pp = MHD_create_post_processor (&connection,
                                    1024,
                                    &value_checker2,
                                    NULL);
    buf = malloc (i);
    if (NULL == buf)
      return 1;
    memset (buf, 'A', i);
    buf[i / 2] = '=';
    delta = 1 + (MHD_random_ () % (i - 1));
    j = 0;
    while (j < i)
    {
      if (j + delta > i)
        delta = i - j;
      if (MHD_NO ==
          MHD_post_process (pp,
                            &buf[j],
                            delta))
        break;
      j += delta;
    }
    free (buf);
    MHD_destroy_post_processor (pp);
  }
  return 0;
}