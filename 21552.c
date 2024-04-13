XML_ParserCreateNS(const XML_Char *encodingName, XML_Char nsSep) {
  XML_Char tmp[2] = {nsSep, 0};
  return XML_ParserCreate_MM(encodingName, NULL, tmp);
}