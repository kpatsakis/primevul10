proc_arity(mrb_state *mrb, mrb_value self)
{
  return mrb_int_value(mrb, mrb_proc_arity(mrb_proc_ptr(self)));
}