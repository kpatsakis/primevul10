bash_transpose_shellwords (count, key)
     int count, key;
{
  char *word1, *word2;
  int w1_beg, w1_end, w2_beg, w2_end;
  int orig_point = rl_point;

  if (count == 0)
    return 0;

  /* Find the two shell words. */
  bash_forward_shellword (count, key);
  w2_end = rl_point;
  bash_backward_shellword (1, key);
  w2_beg = rl_point;
  bash_backward_shellword (count, key);
  w1_beg = rl_point;
  bash_forward_shellword (1, key);
  w1_end = rl_point;

  /* check that there really are two words. */
  if ((w1_beg == w2_beg) || (w2_beg < w1_end))
    {
      rl_ding ();
      rl_point = orig_point;
      return 1;
    }

  /* Get the text of the words. */
  word1 = rl_copy_text (w1_beg, w1_end);
  word2 = rl_copy_text (w2_beg, w2_end);

  /* We are about to do many insertions and deletions.  Remember them
     as one operation. */
  rl_begin_undo_group ();

  /* Do the stuff at word2 first, so that we don't have to worry
     about word1 moving. */
  rl_point = w2_beg;
  rl_delete_text (w2_beg, w2_end);
  rl_insert_text (word1);

  rl_point = w1_beg;
  rl_delete_text (w1_beg, w1_end);
  rl_insert_text (word2);

  /* This is exactly correct since the text before this point has not
     changed in length. */
  rl_point = w2_end;

  /* I think that does it. */
  rl_end_undo_group ();
  xfree (word1);
  xfree (word2);

  return 0;
}