static int on_fcn_new(RAnal *_anal, void* _user, RAnalFunction *fcn) {
	RCore *core = (RCore*)_user;
	const char *cmd = r_config_get (core->config, "cmd.fcn.new");
	if (cmd && *cmd) {
		ut64 oaddr = core->offset;
		ut64 addr = fcn->addr;
		r_core_seek (core, addr, 1);
		r_core_cmd0 (core, cmd);
		r_core_seek (core, oaddr, 1);
	}
	return 0;
}