XML_SetBase(XML_Parser parser, const XML_Char *p)
{
  if (parser == NULL)
    return XML_STATUS_ERROR;
  if (p) {
    p = poolCopyString(&_dtd->pool, p);
    if (!p)
      return XML_STATUS_ERROR;
    curBase = p;
  }
  else
    curBase = NULL;
  return XML_STATUS_OK;
}