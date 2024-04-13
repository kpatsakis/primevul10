llvm::CallInst* Decoder::transformToCondCall(
		llvm::CallInst* pseudo,
		llvm::Value* cond,
		llvm::Function* callee,
		llvm::BasicBlock* falseBb)
{
	auto* oldBb = pseudo->getParent();
	auto* newBb = oldBb->splitBasicBlock(pseudo);
	// We do NOT want to name or give address to this block.

	auto* oldTerm = oldBb->getTerminator();
	BranchInst::Create(newBb, falseBb, cond, oldTerm);
	oldTerm->eraseFromParent();

	auto* newTerm = newBb->getTerminator();
	BranchInst::Create(falseBb, newTerm);
	newTerm->eraseFromParent();

	auto* c = CallInst::Create(callee);
	c->insertAfter(pseudo);

	return c;
}