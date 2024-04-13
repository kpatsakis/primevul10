XML_SetExternalEntityRefHandlerArg(XML_Parser parser, void *arg)
{
  if (parser == NULL)
    return;
  if (arg)
    externalEntityRefHandlerArg = (XML_Parser)arg;
  else
    externalEntityRefHandlerArg = parser;
}