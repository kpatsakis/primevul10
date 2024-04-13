XML_SetUnparsedEntityDeclHandler(XML_Parser parser,
                                 XML_UnparsedEntityDeclHandler handler)
{
  if (parser != NULL)
    unparsedEntityDeclHandler = handler;
}