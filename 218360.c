dvbs2_defragment_init(void)
{
  reassembly_table_init(&dvbs2_reassembly_table,
                        &addresses_reassembly_table_functions);
}