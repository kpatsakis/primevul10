XML_SetDoctypeDeclHandler(XML_Parser parser,
                          XML_StartDoctypeDeclHandler start,
                          XML_EndDoctypeDeclHandler end)
{
  if (parser == NULL)
    return;
  startDoctypeDeclHandler = start;
  endDoctypeDeclHandler = end;
}