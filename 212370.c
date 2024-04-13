mrb_proc_arity(const struct RProc *p)
{
  const mrb_irep *irep;
  const mrb_code *pc;
  mrb_aspec aspec;
  int ma, op, ra, pa, arity;

  if (MRB_PROC_CFUNC_P(p)) {
    /* TODO cfunc aspec not implemented yet */
    return -1;
  }

  irep = p->body.irep;
  if (!irep) {
    return 0;
  }

  pc = irep->iseq;
  /* arity is depend on OP_ENTER */
  if (*pc != OP_ENTER) {
    return 0;
  }

  aspec = PEEK_W(pc+1);
  ma = MRB_ASPEC_REQ(aspec);
  op = MRB_ASPEC_OPT(aspec);
  ra = MRB_ASPEC_REST(aspec);
  pa = MRB_ASPEC_POST(aspec);
  arity = ra || (MRB_PROC_STRICT_P(p) && op) ? -(ma + pa + 1) : ma + pa;

  return arity;
}