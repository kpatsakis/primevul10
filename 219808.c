XML_SetNamespaceDeclHandler(XML_Parser parser,
                            XML_StartNamespaceDeclHandler start,
                            XML_EndNamespaceDeclHandler end)
{
  if (parser == NULL)
    return;
  startNamespaceDeclHandler = start;
  endNamespaceDeclHandler = end;
}