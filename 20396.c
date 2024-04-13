compile_enclosure_node(EnclosureNode* node, regex_t* reg, ScanEnv* env)
{
  int r, len;

  switch (node->type) {
  case ENCLOSURE_MEMORY:
    r = compile_enclosure_memory_node(node, reg, env);
    break;

  case ENCLOSURE_OPTION:
    r = compile_option_node(node, reg, env);
    break;

  case ENCLOSURE_STOP_BACKTRACK:
    if (NODE_IS_STOP_BT_SIMPLE_REPEAT(node)) {
      QuantNode* qn = QUANT_(NODE_ENCLOSURE_BODY(node));
      r = compile_tree_n_times(NODE_QUANT_BODY(qn), qn->lower, reg, env);
      if (r != 0) return r;

      len = compile_length_tree(NODE_QUANT_BODY(qn), reg);
      if (len < 0) return len;

      r = add_opcode_rel_addr(reg, OP_PUSH, len + SIZE_OP_POP_OUT + SIZE_OP_JUMP);
      if (r != 0) return r;
      r = compile_tree(NODE_QUANT_BODY(qn), reg, env);
      if (r != 0) return r;
      r = add_opcode(reg, OP_POP_OUT);
      if (r != 0) return r;
      r = add_opcode_rel_addr(reg, OP_JUMP,
           -((int )SIZE_OP_PUSH + len + (int )SIZE_OP_POP_OUT + (int )SIZE_OP_JUMP));
    }
    else {
      r = add_opcode(reg, OP_ATOMIC_START);
      if (r != 0) return r;
      r = compile_tree(NODE_ENCLOSURE_BODY(node), reg, env);
      if (r != 0) return r;
      r = add_opcode(reg, OP_ATOMIC_END);
    }
    break;

  case ENCLOSURE_IF_ELSE:
    {
      int cond_len, then_len, jump_len;
      Node* cond = NODE_ENCLOSURE_BODY(node);
      Node* Then = node->te.Then;
      Node* Else = node->te.Else;

      r = add_opcode(reg, OP_ATOMIC_START);
      if (r != 0) return r;

      cond_len = compile_length_tree(cond, reg);
      if (cond_len < 0) return cond_len;
      if (IS_NOT_NULL(Then)) {
        then_len = compile_length_tree(Then, reg);
        if (then_len < 0) return then_len;
      }
      else
        then_len = 0;

      jump_len = cond_len + then_len + SIZE_OP_ATOMIC_END;
      if (IS_NOT_NULL(Else)) jump_len += SIZE_OP_JUMP;

      r = add_opcode_rel_addr(reg, OP_PUSH, jump_len);
      if (r != 0) return r;
      r = compile_tree(cond, reg, env);
      if (r != 0) return r;
      r = add_opcode(reg, OP_ATOMIC_END);
      if (r != 0) return r;

      if (IS_NOT_NULL(Then)) {
        r = compile_tree(Then, reg, env);
        if (r != 0) return r;
      }

      if (IS_NOT_NULL(Else)) {
        int else_len = compile_length_tree(Else, reg);
        r = add_opcode_rel_addr(reg, OP_JUMP, else_len);
        if (r != 0) return r;
        r = compile_tree(Else, reg, env);
      }
    }
    break;

  default:
    return ONIGERR_TYPE_BUG;
    break;
  }

  return r;
}