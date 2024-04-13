init_word_char (re_dfa_t *dfa)
{
  int i = 0;
  int j;
  int ch = 0;
  dfa->word_ops_used = 1;
  if (BE (dfa->map_notascii == 0, 1))
    {
      bitset_word_t bits0 = 0x00000000;
      bitset_word_t bits1 = 0x03ff0000;
      bitset_word_t bits2 = 0x87fffffe;
      bitset_word_t bits3 = 0x07fffffe;
      if (BITSET_WORD_BITS == 64)
	{
	  dfa->word_char[0] = bits1 << 31 << 1 | bits0;
	  dfa->word_char[1] = bits3 << 31 << 1 | bits2;
	  i = 2;
	}
      else if (BITSET_WORD_BITS == 32)
	{
	  dfa->word_char[0] = bits0;
	  dfa->word_char[1] = bits1;
	  dfa->word_char[2] = bits2;
	  dfa->word_char[3] = bits3;
	  i = 4;
	}
      else
        goto general_case;
      ch = 128;

      if (BE (dfa->is_utf8, 1))
	{
	  memset (&dfa->word_char[i], '\0', (SBC_MAX - ch) / 8);
	  return;
	}
    }

 general_case:
  for (; i < BITSET_WORDS; ++i)
    for (j = 0; j < BITSET_WORD_BITS; ++j, ++ch)
      if (isalnum (ch) || ch == '_')
	dfa->word_char[i] |= (bitset_word_t) 1 << j;
}