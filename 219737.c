XML_ParserReset(XML_Parser parser, const XML_Char *encodingName)
{
  TAG *tStk;
  OPEN_INTERNAL_ENTITY *openEntityList;

  if (parser == NULL)
      return XML_FALSE;

  if (parentParser)
    return XML_FALSE;
  /* move tagStack to freeTagList */
  tStk = tagStack;
  while (tStk) {
    TAG *tag = tStk;
    tStk = tStk->parent;
    tag->parent = freeTagList;
    moveToFreeBindingList(parser, tag->bindings);
    tag->bindings = NULL;
    freeTagList = tag;
  }
  /* move openInternalEntities to freeInternalEntities */
  openEntityList = openInternalEntities;
  while (openEntityList) {
    OPEN_INTERNAL_ENTITY *openEntity = openEntityList;
    openEntityList = openEntity->next;
    openEntity->next = freeInternalEntities;
    freeInternalEntities = openEntity;
  }
  moveToFreeBindingList(parser, inheritedBindings);
  FREE(unknownEncodingMem);
  if (unknownEncodingRelease)
    unknownEncodingRelease(unknownEncodingData);
  poolClear(&tempPool);
  poolClear(&temp2Pool);
  parserInit(parser, encodingName);
  dtdReset(_dtd, &parser->m_mem);
  return XML_TRUE;
}