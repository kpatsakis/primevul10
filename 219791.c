XML_ParseBuffer(XML_Parser parser, int len, int isFinal)
{
  const char *start;
  enum XML_Status result = XML_STATUS_OK;

  if (parser == NULL)
    return XML_STATUS_ERROR;
  switch (ps_parsing) {
  case XML_SUSPENDED:
    errorCode = XML_ERROR_SUSPENDED;
    return XML_STATUS_ERROR;
  case XML_FINISHED:
    errorCode = XML_ERROR_FINISHED;
    return XML_STATUS_ERROR;
  case XML_INITIALIZED:
    if (parentParser == NULL && !startParsing(parser)) {
      errorCode = XML_ERROR_NO_MEMORY;
      return XML_STATUS_ERROR;
    }
  default:
    ps_parsing = XML_PARSING;
  }

  start = bufferPtr;
  positionPtr = start;
  bufferEnd += len;
  parseEndPtr = bufferEnd;
  parseEndByteIndex += len;
  ps_finalBuffer = (XML_Bool)isFinal;

  errorCode = processor(parser, start, parseEndPtr, &bufferPtr);

  if (errorCode != XML_ERROR_NONE) {
    eventEndPtr = eventPtr;
    processor = errorProcessor;
    return XML_STATUS_ERROR;
  }
  else {
    switch (ps_parsing) {
    case XML_SUSPENDED:
      result = XML_STATUS_SUSPENDED;
      break;
    case XML_INITIALIZED:
    case XML_PARSING:
      if (isFinal) {
        ps_parsing = XML_FINISHED;
        return result;
      }
    default: ;  /* should not happen */
    }
  }

  XmlUpdatePosition(encoding, positionPtr, bufferPtr, &position);
  positionPtr = bufferPtr;
  return result;
}