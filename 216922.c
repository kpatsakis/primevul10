gc_mark_gray_list(mrb_state *mrb, mrb_gc *gc) {
  while (gc->gray_list) {
    if (is_gray(gc->gray_list))
      gc_mark_children(mrb, gc, gc->gray_list);
    else
      gc->gray_list = gc->gray_list->gcnext;
  }
}