XML_GetCurrentByteIndex(XML_Parser parser)
{
  if (parser == NULL)
    return -1;
  if (eventPtr)
    return (XML_Index)(parseEndByteIndex - (parseEndPtr - eventPtr));
  return -1;
}