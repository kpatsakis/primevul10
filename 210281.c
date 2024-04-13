llvm::SwitchInst* Decoder::transformToSwitch(
		llvm::CallInst* pseudo,
		llvm::Value* val,
		llvm::BasicBlock* defaultBb,
		const std::vector<llvm::BasicBlock*>& cases)
{
	unsigned numCases = 0;
	for (auto* c : cases)
	{
		if (c != defaultBb)
		{
			++numCases;
		}
	}

	// If we do not do this, this can happen:
	// "Instruction does not dominate all uses"
	auto* insn = dyn_cast<Instruction>(val);
	if (insn && insn->getType())
	{
		auto* gv = new GlobalVariable(
				*insn->getModule(),
				insn->getType(),
				false,
				GlobalValue::ExternalLinkage,
				nullptr);
		auto* s = new StoreInst(insn, gv);
		s->insertAfter(insn);

		val = new LoadInst(gv, "", pseudo);
	}

	auto* term = pseudo->getParent()->getTerminator();
	assert(pseudo->getNextNode() == term);
	auto* intType = cast<IntegerType>(val->getType());
	auto* sw = SwitchInst::Create(val, defaultBb, numCases, term);
	unsigned cntr = 0;
	for (auto& c : cases)
	{
		if (c != defaultBb)
		{
			sw->addCase(ConstantInt::get(intType, cntr), c);
		}
		++cntr;
	}
	term->eraseFromParent();

	return sw;
}