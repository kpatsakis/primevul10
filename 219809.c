XML_SetEndDoctypeDeclHandler(XML_Parser parser,
                             XML_EndDoctypeDeclHandler end) {
  if (parser != NULL)
    endDoctypeDeclHandler = end;
}