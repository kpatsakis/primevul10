compile_gimmick_node(GimmickNode* node, regex_t* reg)
{
  int r;

  switch (node->type) {
  case GIMMICK_FAIL:
    r = add_opcode(reg, OP_FAIL);
    break;

  case GIMMICK_KEEP:
    r = add_opcode(reg, OP_PUSH_SAVE_VAL);
    if (r != 0) return r;
    r = add_save_type(reg, SAVE_KEEP);
    if (r != 0) return r;
    r = add_mem_num(reg, node->id);
    break;

  case GIMMICK_SAVE:
    r = add_opcode(reg, OP_PUSH_SAVE_VAL);
    if (r != 0) return r;
    r = add_save_type(reg, node->detail_type);
    if (r != 0) return r;
    r = add_mem_num(reg, node->id);
    break;

  case GIMMICK_UPDATE_VAR:
    r = add_opcode(reg, OP_UPDATE_VAR);
    if (r != 0) return r;
    r = add_update_var_type(reg, node->detail_type);
    if (r != 0) return r;
    r = add_mem_num(reg, node->id);
    break;

#ifdef USE_CALLOUT
  case GIMMICK_CALLOUT:
    switch (node->detail_type) {
    case ONIG_CALLOUT_OF_CONTENTS:
    case ONIG_CALLOUT_OF_NAME:
      {
        r = add_opcode(reg, (node->detail_type == ONIG_CALLOUT_OF_CONTENTS) ?
                                  OP_CALLOUT_CONTENTS : OP_CALLOUT_NAME);
        if (r != 0) return r;
        if (node->detail_type == ONIG_CALLOUT_OF_NAME) {
          r = add_mem_num(reg, node->id);
          if (r != 0) return r;
        }
        r = add_mem_num(reg, node->num);
        if (r != 0) return r;
      }
      break;

    default:
      r = ONIGERR_TYPE_BUG;
      break;
    }
#endif
  }

  return r;
}