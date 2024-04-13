compile_length_anchor_node(AnchorNode* node, regex_t* reg)
{
  int len;
  int tlen = 0;

  if (IS_NOT_NULL(NODE_ANCHOR_BODY(node))) {
    tlen = compile_length_tree(NODE_ANCHOR_BODY(node), reg);
    if (tlen < 0) return tlen;
  }

  switch (node->type) {
  case ANCHOR_PREC_READ:
    len = SIZE_OP_PREC_READ_START + tlen + SIZE_OP_PREC_READ_END;
    break;
  case ANCHOR_PREC_READ_NOT:
    len = SIZE_OP_PREC_READ_NOT_START + tlen + SIZE_OP_PREC_READ_NOT_END;
    break;
  case ANCHOR_LOOK_BEHIND:
    len = SIZE_OP_LOOK_BEHIND + tlen;
    break;
  case ANCHOR_LOOK_BEHIND_NOT:
    len = SIZE_OP_LOOK_BEHIND_NOT_START + tlen + SIZE_OP_LOOK_BEHIND_NOT_END;
    break;

  case ANCHOR_WORD_BOUNDARY:
  case ANCHOR_NO_WORD_BOUNDARY:
#ifdef USE_WORD_BEGIN_END
  case ANCHOR_WORD_BEGIN:
  case ANCHOR_WORD_END:
#endif
    len = SIZE_OP_WORD_BOUNDARY;
    break;

  case ANCHOR_EXTENDED_GRAPHEME_CLUSTER_BOUNDARY:
  case ANCHOR_NO_EXTENDED_GRAPHEME_CLUSTER_BOUNDARY:
    len = SIZE_OPCODE;
    break;

  default:
    len = SIZE_OPCODE;
    break;
  }

  return len;
}