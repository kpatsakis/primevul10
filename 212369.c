mrb_proc_new_cfunc_with_env(mrb_state *mrb, mrb_func_t func, mrb_int argc, const mrb_value *argv)
{
  struct RProc *p = mrb_proc_new_cfunc(mrb, func);
  struct REnv *e;
  int i;

  p->e.env = e = mrb_env_new(mrb, mrb->c, mrb->c->ci, 0, NULL, NULL);
  p->flags |= MRB_PROC_ENVSET;
  mrb_field_write_barrier(mrb, (struct RBasic*)p, (struct RBasic*)e);
  MRB_ENV_CLOSE(e);

  e->stack = (mrb_value*)mrb_malloc(mrb, sizeof(mrb_value) * argc);
  MRB_ENV_SET_LEN(e, argc);

  if (argv) {
    for (i = 0; i < argc; ++i) {
      e->stack[i] = argv[i];
    }
  }
  else {
    for (i = 0; i < argc; ++i) {
      SET_NIL_VALUE(e->stack[i]);
    }
  }
  return p;
}