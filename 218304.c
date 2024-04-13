static struct i915_ppgtt *vm_alias(struct i915_address_space *vm)
{
	if (i915_is_ggtt(vm))
		return i915_vm_to_ggtt(vm)->alias;
	else
		return i915_vm_to_ppgtt(vm);
}