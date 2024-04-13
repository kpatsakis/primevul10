XML_SetStartDoctypeDeclHandler(XML_Parser parser,
                               XML_StartDoctypeDeclHandler start) {
  if (parser != NULL)
    startDoctypeDeclHandler = start;
}