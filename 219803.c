XML_SetEndCdataSectionHandler(XML_Parser parser,
                              XML_EndCdataSectionHandler end) {
  if (parser != NULL)
    endCdataSectionHandler = end;
}