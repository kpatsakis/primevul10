XML_SetEndNamespaceDeclHandler(XML_Parser parser,
                               XML_EndNamespaceDeclHandler end) {
  if (parser != NULL)
    endNamespaceDeclHandler = end;
}