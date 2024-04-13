incremental_marking_phase(mrb_state *mrb, mrb_gc *gc, size_t limit)
{
  size_t tried_marks = 0;

  while (gc->gray_list && tried_marks < limit) {
    struct RBasic *obj = gc->gray_list;
    gc_mark_children(mrb, gc, obj);
    tried_marks += gc_gray_counts(mrb, gc, obj);
  }

  return tried_marks;
}