add_opcode_rel_addr(regex_t* reg, int opcode, int addr)
{
  int r;

  r = add_opcode(reg, opcode);
  if (r != 0) return r;
  r = add_rel_addr(reg, addr);
  return r;
}