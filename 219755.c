XML_SetNotStandaloneHandler(XML_Parser parser,
                            XML_NotStandaloneHandler handler)
{
  if (parser != NULL)
    notStandaloneHandler = handler;
}