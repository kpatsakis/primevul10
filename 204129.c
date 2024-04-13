grow_hunkmax (void)
{
    hunkmax *= 2;
    assert (p_line && p_len && p_Char);
    if ((p_line = realloc (p_line, hunkmax * sizeof (*p_line)))
	&& (p_len = realloc (p_len, hunkmax * sizeof (*p_len)))
	&& (p_Char = realloc (p_Char, hunkmax * sizeof (*p_Char))))
      return true;
    if (!using_plan_a)
      xalloc_die ();
    /* Don't free previous values of p_line etc.,
       since some broken implementations free them for us.
       Whatever is null will be allocated again from within plan_a (),
       of all places.  */
    return false;
}