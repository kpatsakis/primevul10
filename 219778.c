XML_SetUnknownEncodingHandler(XML_Parser parser,
                              XML_UnknownEncodingHandler handler,
                              void *data)
{
  if (parser == NULL)
    return;
  unknownEncodingHandler = handler;
  unknownEncodingHandlerData = data;
}