R_API void r_core_bind_cons(RCore *core) {
	core->cons->num = core->num;
	core->cons->cb_fkey = (RConsFunctionKey)__cons_cb_fkey;
	core->cons->cb_editor = (RConsEditorCallback)r_core_editor;
	core->cons->cb_break = (RConsBreakCallback)r_core_break;
	core->cons->cb_sleep_begin = (RConsSleepBeginCallback)r_core_sleep_begin;
	core->cons->cb_sleep_end = (RConsSleepEndCallback)r_core_sleep_end;
	core->cons->cb_task_oneshot = (RConsQueueTaskOneshot) r_core_task_enqueue_oneshot;
	core->cons->user = (void*)core;
}