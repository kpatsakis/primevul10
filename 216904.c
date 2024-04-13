mrb_gc_unregister(mrb_state *mrb, mrb_value obj)
{
  mrb_sym root;
  mrb_value table;
  struct RArray *a;
  mrb_int i;

  if (mrb_immediate_p(obj)) return;
  root = mrb_intern_lit(mrb, GC_ROOT_NAME);
  table = mrb_gv_get(mrb, root);
  if (mrb_nil_p(table)) return;
  if (!mrb_array_p(table)) {
    mrb_gv_set(mrb, root, mrb_nil_value());
    return;
  }
  a = mrb_ary_ptr(table);
  mrb_ary_modify(mrb, a);
  for (i = 0; i < ARY_LEN(a); i++) {
    if (mrb_ptr(ARY_PTR(a)[i]) == mrb_ptr(obj)) {
      mrb_int len = ARY_LEN(a)-1;
      mrb_value *ptr = ARY_PTR(a);

      ARY_SET_LEN(a, len);
      memmove(&ptr[i], &ptr[i + 1], (len - i) * sizeof(mrb_value));
      break;
    }
  }
}