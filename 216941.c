mrb_incremental_gc(mrb_state *mrb)
{
  mrb_gc *gc = &mrb->gc;

  if (gc->disabled || gc->iterating) return;

  GC_INVOKE_TIME_REPORT("mrb_incremental_gc()");
  GC_TIME_START;

  if (is_minor_gc(gc)) {
    incremental_gc_until(mrb, gc, MRB_GC_STATE_ROOT);
  }
  else {
    incremental_gc_step(mrb, gc);
  }

  if (gc->state == MRB_GC_STATE_ROOT) {
    mrb_assert(gc->live >= gc->live_after_mark);
    gc->threshold = (gc->live_after_mark/100) * gc->interval_ratio;
    if (gc->threshold < GC_STEP_SIZE) {
      gc->threshold = GC_STEP_SIZE;
    }

    if (is_major_gc(gc)) {
      size_t threshold = gc->live_after_mark/100 * MAJOR_GC_INC_RATIO;

      gc->full = FALSE;
      if (threshold < MAJOR_GC_TOOMANY) {
        gc->majorgc_old_threshold = threshold;
      }
      else {
        /* too many objects allocated during incremental GC, */
        /* instead of increasing threshold, invoke full GC. */
        mrb_full_gc(mrb);
      }
    }
    else if (is_minor_gc(gc)) {
      if (gc->live > gc->majorgc_old_threshold) {
        clear_all_old(mrb, gc);
        gc->full = TRUE;
      }
    }
  }

  GC_TIME_STOP_AND_REPORT;
}