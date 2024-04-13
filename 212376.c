mrb_proc_cfunc_env_get(mrb_state *mrb, mrb_int idx)
{
  const struct RProc *p = mrb->c->ci->proc;
  struct REnv *e;

  if (!p || !MRB_PROC_CFUNC_P(p)) {
    mrb_raise(mrb, E_TYPE_ERROR, "Can't get cfunc env from non-cfunc proc");
  }
  e = MRB_PROC_ENV(p);
  if (!e) {
    mrb_raise(mrb, E_TYPE_ERROR, "Can't get cfunc env from cfunc Proc without REnv");
  }
  if (idx < 0 || MRB_ENV_LEN(e) <= idx) {
    mrb_raisef(mrb, E_INDEX_ERROR, "Env index out of range: %i (expected: 0 <= index < %i)",
               idx, MRB_ENV_LEN(e));
  }

  return e->stack[idx];
}