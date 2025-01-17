static int save_guest_segment_descriptor(struct kvm_vcpu *vcpu, u16 selector,
					 struct desc_struct *seg_desc)
{
	gpa_t gpa;
	struct descriptor_table dtable;
	u16 index = selector >> 3;

	get_segment_descriptor_dtable(vcpu, selector, &dtable);

	if (dtable.limit < index * 8 + 7)
		return 1;
	gpa = vcpu->arch.mmu.gva_to_gpa(vcpu, dtable.base);
	gpa += index * 8;
	return kvm_write_guest(vcpu->kvm, gpa, seg_desc, 8);
}