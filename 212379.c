mrb_proc_merge_lvar(mrb_state *mrb, mrb_irep *irep, struct REnv *env, int num, const mrb_sym *lv, const mrb_value *stack)
{
  mrb_assert(!(irep->flags & MRB_IREP_NO_FREE));

  if ((irep->nlocals + num) > IREP_LVAR_MERGE_LIMIT) {
    mrb_raise(mrb, E_RUNTIME_ERROR, "too many local variables for binding (mruby limitation)");
  }

  if (!lv) {
    mrb_raise(mrb, E_RUNTIME_ERROR, "unavailable local variable names");
  }

  irep->lv = (mrb_sym*)mrb_realloc(mrb, (mrb_sym*)irep->lv, sizeof(mrb_sym) * (irep->nlocals + num));
  env->stack = (mrb_value*)mrb_realloc(mrb, env->stack, sizeof(mrb_value) * (irep->nlocals + 1 /* self */ + num));

  mrb_sym *destlv = (mrb_sym*)irep->lv + irep->nlocals - 1 /* self */;
  mrb_value *destst = env->stack + irep->nlocals;
  memmove(destlv, lv, sizeof(mrb_sym) * num);
  if (stack) {
    memmove(destst, stack, sizeof(mrb_value) * num);
    for (int i = 0; i < num; i++) {
      if (!mrb_immediate_p(stack[i])) {
        mrb_field_write_barrier(mrb, (struct RBasic*)env, (struct RBasic*)mrb_obj_ptr(stack[i]));
      }
    }
  }
  else {
    for (int i = num; i > 0; i--, destst++) {
      *destst = mrb_nil_value();
    }
  }
  irep->nlocals += num;
  irep->nregs = irep->nlocals;
  MRB_ENV_SET_LEN(env, irep->nlocals);
}