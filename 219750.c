XML_ResumeParser(XML_Parser parser)
{
  enum XML_Status result = XML_STATUS_OK;

  if (parser == NULL)
    return XML_STATUS_ERROR;
  if (ps_parsing != XML_SUSPENDED) {
    errorCode = XML_ERROR_NOT_SUSPENDED;
    return XML_STATUS_ERROR;
  }
  ps_parsing = XML_PARSING;

  errorCode = processor(parser, bufferPtr, parseEndPtr, &bufferPtr);

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
      if (ps_finalBuffer) {
        ps_parsing = XML_FINISHED;
        return result;
      }
    default: ;
    }
  }

  XmlUpdatePosition(encoding, positionPtr, bufferPtr, &position);
  positionPtr = bufferPtr;
  return result;
}