XML_GetAttributeInfo(XML_Parser parser)
{
  if (parser == NULL)
    return NULL;
  return attInfo;
}