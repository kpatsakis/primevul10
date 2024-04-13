void __init acpi_initrd_initialize_tables(void)
{
	int table_offset = 0;
	int table_index = 0;
	u32 table_length;
	struct acpi_table_header *table;

	if (!acpi_tables_addr)
		return;

	while (table_offset + ACPI_HEADER_SIZE <= all_tables_size) {
		table = acpi_os_map_memory(acpi_tables_addr + table_offset,
					   ACPI_HEADER_SIZE);
		if (table_offset + table->length > all_tables_size) {
			acpi_os_unmap_memory(table, ACPI_HEADER_SIZE);
			WARN_ON(1);
			return;
		}

		table_length = table->length;

		/* Skip RSDT/XSDT which should only be used for override */
		if (test_bit(table_index, acpi_initrd_installed) ||
		    ACPI_COMPARE_NAME(table->signature, ACPI_SIG_RSDT) ||
		    ACPI_COMPARE_NAME(table->signature, ACPI_SIG_XSDT)) {
			acpi_os_unmap_memory(table, ACPI_HEADER_SIZE);
			goto next_table;
		}

		acpi_table_taint(table);
		acpi_os_unmap_memory(table, ACPI_HEADER_SIZE);
		acpi_install_table(acpi_tables_addr + table_offset, TRUE);
		set_bit(table_index, acpi_initrd_installed);
next_table:
		table_offset += table_length;
		table_index++;
	}
}