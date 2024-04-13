static int set_guest_debug(struct kvm_vcpu *vcpu, struct kvm_guest_debug *dbg)
{
	int old_debug = vcpu->guest_debug;
	unsigned long flags;

	vcpu->guest_debug = dbg->control;
	if (!(vcpu->guest_debug & KVM_GUESTDBG_ENABLE))
		vcpu->guest_debug = 0;

	if (vcpu->guest_debug & KVM_GUESTDBG_USE_HW_BP)
		vmcs_writel(GUEST_DR7, dbg->arch.debugreg[7]);
	else
		vmcs_writel(GUEST_DR7, vcpu->arch.dr7);

	flags = vmcs_readl(GUEST_RFLAGS);
	if (vcpu->guest_debug & KVM_GUESTDBG_SINGLESTEP)
		flags |= X86_EFLAGS_TF | X86_EFLAGS_RF;
	else if (old_debug & KVM_GUESTDBG_SINGLESTEP)
		flags &= ~(X86_EFLAGS_TF | X86_EFLAGS_RF);
	vmcs_writel(GUEST_RFLAGS, flags);

	update_exception_bitmap(vcpu);

	return 0;
}