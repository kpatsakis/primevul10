add_multi_byte_cclass(BBuf* mbuf, regex_t* reg)
{
#ifdef PLATFORM_UNALIGNED_WORD_ACCESS
  add_length(reg, mbuf->used);
  return add_bytes(reg, mbuf->p, mbuf->used);
#else
  int r, pad_size;
  UChar* p = BB_GET_ADD_ADDRESS(reg) + SIZE_LENGTH;

  GET_ALIGNMENT_PAD_SIZE(p, pad_size);
  add_length(reg, mbuf->used + (WORD_ALIGNMENT_SIZE - 1));
  if (pad_size != 0) add_bytes(reg, PadBuf, pad_size);

  r = add_bytes(reg, mbuf->p, mbuf->used);

  /* padding for return value from compile_length_cclass_node() to be fix. */
  pad_size = (WORD_ALIGNMENT_SIZE - 1) - pad_size;
  if (pad_size != 0) add_bytes(reg, PadBuf, pad_size);
  return r;
#endif
}