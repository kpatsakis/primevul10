mrb_gc_register(mrb_state *mrb, mrb_value obj)
{
  mrb_sym root;
  mrb_value table;

  if (mrb_immediate_p(obj)) return;
  root = mrb_intern_lit(mrb, GC_ROOT_NAME);
  table = mrb_gv_get(mrb, root);
  if (mrb_nil_p(table) || !mrb_array_p(table)) {
    table = mrb_ary_new(mrb);
    mrb_gv_set(mrb, root, table);
  }
  mrb_ary_push(mrb, table, obj);
}