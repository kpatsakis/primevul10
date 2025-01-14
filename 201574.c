void kvm_queue_exception_e(struct kvm_vcpu *vcpu, unsigned nr, u32 error_code)
{
	WARN_ON(vcpu->arch.exception.pending);
	vcpu->arch.exception.pending = true;
	vcpu->arch.exception.has_error_code = true;
	vcpu->arch.exception.nr = nr;
	vcpu->arch.exception.error_code = error_code;
}