XML_GetBase(XML_Parser parser)
{
  if (parser == NULL)
    return NULL;
  return curBase;
}