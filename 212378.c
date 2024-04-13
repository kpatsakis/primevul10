mrb_proc_local_variables(mrb_state *mrb, const struct RProc *proc)
{
  const mrb_irep *irep;
  mrb_value vars;
  size_t i;

  if (proc == NULL || MRB_PROC_CFUNC_P(proc)) {
    return mrb_ary_new(mrb);
  }
  vars = mrb_hash_new(mrb);
  while (proc) {
    if (MRB_PROC_CFUNC_P(proc)) break;
    irep = proc->body.irep;
    if (irep->lv) {
      for (i = 0; i + 1 < irep->nlocals; ++i) {
        if (irep->lv[i]) {
          mrb_sym sym = irep->lv[i];
          const char *name = mrb_sym_name(mrb, sym);
          switch (name[0]) {
          case '*': case '&':
            break;
          default:
            mrb_hash_set(mrb, vars, mrb_symbol_value(sym), mrb_true_value());
            break;
          }
        }
      }
    }
    if (MRB_PROC_SCOPE_P(proc)) break;
    proc = proc->upper;
  }

  return mrb_hash_keys(mrb, vars);
}