llvm::CallInst* Decoder::transformToCall(
		llvm::CallInst* pseudo,
		llvm::Function* callee)
{
	auto* c = CallInst::Create(callee);
	c->insertAfter(pseudo);

	if (auto* retObj = getCallReturnObject())
	{
		auto* cc = cast<Instruction>(
				IrModifier::convertValueToTypeAfter(c, retObj->getValueType(), c));
		auto* s = new StoreInst(cc, retObj);
		s->insertAfter(cc);
	}

	return c;
}