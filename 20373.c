setup_call2(Node* node)
{
  int r = 0;

  switch (NODE_TYPE(node)) {
  case NODE_LIST:
  case NODE_ALT:
    do {
      r = setup_call2(NODE_CAR(node));
    } while (r == 0 && IS_NOT_NULL(node = NODE_CDR(node)));
    break;

  case NODE_QUANT:
    if (QUANT_(node)->upper != 0)
      r = setup_call2(NODE_BODY(node));
    break;

  case NODE_ANCHOR:
    if (ANCHOR_HAS_BODY(ANCHOR_(node)))
      r = setup_call2(NODE_BODY(node));
    break;

  case NODE_ENCLOSURE:
    if (! NODE_IS_IN_ZERO_REPEAT(node))
      r = setup_call2(NODE_BODY(node));

    {
      EnclosureNode* en = ENCLOSURE_(node);

      if (r != 0) return r;
      if (en->type == ENCLOSURE_IF_ELSE) {
        if (IS_NOT_NULL(en->te.Then)) {
          r = setup_call2(en->te.Then);
          if (r != 0) return r;
        }
        if (IS_NOT_NULL(en->te.Else))
          r = setup_call2(en->te.Else);
      }
    }
    break;

  case NODE_CALL:
    if (! NODE_IS_IN_ZERO_REPEAT(node)) {
      setup_call2_call(node);
    }
    break;

  default:
    break;
  }

  return r;
}