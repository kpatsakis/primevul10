compile_tree(Node* node, regex_t* reg, ScanEnv* env)
{
  int n, len, pos, r = 0;

  switch (NODE_TYPE(node)) {
  case NODE_LIST:
    do {
      r = compile_tree(NODE_CAR(node), reg, env);
    } while (r == 0 && IS_NOT_NULL(node = NODE_CDR(node)));
    break;

  case NODE_ALT:
    {
      Node* x = node;
      len = 0;
      do {
        len += compile_length_tree(NODE_CAR(x), reg);
        if (IS_NOT_NULL(NODE_CDR(x))) {
          len += SIZE_OP_PUSH + SIZE_OP_JUMP;
        }
      } while (IS_NOT_NULL(x = NODE_CDR(x)));
      pos = reg->used + len;  /* goal position */

      do {
        len = compile_length_tree(NODE_CAR(node), reg);
        if (IS_NOT_NULL(NODE_CDR(node))) {
          enum OpCode push = NODE_IS_SUPER(node) ? OP_PUSH_SUPER : OP_PUSH;
          r = add_opcode_rel_addr(reg, push, len + SIZE_OP_JUMP);
          if (r != 0) break;
        }
        r = compile_tree(NODE_CAR(node), reg, env);
        if (r != 0) break;
        if (IS_NOT_NULL(NODE_CDR(node))) {
          len = pos - (reg->used + SIZE_OP_JUMP);
          r = add_opcode_rel_addr(reg, OP_JUMP, len);
          if (r != 0) break;
        }
      } while (IS_NOT_NULL(node = NODE_CDR(node)));
    }
    break;

  case NODE_STRING:
    if (NODE_STRING_IS_RAW(node))
      r = compile_string_raw_node(STR_(node), reg);
    else
      r = compile_string_node(node, reg);
    break;

  case NODE_CCLASS:
    r = compile_cclass_node(CCLASS_(node), reg);
    break;

  case NODE_CTYPE:
    {
      int op;

      switch (CTYPE_(node)->ctype) {
      case CTYPE_ANYCHAR:
        if (IS_MULTILINE(CTYPE_OPTION(node, reg)))
          r = add_opcode(reg, OP_ANYCHAR_ML);
        else
          r = add_opcode(reg, OP_ANYCHAR);
        break;

      case ONIGENC_CTYPE_WORD:
        if (CTYPE_(node)->ascii_mode == 0) {
          op = CTYPE_(node)->not != 0 ? OP_NO_WORD : OP_WORD;
        }
        else {
          op = CTYPE_(node)->not != 0 ? OP_NO_WORD_ASCII : OP_WORD_ASCII;
        }
        r = add_opcode(reg, op);
        break;

      default:
        return ONIGERR_TYPE_BUG;
        break;
      }
    }
    break;

  case NODE_BACKREF:
    {
      BackRefNode* br = BACKREF_(node);

      if (NODE_IS_CHECKER(node)) {
#ifdef USE_BACKREF_WITH_LEVEL
        if (NODE_IS_NEST_LEVEL(node)) {
          r = add_opcode(reg, OP_BACKREF_CHECK_WITH_LEVEL);
          if (r != 0) return r;
          r = add_length(reg, br->nest_level);
          if (r != 0) return r;
        }
        else
#endif
          {
            r = add_opcode(reg, OP_BACKREF_CHECK);
            if (r != 0) return r;
          }

        goto add_bacref_mems;
      }
      else {
#ifdef USE_BACKREF_WITH_LEVEL
        if (NODE_IS_NEST_LEVEL(node)) {
          r = add_opcode(reg, OP_BACKREF_WITH_LEVEL);
          if (r != 0) return r;
          r = add_option(reg, (reg->options & ONIG_OPTION_IGNORECASE));
          if (r != 0) return r;
          r = add_length(reg, br->nest_level);
          if (r != 0) return r;

          goto add_bacref_mems;
        }
        else
#endif
        if (br->back_num == 1) {
          n = br->back_static[0];
          if (IS_IGNORECASE(reg->options)) {
            r = add_opcode(reg, OP_BACKREF_N_IC);
            if (r != 0) return r;
            r = add_mem_num(reg, n);
          }
          else {
            switch (n) {
            case 1:  r = add_opcode(reg, OP_BACKREF1); break;
            case 2:  r = add_opcode(reg, OP_BACKREF2); break;
            default:
              r = add_opcode(reg, OP_BACKREF_N);
              if (r != 0) return r;
              r = add_mem_num(reg, n);
              break;
            }
          }
        }
        else {
          int i;
          int* p;

          if (IS_IGNORECASE(reg->options)) {
            r = add_opcode(reg, OP_BACKREF_MULTI_IC);
          }
          else {
            r = add_opcode(reg, OP_BACKREF_MULTI);
          }
          if (r != 0) return r;

        add_bacref_mems:
          r = add_length(reg, br->back_num);
          if (r != 0) return r;
          p = BACKREFS_P(br);
          for (i = br->back_num - 1; i >= 0; i--) {
            r = add_mem_num(reg, p[i]);
            if (r != 0) return r;
          }
        }
      }
    }
    break;

#ifdef USE_CALL
  case NODE_CALL:
    r = compile_call(CALL_(node), reg, env);
    break;
#endif

  case NODE_QUANT:
    r = compile_quantifier_node(QUANT_(node), reg, env);
    break;

  case NODE_ENCLOSURE:
    r = compile_enclosure_node(ENCLOSURE_(node), reg, env);
    break;

  case NODE_ANCHOR:
    r = compile_anchor_node(ANCHOR_(node), reg, env);
    break;

  case NODE_GIMMICK:
    r = compile_gimmick_node(GIMMICK_(node), reg);
    break;

  default:
#ifdef ONIG_DEBUG
    fprintf(stderr, "compile_tree: undefined node type %d\n", NODE_TYPE(node));
#endif
    break;
  }

  return r;
}