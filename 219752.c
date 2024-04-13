XML_SetEncoding(XML_Parser parser, const XML_Char *encodingName)
{
  if (parser == NULL)
      return XML_STATUS_ERROR;
  /* Block after XML_Parse()/XML_ParseBuffer() has been called.
     XXX There's no way for the caller to determine which of the
     XXX possible error cases caused the XML_STATUS_ERROR return.
  */
  if (ps_parsing == XML_PARSING || ps_parsing == XML_SUSPENDED)
    return XML_STATUS_ERROR;
  if (encodingName == NULL)
    protocolEncodingName = NULL;
  else {
    protocolEncodingName = poolCopyString(&tempPool, encodingName);
    if (!protocolEncodingName)
      return XML_STATUS_ERROR;
  }
  return XML_STATUS_OK;
}