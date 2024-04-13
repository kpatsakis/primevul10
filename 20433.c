compile_anchor_node(AnchorNode* node, regex_t* reg, ScanEnv* env)
{
  int r, len;
  enum OpCode op;

  switch (node->type) {
  case ANCHOR_BEGIN_BUF:      r = add_opcode(reg, OP_BEGIN_BUF);      break;
  case ANCHOR_END_BUF:        r = add_opcode(reg, OP_END_BUF);        break;
  case ANCHOR_BEGIN_LINE:     r = add_opcode(reg, OP_BEGIN_LINE);     break;
  case ANCHOR_END_LINE:       r = add_opcode(reg, OP_END_LINE);       break;
  case ANCHOR_SEMI_END_BUF:   r = add_opcode(reg, OP_SEMI_END_BUF);   break;
  case ANCHOR_BEGIN_POSITION: r = add_opcode(reg, OP_BEGIN_POSITION); break;

  case ANCHOR_WORD_BOUNDARY:
    op = OP_WORD_BOUNDARY;
  word:
    r = add_opcode(reg, op);
    if (r != 0) return r;
    r = add_mode(reg, (ModeType )node->ascii_mode);
    break;

  case ANCHOR_NO_WORD_BOUNDARY:
    op = OP_NO_WORD_BOUNDARY; goto word;
    break;
#ifdef USE_WORD_BEGIN_END
  case ANCHOR_WORD_BEGIN:
    op = OP_WORD_BEGIN; goto word;
    break;
  case ANCHOR_WORD_END:
    op = OP_WORD_END; goto word;
    break;
#endif

  case ANCHOR_EXTENDED_GRAPHEME_CLUSTER_BOUNDARY:
    r = add_opcode(reg, OP_EXTENDED_GRAPHEME_CLUSTER_BOUNDARY);
    break;

  case ANCHOR_NO_EXTENDED_GRAPHEME_CLUSTER_BOUNDARY:
    r = add_opcode(reg, OP_NO_EXTENDED_GRAPHEME_CLUSTER_BOUNDARY);
    break;

  case ANCHOR_PREC_READ:
    r = add_opcode(reg, OP_PREC_READ_START);
    if (r != 0) return r;
    r = compile_tree(NODE_ANCHOR_BODY(node), reg, env);
    if (r != 0) return r;
    r = add_opcode(reg, OP_PREC_READ_END);
    break;

  case ANCHOR_PREC_READ_NOT:
    len = compile_length_tree(NODE_ANCHOR_BODY(node), reg);
    if (len < 0) return len;
    r = add_opcode_rel_addr(reg, OP_PREC_READ_NOT_START, len + SIZE_OP_PREC_READ_NOT_END);
    if (r != 0) return r;
    r = compile_tree(NODE_ANCHOR_BODY(node), reg, env);
    if (r != 0) return r;
    r = add_opcode(reg, OP_PREC_READ_NOT_END);
    break;

  case ANCHOR_LOOK_BEHIND:
    {
      int n;
      r = add_opcode(reg, OP_LOOK_BEHIND);
      if (r != 0) return r;
      if (node->char_len < 0) {
        r = get_char_length_tree(NODE_ANCHOR_BODY(node), reg, &n);
        if (r != 0) return ONIGERR_INVALID_LOOK_BEHIND_PATTERN;
      }
      else
        n = node->char_len;

      r = add_length(reg, n);
      if (r != 0) return r;
      r = compile_tree(NODE_ANCHOR_BODY(node), reg, env);
    }
    break;

  case ANCHOR_LOOK_BEHIND_NOT:
    {
      int n;

      len = compile_length_tree(NODE_ANCHOR_BODY(node), reg);
      r = add_opcode_rel_addr(reg, OP_LOOK_BEHIND_NOT_START,
                              len + SIZE_OP_LOOK_BEHIND_NOT_END);
      if (r != 0) return r;
      if (node->char_len < 0) {
        r = get_char_length_tree(NODE_ANCHOR_BODY(node), reg, &n);
        if (r != 0) return ONIGERR_INVALID_LOOK_BEHIND_PATTERN;
      }
      else
        n = node->char_len;
      r = add_length(reg, n);
      if (r != 0) return r;
      r = compile_tree(NODE_ANCHOR_BODY(node), reg, env);
      if (r != 0) return r;
      r = add_opcode(reg, OP_LOOK_BEHIND_NOT_END);
    }
    break;

  default:
    return ONIGERR_TYPE_BUG;
    break;
  }

  return r;
}