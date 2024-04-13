clear_opt_exact(OptExact* e)
{
  clear_mml(&e->mmd);
  clear_opt_anc_info(&e->anc);
  e->reach_end   = 0;
  e->ignore_case = 0;
  e->len         = 0;
  e->s[0]        = '\0';
}