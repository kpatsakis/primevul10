mrb_proc_get_caller(mrb_state *mrb, struct REnv **envp)
{
  struct mrb_context *c = mrb->c;
  mrb_callinfo *ci = (c->ci > c->cibase) ? c->ci - 1 : c->cibase;
  const struct RProc *proc = ci->proc;

  if (!proc || MRB_PROC_CFUNC_P(proc)) {
    if (envp) *envp = NULL;
  }
  else {
    struct RClass *tc = MRB_PROC_TARGET_CLASS(proc);
    struct REnv *e = mrb_vm_ci_env(ci);

    if (e == NULL) {
      int nstacks = proc->body.irep->nlocals;
      e = mrb_env_new(mrb, c, ci, nstacks, ci->stack, tc);
      ci->u.env = e;
    }
    else if (tc) {
      e->c = tc;
      mrb_field_write_barrier(mrb, (struct RBasic*)e, (struct RBasic*)tc);
    }
    if (envp) *envp = e;
  }

  return proc;
}