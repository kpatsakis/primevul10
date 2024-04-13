closure_setup(mrb_state *mrb, struct RProc *p)
{
  mrb_callinfo *ci = mrb->c->ci;
  const struct RProc *up = p->upper;
  struct REnv *e = NULL;

  if (ci && (e = mrb_vm_ci_env(ci)) != NULL) {
    /* do nothing, because e is assigned already */
  }
  else if (up) {
    struct RClass *tc = ci->u.target_class;

    e = mrb_env_new(mrb, mrb->c, ci, up->body.irep->nlocals, ci->stack, tc);
    ci->u.env = e;
    if (MRB_PROC_ENV_P(up) && MRB_PROC_ENV(up)->cxt == NULL) {
      e->mid = MRB_PROC_ENV(up)->mid;
    }
  }
  if (e) {
    p->e.env = e;
    p->flags |= MRB_PROC_ENVSET;
    mrb_field_write_barrier(mrb, (struct RBasic*)p, (struct RBasic*)e);
  }
}