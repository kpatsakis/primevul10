mrb_proc_new(mrb_state *mrb, const mrb_irep *irep)
{
  struct RProc *p;
  mrb_callinfo *ci = mrb->c->ci;

  p = MRB_OBJ_ALLOC(mrb, MRB_TT_PROC, mrb->proc_class);
  if (ci) {
    struct RClass *tc = NULL;

    if (ci->proc) {
      if (ci->proc->color != MRB_GC_RED) {
        tc = MRB_PROC_TARGET_CLASS(ci->proc);
      }
      else {
        tc = mrb_vm_ci_target_class(ci);
        if (tc && tc->tt == MRB_TT_ICLASS) {
          tc = tc->c;
        }
      }
    }
    if (tc == NULL) {
      tc = mrb_vm_ci_target_class(ci);
    }
    p->upper = ci->proc;
    p->e.target_class = tc;
  }
  p->body.irep = irep;
  if (irep) {
    mrb_irep_incref(mrb, (mrb_irep*)irep);
  }

  return p;
}