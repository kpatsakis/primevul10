XML_SetAttlistDeclHandler(XML_Parser parser,
                          XML_AttlistDeclHandler attdecl)
{
  if (parser != NULL)
    attlistDeclHandler = attdecl;
}