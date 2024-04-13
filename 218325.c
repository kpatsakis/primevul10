static void virtual_update_register_offsets(u32 *regs,
					    struct intel_engine_cs *engine)
{
	set_offsets(regs, reg_offsets(engine), engine);
}