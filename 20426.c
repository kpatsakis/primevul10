compile_call(CallNode* node, regex_t* reg, ScanEnv* env)
{
  int r;

  r = add_opcode(reg, OP_CALL);
  if (r != 0) return r;
  r = unset_addr_list_add(env->unset_addr_list, BB_GET_OFFSET_POS(reg),
                          NODE_CALL_BODY(node));
  if (r != 0) return r;
  r = add_abs_addr(reg, 0 /*dummy addr.*/);
  return r;
}