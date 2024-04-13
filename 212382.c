mrb_init_proc(mrb_state *mrb)
{
  mrb_method_t m;

  mrb_define_class_method(mrb, mrb->proc_class, "new", mrb_proc_s_new, MRB_ARGS_NONE()|MRB_ARGS_BLOCK());
  mrb_define_method(mrb, mrb->proc_class, "initialize_copy", mrb_proc_init_copy, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, mrb->proc_class, "arity", proc_arity, MRB_ARGS_NONE());

  MRB_METHOD_FROM_PROC(m, &call_proc);
  mrb_define_method_raw(mrb, mrb->proc_class, MRB_SYM(call), m);
  mrb_define_method_raw(mrb, mrb->proc_class, MRB_OPSYM(aref), m);

  mrb_define_class_method(mrb, mrb->kernel_module, "lambda", proc_lambda, MRB_ARGS_NONE()|MRB_ARGS_BLOCK()); /* 15.3.1.2.6  */
  mrb_define_method(mrb, mrb->kernel_module,       "lambda", proc_lambda, MRB_ARGS_NONE()|MRB_ARGS_BLOCK()); /* 15.3.1.3.27 */
}