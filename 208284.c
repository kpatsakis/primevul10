static void acpi_table_taint(struct acpi_table_header *table)
{
	pr_warn(PREFIX
		"Override [%4.4s-%8.8s], this is unsafe: tainting kernel\n",
		table->signature, table->oem_table_id);
	add_taint(TAINT_OVERRIDDEN_ACPI_TABLE, LOCKDEP_NOW_UNRELIABLE);
}