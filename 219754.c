XML_SetElementDeclHandler(XML_Parser parser,
                          XML_ElementDeclHandler eldecl)
{
  if (parser != NULL)
    elementDeclHandler = eldecl;
}