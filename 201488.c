static int handle_dr(struct kvm_vcpu *vcpu, struct kvm_run *kvm_run)
{
	unsigned long exit_qualification;
	unsigned long val;
	int dr, reg;

	dr = vmcs_readl(GUEST_DR7);
	if (dr & DR7_GD) {
		/*
		 * As the vm-exit takes precedence over the debug trap, we
		 * need to emulate the latter, either for the host or the
		 * guest debugging itself.
		 */
		if (vcpu->guest_debug & KVM_GUESTDBG_USE_HW_BP) {
			kvm_run->debug.arch.dr6 = vcpu->arch.dr6;
			kvm_run->debug.arch.dr7 = dr;
			kvm_run->debug.arch.pc =
				vmcs_readl(GUEST_CS_BASE) +
				vmcs_readl(GUEST_RIP);
			kvm_run->debug.arch.exception = DB_VECTOR;
			kvm_run->exit_reason = KVM_EXIT_DEBUG;
			return 0;
		} else {
			vcpu->arch.dr7 &= ~DR7_GD;
			vcpu->arch.dr6 |= DR6_BD;
			vmcs_writel(GUEST_DR7, vcpu->arch.dr7);
			kvm_queue_exception(vcpu, DB_VECTOR);
			return 1;
		}
	}

	exit_qualification = vmcs_readl(EXIT_QUALIFICATION);
	dr = exit_qualification & DEBUG_REG_ACCESS_NUM;
	reg = DEBUG_REG_ACCESS_REG(exit_qualification);
	if (exit_qualification & TYPE_MOV_FROM_DR) {
		switch (dr) {
		case 0 ... 3:
			val = vcpu->arch.db[dr];
			break;
		case 6:
			val = vcpu->arch.dr6;
			break;
		case 7:
			val = vcpu->arch.dr7;
			break;
		default:
			val = 0;
		}
		kvm_register_write(vcpu, reg, val);
		KVMTRACE_2D(DR_READ, vcpu, (u32)dr, (u32)val, handler);
	} else {
		val = vcpu->arch.regs[reg];
		switch (dr) {
		case 0 ... 3:
			vcpu->arch.db[dr] = val;
			if (!(vcpu->guest_debug & KVM_GUESTDBG_USE_HW_BP))
				vcpu->arch.eff_db[dr] = val;
			break;
		case 4 ... 5:
			if (vcpu->arch.cr4 & X86_CR4_DE)
				kvm_queue_exception(vcpu, UD_VECTOR);
			break;
		case 6:
			if (val & 0xffffffff00000000ULL) {
				kvm_queue_exception(vcpu, GP_VECTOR);
				break;
			}
			vcpu->arch.dr6 = (val & DR6_VOLATILE) | DR6_FIXED_1;
			break;
		case 7:
			if (val & 0xffffffff00000000ULL) {
				kvm_queue_exception(vcpu, GP_VECTOR);
				break;
			}
			vcpu->arch.dr7 = (val & DR7_VOLATILE) | DR7_FIXED_1;
			if (!(vcpu->guest_debug & KVM_GUESTDBG_USE_HW_BP)) {
				vmcs_writel(GUEST_DR7, vcpu->arch.dr7);
				vcpu->arch.switch_db_regs =
					(val & DR7_BP_EN_MASK);
			}
			break;
		}
		KVMTRACE_2D(DR_WRITE, vcpu, (u32)dr, (u32)val, handler);
	}
	skip_emulated_instruction(vcpu);
	return 1;
}