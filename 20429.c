compile_length_tree(Node* node, regex_t* reg)
{
  int len, r;

  switch (NODE_TYPE(node)) {
  case NODE_LIST:
    len = 0;
    do {
      r = compile_length_tree(NODE_CAR(node), reg);
      if (r < 0) return r;
      len += r;
    } while (IS_NOT_NULL(node = NODE_CDR(node)));
    r = len;
    break;

  case NODE_ALT:
    {
      int n;

      n = r = 0;
      do {
        r += compile_length_tree(NODE_CAR(node), reg);
        n++;
      } while (IS_NOT_NULL(node = NODE_CDR(node)));
      r += (SIZE_OP_PUSH + SIZE_OP_JUMP) * (n - 1);
    }
    break;

  case NODE_STRING:
    if (NODE_STRING_IS_RAW(node))
      r = compile_length_string_raw_node(STR_(node), reg);
    else
      r = compile_length_string_node(node, reg);
    break;

  case NODE_CCLASS:
    r = compile_length_cclass_node(CCLASS_(node), reg);
    break;

  case NODE_CTYPE:
    r = SIZE_OPCODE;
    break;

  case NODE_BACKREF:
    {
      BackRefNode* br = BACKREF_(node);

      if (NODE_IS_CHECKER(node)) {
#ifdef USE_BACKREF_WITH_LEVEL
        if (NODE_IS_NEST_LEVEL(node)) {
          r = SIZE_OPCODE + SIZE_LENGTH + SIZE_LENGTH + (SIZE_MEMNUM * br->back_num);
        }
        else
#endif
          r = SIZE_OPCODE + SIZE_LENGTH + (SIZE_MEMNUM * br->back_num);
      }
      else {
#ifdef USE_BACKREF_WITH_LEVEL
        if (NODE_IS_NEST_LEVEL(node)) {
          r = SIZE_OPCODE + SIZE_OPTION + SIZE_LENGTH +
            SIZE_LENGTH + (SIZE_MEMNUM * br->back_num);
        }
        else
#endif
        if (br->back_num == 1) {
          r = ((!IS_IGNORECASE(reg->options) && br->back_static[0] <= 2)
               ? SIZE_OPCODE : (SIZE_OPCODE + SIZE_MEMNUM));
        }
        else {
          r = SIZE_OPCODE + SIZE_LENGTH + (SIZE_MEMNUM * br->back_num);
        }
      }
    }
    break;

#ifdef USE_CALL
  case NODE_CALL:
    r = SIZE_OP_CALL;
    break;
#endif

  case NODE_QUANT:
    r = compile_length_quantifier_node(QUANT_(node), reg);
    break;

  case NODE_ENCLOSURE:
    r = compile_length_enclosure_node(ENCLOSURE_(node), reg);
    break;

  case NODE_ANCHOR:
    r = compile_length_anchor_node(ANCHOR_(node), reg);
    break;

  case NODE_GIMMICK:
    r = compile_length_gimmick_node(GIMMICK_(node), reg);
    break;

  default:
    return ONIGERR_TYPE_BUG;
    break;
  }

  return r;
}