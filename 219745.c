XML_DefaultCurrent(XML_Parser parser)
{
  if (parser == NULL)
    return;
  if (defaultHandler) {
    if (openInternalEntities)
      reportDefault(parser,
                    internalEncoding,
                    openInternalEntities->internalEventPtr,
                    openInternalEntities->internalEventEndPtr);
    else
      reportDefault(parser, encoding, eventPtr, eventEndPtr);
  }
}