XML_SetElementHandler(XML_Parser parser,
                      XML_StartElementHandler start,
                      XML_EndElementHandler end)
{
  if (parser == NULL)
    return;
  startElementHandler = start;
  endElementHandler = end;
}