R_API RTable *r_core_table(RCore *core) {
	RTable *table = r_table_new ();
	if (table) {
		table->cons = core->cons;
	}
	return table;
}