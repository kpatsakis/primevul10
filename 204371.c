MHD_create_post_processor (struct MHD_Connection *connection,
                           size_t buffer_size,
                           MHD_PostDataIterator iter,
                           void *iter_cls)
{
  struct MHD_PostProcessor *ret;
  const char *encoding;
  const char *boundary;
  size_t blen;

  if ( (buffer_size < 256) ||
       (NULL == connection) ||
       (NULL == iter))
    mhd_panic (mhd_panic_cls,
               __FILE__,
               __LINE__,
               NULL);
  if (MHD_NO == MHD_lookup_connection_value_n (connection,
                                               MHD_HEADER_KIND,
                                               MHD_HTTP_HEADER_CONTENT_TYPE,
                                               MHD_STATICSTR_LEN_ (
                                                 MHD_HTTP_HEADER_CONTENT_TYPE),
                                               &encoding,
                                               NULL))
    return NULL;
  boundary = NULL;
  if (! MHD_str_equal_caseless_n_ (MHD_HTTP_POST_ENCODING_FORM_URLENCODED,
                                   encoding,
                                   MHD_STATICSTR_LEN_ (
                                     MHD_HTTP_POST_ENCODING_FORM_URLENCODED)))
  {
    if (! MHD_str_equal_caseless_n_ (MHD_HTTP_POST_ENCODING_MULTIPART_FORMDATA,
                                     encoding,
                                     MHD_STATICSTR_LEN_ (
                                       MHD_HTTP_POST_ENCODING_MULTIPART_FORMDATA)))
      return NULL;
    boundary =
      &encoding[MHD_STATICSTR_LEN_ (MHD_HTTP_POST_ENCODING_MULTIPART_FORMDATA)];
    /* Q: should this be "strcasestr"? */
    boundary = strstr (boundary, "boundary=");
    if (NULL == boundary)
      return NULL; /* failed to determine boundary */
    boundary += MHD_STATICSTR_LEN_ ("boundary=");
    blen = strlen (boundary);
    if ( (blen == 0) ||
         (blen * 2 + 2 > buffer_size) )
      return NULL;              /* (will be) out of memory or invalid boundary */
    if ( (boundary[0] == '"') &&
         (boundary[blen - 1] == '"') )
    {
      /* remove enclosing quotes */
      ++boundary;
      blen -= 2;
    }
  }
  else
    blen = 0;
  buffer_size += 4; /* round up to get nice block sizes despite boundary search */

  /* add +1 to ensure we ALWAYS have a zero-termination at the end */
  if (NULL == (ret = MHD_calloc_ (1, sizeof (struct MHD_PostProcessor)
                                  + buffer_size + 1)))
    return NULL;
  ret->connection = connection;
  ret->ikvi = iter;
  ret->cls = iter_cls;
  ret->encoding = encoding;
  ret->buffer_size = buffer_size;
  ret->state = PP_Init;
  ret->blen = blen;
  ret->boundary = boundary;
  ret->skip_rn = RN_Inactive;
  return ret;
}