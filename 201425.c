static int vmx_vcpu_setup(struct vcpu_vmx *vmx)
{
	u32 host_sysenter_cs, msr_low, msr_high;
	u32 junk;
	u64 host_pat, tsc_this, tsc_base;
	unsigned long a;
	struct descriptor_table dt;
	int i;
	unsigned long kvm_vmx_return;
	u32 exec_control;

	/* I/O */
	vmcs_write64(IO_BITMAP_A, page_to_phys(vmx_io_bitmap_a));
	vmcs_write64(IO_BITMAP_B, page_to_phys(vmx_io_bitmap_b));

	if (cpu_has_vmx_msr_bitmap())
		vmcs_write64(MSR_BITMAP, page_to_phys(vmx_msr_bitmap));

	vmcs_write64(VMCS_LINK_POINTER, -1ull); /* 22.3.1.5 */

	/* Control */
	vmcs_write32(PIN_BASED_VM_EXEC_CONTROL,
		vmcs_config.pin_based_exec_ctrl);

	exec_control = vmcs_config.cpu_based_exec_ctrl;
	if (!vm_need_tpr_shadow(vmx->vcpu.kvm)) {
		exec_control &= ~CPU_BASED_TPR_SHADOW;
#ifdef CONFIG_X86_64
		exec_control |= CPU_BASED_CR8_STORE_EXITING |
				CPU_BASED_CR8_LOAD_EXITING;
#endif
	}
	if (!vm_need_ept())
		exec_control |= CPU_BASED_CR3_STORE_EXITING |
				CPU_BASED_CR3_LOAD_EXITING  |
				CPU_BASED_INVLPG_EXITING;
	vmcs_write32(CPU_BASED_VM_EXEC_CONTROL, exec_control);

	if (cpu_has_secondary_exec_ctrls()) {
		exec_control = vmcs_config.cpu_based_2nd_exec_ctrl;
		if (!vm_need_virtualize_apic_accesses(vmx->vcpu.kvm))
			exec_control &=
				~SECONDARY_EXEC_VIRTUALIZE_APIC_ACCESSES;
		if (vmx->vpid == 0)
			exec_control &= ~SECONDARY_EXEC_ENABLE_VPID;
		if (!vm_need_ept())
			exec_control &= ~SECONDARY_EXEC_ENABLE_EPT;
		vmcs_write32(SECONDARY_VM_EXEC_CONTROL, exec_control);
	}

	vmcs_write32(PAGE_FAULT_ERROR_CODE_MASK, !!bypass_guest_pf);
	vmcs_write32(PAGE_FAULT_ERROR_CODE_MATCH, !!bypass_guest_pf);
	vmcs_write32(CR3_TARGET_COUNT, 0);           /* 22.2.1 */

	vmcs_writel(HOST_CR0, read_cr0());  /* 22.2.3 */
	vmcs_writel(HOST_CR4, read_cr4());  /* 22.2.3, 22.2.5 */
	vmcs_writel(HOST_CR3, read_cr3());  /* 22.2.3  FIXME: shadow tables */

	vmcs_write16(HOST_CS_SELECTOR, __KERNEL_CS);  /* 22.2.4 */
	vmcs_write16(HOST_DS_SELECTOR, __KERNEL_DS);  /* 22.2.4 */
	vmcs_write16(HOST_ES_SELECTOR, __KERNEL_DS);  /* 22.2.4 */
	vmcs_write16(HOST_FS_SELECTOR, kvm_read_fs());    /* 22.2.4 */
	vmcs_write16(HOST_GS_SELECTOR, kvm_read_gs());    /* 22.2.4 */
	vmcs_write16(HOST_SS_SELECTOR, __KERNEL_DS);  /* 22.2.4 */
#ifdef CONFIG_X86_64
	rdmsrl(MSR_FS_BASE, a);
	vmcs_writel(HOST_FS_BASE, a); /* 22.2.4 */
	rdmsrl(MSR_GS_BASE, a);
	vmcs_writel(HOST_GS_BASE, a); /* 22.2.4 */
#else
	vmcs_writel(HOST_FS_BASE, 0); /* 22.2.4 */
	vmcs_writel(HOST_GS_BASE, 0); /* 22.2.4 */
#endif

	vmcs_write16(HOST_TR_SELECTOR, GDT_ENTRY_TSS*8);  /* 22.2.4 */

	kvm_get_idt(&dt);
	vmcs_writel(HOST_IDTR_BASE, dt.base);   /* 22.2.4 */

	asm("mov $.Lkvm_vmx_return, %0" : "=r"(kvm_vmx_return));
	vmcs_writel(HOST_RIP, kvm_vmx_return); /* 22.2.5 */
	vmcs_write32(VM_EXIT_MSR_STORE_COUNT, 0);
	vmcs_write32(VM_EXIT_MSR_LOAD_COUNT, 0);
	vmcs_write32(VM_ENTRY_MSR_LOAD_COUNT, 0);

	rdmsr(MSR_IA32_SYSENTER_CS, host_sysenter_cs, junk);
	vmcs_write32(HOST_IA32_SYSENTER_CS, host_sysenter_cs);
	rdmsrl(MSR_IA32_SYSENTER_ESP, a);
	vmcs_writel(HOST_IA32_SYSENTER_ESP, a);   /* 22.2.3 */
	rdmsrl(MSR_IA32_SYSENTER_EIP, a);
	vmcs_writel(HOST_IA32_SYSENTER_EIP, a);   /* 22.2.3 */

	if (vmcs_config.vmexit_ctrl & VM_EXIT_LOAD_IA32_PAT) {
		rdmsr(MSR_IA32_CR_PAT, msr_low, msr_high);
		host_pat = msr_low | ((u64) msr_high << 32);
		vmcs_write64(HOST_IA32_PAT, host_pat);
	}
	if (vmcs_config.vmentry_ctrl & VM_ENTRY_LOAD_IA32_PAT) {
		rdmsr(MSR_IA32_CR_PAT, msr_low, msr_high);
		host_pat = msr_low | ((u64) msr_high << 32);
		/* Write the default value follow host pat */
		vmcs_write64(GUEST_IA32_PAT, host_pat);
		/* Keep arch.pat sync with GUEST_IA32_PAT */
		vmx->vcpu.arch.pat = host_pat;
	}

	for (i = 0; i < NR_VMX_MSR; ++i) {
		u32 index = vmx_msr_index[i];
		u32 data_low, data_high;
		u64 data;
		int j = vmx->nmsrs;

		if (rdmsr_safe(index, &data_low, &data_high) < 0)
			continue;
		if (wrmsr_safe(index, data_low, data_high) < 0)
			continue;
		data = data_low | ((u64)data_high << 32);
		vmx->host_msrs[j].index = index;
		vmx->host_msrs[j].reserved = 0;
		vmx->host_msrs[j].data = data;
		vmx->guest_msrs[j] = vmx->host_msrs[j];
		++vmx->nmsrs;
	}

	vmcs_write32(VM_EXIT_CONTROLS, vmcs_config.vmexit_ctrl);

	/* 22.2.1, 20.8.1 */
	vmcs_write32(VM_ENTRY_CONTROLS, vmcs_config.vmentry_ctrl);

	vmcs_writel(CR0_GUEST_HOST_MASK, ~0UL);
	vmcs_writel(CR4_GUEST_HOST_MASK, KVM_GUEST_CR4_MASK);

	tsc_base = vmx->vcpu.kvm->arch.vm_init_tsc;
	rdtscll(tsc_this);
	if (tsc_this < vmx->vcpu.kvm->arch.vm_init_tsc)
		tsc_base = tsc_this;

	guest_write_tsc(0, tsc_base);

	return 0;
}