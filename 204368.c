MHD_post_process (struct MHD_PostProcessor *pp,
                  const char *post_data,
                  size_t post_data_len)
{
  if (0 == post_data_len)
    return MHD_YES;
  if (NULL == pp)
    return MHD_NO;
  if (MHD_str_equal_caseless_n_ (MHD_HTTP_POST_ENCODING_FORM_URLENCODED,
                                 pp->encoding,
                                 MHD_STATICSTR_LEN_ (
                                   MHD_HTTP_POST_ENCODING_FORM_URLENCODED)))
    return post_process_urlencoded (pp,
                                    post_data,
                                    post_data_len);
  if (MHD_str_equal_caseless_n_ (MHD_HTTP_POST_ENCODING_MULTIPART_FORMDATA,
                                 pp->encoding,
                                 MHD_STATICSTR_LEN_ (
                                   MHD_HTTP_POST_ENCODING_MULTIPART_FORMDATA)))
    return post_process_multipart (pp,
                                   post_data,
                                   post_data_len);
  /* this should never be reached */
  return MHD_NO;
}