noname_disable_map(Node** plink, GroupNumRemap* map, int* counter)
{
  int r = 0;
  Node* node = *plink;

  switch (NODE_TYPE(node)) {
  case NODE_LIST:
  case NODE_ALT:
    do {
      r = noname_disable_map(&(NODE_CAR(node)), map, counter);
    } while (r == 0 && IS_NOT_NULL(node = NODE_CDR(node)));
    break;

  case NODE_QUANT:
    {
      Node** ptarget = &(NODE_BODY(node));
      Node*  old = *ptarget;
      r = noname_disable_map(ptarget, map, counter);
      if (*ptarget != old && NODE_TYPE(*ptarget) == NODE_QUANT) {
        onig_reduce_nested_quantifier(node, *ptarget);
      }
    }
    break;

  case NODE_ENCLOSURE:
    {
      EnclosureNode* en = ENCLOSURE_(node);
      if (en->type == ENCLOSURE_MEMORY) {
        if (NODE_IS_NAMED_GROUP(node)) {
          (*counter)++;
          map[en->m.regnum].new_val = *counter;
          en->m.regnum = *counter;
          r = noname_disable_map(&(NODE_BODY(node)), map, counter);
        }
        else {
          *plink = NODE_BODY(node);
          NODE_BODY(node) = NULL_NODE;
          onig_node_free(node);
          r = noname_disable_map(plink, map, counter);
        }
      }
      else if (en->type == ENCLOSURE_IF_ELSE) {
        r = noname_disable_map(&(NODE_ENCLOSURE_BODY(en)), map, counter);
        if (r != 0) return r;
        if (IS_NOT_NULL(en->te.Then)) {
          r = noname_disable_map(&(en->te.Then), map, counter);
          if (r != 0) return r;
        }
        if (IS_NOT_NULL(en->te.Else)) {
          r = noname_disable_map(&(en->te.Else), map, counter);
          if (r != 0) return r;
        }
      }
      else
        r = noname_disable_map(&(NODE_BODY(node)), map, counter);
    }
    break;

  case NODE_ANCHOR:
    if (IS_NOT_NULL(NODE_BODY(node)))
      r = noname_disable_map(&(NODE_BODY(node)), map, counter);
    break;

  default:
    break;
  }

  return r;
}