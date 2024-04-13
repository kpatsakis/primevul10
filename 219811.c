XML_GetCurrentByteCount(XML_Parser parser)
{
  if (parser == NULL)
    return 0;
  if (eventEndPtr && eventPtr)
    return (int)(eventEndPtr - eventPtr);
  return 0;
}