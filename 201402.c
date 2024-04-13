static void __vmx_load_host_state(struct vcpu_vmx *vmx)
{
	unsigned long flags;

	if (!vmx->host_state.loaded)
		return;

	++vmx->vcpu.stat.host_state_reload;
	vmx->host_state.loaded = 0;
	if (vmx->host_state.fs_reload_needed)
		kvm_load_fs(vmx->host_state.fs_sel);
	if (vmx->host_state.gs_ldt_reload_needed) {
		kvm_load_ldt(vmx->host_state.ldt_sel);
		/*
		 * If we have to reload gs, we must take care to
		 * preserve our gs base.
		 */
		local_irq_save(flags);
		kvm_load_gs(vmx->host_state.gs_sel);
#ifdef CONFIG_X86_64
		wrmsrl(MSR_GS_BASE, vmcs_readl(HOST_GS_BASE));
#endif
		local_irq_restore(flags);
	}
	reload_tss();
	save_msrs(vmx->guest_msrs, vmx->save_nmsrs);
	load_msrs(vmx->host_msrs, vmx->save_nmsrs);
	reload_host_efer(vmx);
}