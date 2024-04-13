processInternalEntity(XML_Parser parser, ENTITY *entity,
                      XML_Bool betweenDecl)
{
  const char *textStart, *textEnd;
  const char *next;
  enum XML_Error result;
  OPEN_INTERNAL_ENTITY *openEntity;

  if (freeInternalEntities) {
    openEntity = freeInternalEntities;
    freeInternalEntities = openEntity->next;
  }
  else {
    openEntity = (OPEN_INTERNAL_ENTITY *)MALLOC(sizeof(OPEN_INTERNAL_ENTITY));
    if (!openEntity)
      return XML_ERROR_NO_MEMORY;
  }
  entity->open = XML_TRUE;
  entity->processed = 0;
  openEntity->next = openInternalEntities;
  openInternalEntities = openEntity;
  openEntity->entity = entity;
  openEntity->startTagLevel = tagLevel;
  openEntity->betweenDecl = betweenDecl;
  openEntity->internalEventPtr = NULL;
  openEntity->internalEventEndPtr = NULL;
  textStart = (char *)entity->textPtr;
  textEnd = (char *)(entity->textPtr + entity->textLen);
  /* Set a safe default value in case 'next' does not get set */
  next = textStart;

#ifdef XML_DTD
  if (entity->is_param) {
    int tok = XmlPrologTok(internalEncoding, textStart, textEnd, &next);
    result = doProlog(parser, internalEncoding, textStart, textEnd, tok,
                      next, &next, XML_FALSE);
  }
  else
#endif /* XML_DTD */
    result = doContent(parser, tagLevel, internalEncoding, textStart,
                       textEnd, &next, XML_FALSE);

  if (result == XML_ERROR_NONE) {
    if (textEnd != next && ps_parsing == XML_SUSPENDED) {
      entity->processed = (int)(next - textStart);
      processor = internalEntityProcessor;
    }
    else {
      entity->open = XML_FALSE;
      openInternalEntities = openEntity->next;
      /* put openEntity back in list of free instances */
      openEntity->next = freeInternalEntities;
      freeInternalEntities = openEntity;
    }
  }
  return result;
}