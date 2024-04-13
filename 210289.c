llvm::BranchInst* Decoder::transformToBranch(
		llvm::CallInst* pseudo,
		llvm::BasicBlock* branchee)
{
	auto* term = pseudo->getParent()->getTerminator();
	assert(pseudo->getNextNode() == term);
	auto* br = BranchInst::Create(branchee, term);
	term->eraseFromParent();

	return br;
}