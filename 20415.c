check_type_tree(Node* node, int type_mask, int enclosure_mask, int anchor_mask)
{
  NodeType type;
  int r = 0;

  type = NODE_TYPE(node);
  if ((NODE_TYPE2BIT(type) & type_mask) == 0)
    return 1;

  switch (type) {
  case NODE_LIST:
  case NODE_ALT:
    do {
      r = check_type_tree(NODE_CAR(node), type_mask, enclosure_mask,
                          anchor_mask);
    } while (r == 0 && IS_NOT_NULL(node = NODE_CDR(node)));
    break;

  case NODE_QUANT:
    r = check_type_tree(NODE_BODY(node), type_mask, enclosure_mask, anchor_mask);
    break;

  case NODE_ENCLOSURE:
    {
      EnclosureNode* en = ENCLOSURE_(node);
      if (((1<<en->type) & enclosure_mask) == 0)
        return 1;

      r = check_type_tree(NODE_BODY(node), type_mask, enclosure_mask, anchor_mask);
      if (r == 0 && en->type == ENCLOSURE_IF_ELSE) {
        if (IS_NOT_NULL(en->te.Then)) {
          r = check_type_tree(en->te.Then, type_mask, enclosure_mask, anchor_mask);
          if (r != 0) break;
        }
        if (IS_NOT_NULL(en->te.Else)) {
          r = check_type_tree(en->te.Else, type_mask, enclosure_mask, anchor_mask);
        }
      }
    }
    break;

  case NODE_ANCHOR:
    type = ANCHOR_(node)->type;
    if ((type & anchor_mask) == 0)
      return 1;

    if (IS_NOT_NULL(NODE_BODY(node)))
      r = check_type_tree(NODE_BODY(node), type_mask, enclosure_mask, anchor_mask);
    break;

  case NODE_GIMMICK:
  default:
    break;
  }
  return r;
}