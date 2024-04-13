compile_cclass_node(CClassNode* cc, regex_t* reg)
{
  int r;

  if (IS_NULL(cc->mbuf)) {
    if (IS_NCCLASS_NOT(cc))
      add_opcode(reg, OP_CCLASS_NOT);
    else
      add_opcode(reg, OP_CCLASS);

    r = add_bitset(reg, cc->bs);
  }
  else {
    if (ONIGENC_MBC_MINLEN(reg->enc) > 1 || bitset_is_empty(cc->bs)) {
      if (IS_NCCLASS_NOT(cc))
        add_opcode(reg, OP_CCLASS_MB_NOT);
      else
        add_opcode(reg, OP_CCLASS_MB);

      r = add_multi_byte_cclass(cc->mbuf, reg);
    }
    else {
      if (IS_NCCLASS_NOT(cc))
        add_opcode(reg, OP_CCLASS_MIX_NOT);
      else
        add_opcode(reg, OP_CCLASS_MIX);

      r = add_bitset(reg, cc->bs);
      if (r != 0) return r;
      r = add_multi_byte_cclass(cc->mbuf, reg);
    }
  }

  return r;
}