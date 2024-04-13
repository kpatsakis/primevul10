mrb_proc_new_cfunc(mrb_state *mrb, mrb_func_t func)
{
  struct RProc *p;

  p = MRB_OBJ_ALLOC(mrb, MRB_TT_PROC, mrb->proc_class);
  p->body.func = func;
  p->flags |= MRB_PROC_CFUNC_FL;
  p->upper = 0;
  p->e.target_class = 0;

  return p;
}