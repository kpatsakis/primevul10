freeBindings(XML_Parser parser, BINDING *bindings)
{
  while (bindings) {
    BINDING *b = bindings;

    /* startNamespaceDeclHandler will have been called for this
     * binding in addBindings(), so call the end handler now.
     */
    if (endNamespaceDeclHandler)
        endNamespaceDeclHandler(handlerArg, b->prefix->name);

    bindings = bindings->nextTagBinding;
    b->nextTagBinding = freeBindingList;
    freeBindingList = b;
    b->prefix->binding = b->prevPrefixBinding;
  }
}