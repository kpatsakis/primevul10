XML_SetXmlDeclHandler(XML_Parser parser,
                      XML_XmlDeclHandler handler) {
  if (parser != NULL)
    xmlDeclHandler = handler;
}