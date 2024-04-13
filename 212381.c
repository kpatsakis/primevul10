mrb_env_new(mrb_state *mrb, struct mrb_context *c, mrb_callinfo *ci, int nstacks, mrb_value *stack, struct RClass *tc)
{
  struct REnv *e;
  mrb_int bidx = 1;
  int n = ci->n;
  int nk = ci->nk;

  e = MRB_OBJ_ALLOC(mrb, MRB_TT_ENV, NULL);
  e->c = tc;
  MRB_ENV_SET_LEN(e, nstacks);
  bidx += (n == 15) ? 1 : n;
  bidx += (nk == 15) ? 1 : (2*nk);
  MRB_ENV_SET_BIDX(e, bidx);
  e->mid = ci->mid;
  e->stack = stack;
  e->cxt = c;

  return e;
}