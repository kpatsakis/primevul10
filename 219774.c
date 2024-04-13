XML_StopParser(XML_Parser parser, XML_Bool resumable)
{
  if (parser == NULL)
    return XML_STATUS_ERROR;
  switch (ps_parsing) {
  case XML_SUSPENDED:
    if (resumable) {
      errorCode = XML_ERROR_SUSPENDED;
      return XML_STATUS_ERROR;
    }
    ps_parsing = XML_FINISHED;
    break;
  case XML_FINISHED:
    errorCode = XML_ERROR_FINISHED;
    return XML_STATUS_ERROR;
  default:
    if (resumable) {
#ifdef XML_DTD
      if (isParamEntity) {
        errorCode = XML_ERROR_SUSPEND_PE;
        return XML_STATUS_ERROR;
      }
#endif
      ps_parsing = XML_SUSPENDED;
    }
    else
      ps_parsing = XML_FINISHED;
  }
  return XML_STATUS_OK;
}