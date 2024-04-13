XML_GetCurrentLineNumber(XML_Parser parser)
{
  if (parser == NULL)
    return 0;
  if (eventPtr && eventPtr >= positionPtr) {
    XmlUpdatePosition(encoding, positionPtr, eventPtr, &position);
    positionPtr = eventPtr;
  }
  return position.lineNumber + 1;
}