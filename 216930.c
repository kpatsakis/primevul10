mrb_object_dead_p(mrb_state *mrb, struct RBasic *object) {
  mrb_gc *gc = &mrb->gc;
  if (!heap_p(gc, object)) return TRUE;
  return is_dead(gc, object);
}