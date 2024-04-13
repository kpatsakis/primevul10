acpi_os_physical_table_override(struct acpi_table_header *existing_table,
				acpi_physical_address *address, u32 *length)
{
	int table_offset = 0;
	int table_index = 0;
	struct acpi_table_header *table;
	u32 table_length;

	*length = 0;
	*address = 0;
	if (!acpi_tables_addr)
		return AE_OK;

	while (table_offset + ACPI_HEADER_SIZE <= all_tables_size) {
		table = acpi_os_map_memory(acpi_tables_addr + table_offset,
					   ACPI_HEADER_SIZE);
		if (table_offset + table->length > all_tables_size) {
			acpi_os_unmap_memory(table, ACPI_HEADER_SIZE);
			WARN_ON(1);
			return AE_OK;
		}

		table_length = table->length;

		/* Only override tables matched */
		if (test_bit(table_index, acpi_initrd_installed) ||
		    memcmp(existing_table->signature, table->signature, 4) ||
		    memcmp(table->oem_table_id, existing_table->oem_table_id,
			   ACPI_OEM_TABLE_ID_SIZE)) {
			acpi_os_unmap_memory(table, ACPI_HEADER_SIZE);
			goto next_table;
		}

		*length = table_length;
		*address = acpi_tables_addr + table_offset;
		acpi_table_taint(existing_table);
		acpi_os_unmap_memory(table, ACPI_HEADER_SIZE);
		set_bit(table_index, acpi_initrd_installed);
		break;

next_table:
		table_offset += table_length;
		table_index++;
	}
	return AE_OK;
}