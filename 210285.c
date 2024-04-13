llvm::Function* Decoder::splitFunctionOn(utils::Address addr)
{
	if (auto* bb = getBasicBlockAtAddress(addr))
	{
		LOG << "\t\t\t\t" << "S: splitFunctionOn @ " << addr << std::endl;
		return bb->getPrevNode()
				? splitFunctionOn(addr, bb)
				: bb->getParent();
	}
	// There is an instruction at address, but not BB -> do not split
	// existing blocks to create functions.
	//
	else if (auto ai = AsmInstruction(_module, addr))
	{
		if (ai.isInvalid())
		{
			LOG << "\t\t\t\t" << "S: invalid ASM @ " << addr << std::endl;
			return nullptr;
		}
		else
		{
			LOG << "\t\t\t\t" << "S: ASM @ " << addr << std::endl;
			return nullptr;
		}
	}
	else if (getFunctionContainingAddress(addr))
	{
		LOG << "\t\t\t\t" << "S: getFunctionContainingAddress() @ " << addr << std::endl;
		auto* before = getBasicBlockBeforeAddress(addr);
		assert(before);
		auto* newBb = createBasicBlock(addr, before->getParent(), before);
		return splitFunctionOn(addr, newBb);
	}
	else
	{
		LOG << "\t\t\t\t" << "S: createFunction() @ " << addr << std::endl;
		return createFunction(addr);
	}
}