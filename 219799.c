XML_SetExternalEntityRefHandler(XML_Parser parser,
                                XML_ExternalEntityRefHandler handler)
{
  if (parser != NULL)
    externalEntityRefHandler = handler;
}