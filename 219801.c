XML_SetStartNamespaceDeclHandler(XML_Parser parser,
                                 XML_StartNamespaceDeclHandler start) {
  if (parser != NULL)
    startNamespaceDeclHandler = start;
}