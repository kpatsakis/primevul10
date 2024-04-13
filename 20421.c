compile_length_gimmick_node(GimmickNode* node, regex_t* reg)
{
  int len;

  switch (node->type) {
  case GIMMICK_FAIL:
    len = SIZE_OP_FAIL;
    break;

  case GIMMICK_KEEP:
  case GIMMICK_SAVE:
    len = SIZE_OP_PUSH_SAVE_VAL;
    break;

  case GIMMICK_UPDATE_VAR:
    len = SIZE_OP_UPDATE_VAR;
    break;

#ifdef USE_CALLOUT
  case GIMMICK_CALLOUT:
    switch (node->detail_type) {
    case ONIG_CALLOUT_OF_CONTENTS:
      len = SIZE_OP_CALLOUT_CONTENTS;
      break;
    case ONIG_CALLOUT_OF_NAME:
      len = SIZE_OP_CALLOUT_NAME;
      break;

    default:
      len = ONIGERR_TYPE_BUG;
      break;
    }
    break;
#endif
  }

  return len;
}