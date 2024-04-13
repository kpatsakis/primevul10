compile_range_repeat_node(QuantNode* qn, int target_len, int empty_info,
                          regex_t* reg, ScanEnv* env)
{
  int r;
  int num_repeat = reg->num_repeat;

  r = add_opcode(reg, qn->greedy ? OP_REPEAT : OP_REPEAT_NG);
  if (r != 0) return r;
  r = add_mem_num(reg, num_repeat); /* OP_REPEAT ID */
  reg->num_repeat++;
  if (r != 0) return r;
  r = add_rel_addr(reg, target_len + SIZE_OP_REPEAT_INC);
  if (r != 0) return r;

  r = entry_repeat_range(reg, num_repeat, qn->lower, qn->upper);
  if (r != 0) return r;

  r = compile_tree_empty_check(NODE_QUANT_BODY(qn), reg, empty_info, env);
  if (r != 0) return r;

  if (
#ifdef USE_CALL
      NODE_IS_IN_MULTI_ENTRY(qn) ||
#endif
      NODE_IS_IN_REAL_REPEAT(qn)) {
    r = add_opcode(reg, qn->greedy ? OP_REPEAT_INC_SG : OP_REPEAT_INC_NG_SG);
  }
  else {
    r = add_opcode(reg, qn->greedy ? OP_REPEAT_INC : OP_REPEAT_INC_NG);
  }
  if (r != 0) return r;
  r = add_mem_num(reg, num_repeat); /* OP_REPEAT ID */
  return r;
}