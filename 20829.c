convert_from_network(char *buffer, size_t length)
{
  CURLcode rc;

  /* translate from the network encoding to the host encoding */
  char *input_ptr, *output_ptr;
  size_t in_bytes, out_bytes;

  /* open an iconv conversion descriptor if necessary */
  if(inbound_cd == (iconv_t)-1) {
    inbound_cd = iconv_open(CURL_ICONV_CODESET_OF_HOST,
                            CURL_ICONV_CODESET_OF_NETWORK);
    if(inbound_cd == (iconv_t)-1) {
      return CURLE_CONV_FAILED;
    }
  }
  /* call iconv */
  input_ptr = output_ptr = buffer;
  in_bytes = out_bytes = length;
  rc = iconv(inbound_cd, &input_ptr,  &in_bytes,
                         &output_ptr, &out_bytes);
  if ((rc == -1) || (in_bytes != 0)) {
    return CURLE_CONV_FAILED;
  }

  return CURLE_OK;
}