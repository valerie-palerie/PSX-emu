#include "CXD8530BQ.h"
#include "Playstation.h"
#include "MathUtils.h"
#include "MemoryInterface.h"
#include "DMAController.h"

void CXD8530BQ::Init()
{
	_cw33300.Init();
	_cop0.Init();
	_gte.Init();
}

void CXD8530BQ::Tick(double deltaT)
{
	if (_dma->status() == DMAController::Status::Active)
		return;

#if DEBUG_LOG_ENABLED
	std::cout << "\n\n			----CLOCK----\n";
#endif

	_cw33300.ProcessNextInstruction();

	ProcessDelayStores();
	CheckPendingInterrupts();
}

void CXD8530BQ::ProcessDelayStores()
{
	for (size_t i = 0; i < _delayMemStores.size(); ++i)
	{
		if (_delayMemStores[i]->Tick())
		{
			_delayMemStores.erase(_delayMemStores.begin() + i);
			--i;
		}
	}
}

void CXD8530BQ::CheckPendingInterrupts()
{
	uint32 iStat;
	uint32 iMask;
	playstation()->memInterface()->Read<uint32>(MemoryMap::INTERRUPT_CONTROLLER_BASE, iStat);
	playstation()->memInterface()->Read<uint32>(MemoryMap::INTERRUPT_CONTROLLER_BASE + 0x4, iMask);

	uint32 cop0r13 = _cop0.GetRegister(13);
	_cop0.SetRegisterImmediate(13, Math::ToggleBit(cop0r13, 10, iStat & iMask));

	//SR bit 0, current interrupt enable/disable
	if (!Math::GetBit(_cop0.GetRegister(12), 0))
		return;

	if (Math::GetBits(_cop0.GetRegister(13), 8, 10) != 0)
	{
		uint32 cop0r12 = _cop0.GetRegister(12);
		if (Math::GetBits(cop0r12, 8, 15) & Math::GetBits(cop0r13, 8, 15))
			RaiseException(ExceptionType::INT);
	}
}

void CXD8530BQ::ExecuteInstruction(Opcode opcode)
{
	_cw33300.ExecuteInstruction(opcode);
}

void CXD8530BQ::RaiseException(ExceptionType exceptionType, uint32 data)
{
	bool isBranchDelay = _cw33300.isExecutingDelaySlot();

	//If we're executing a branch delay slot, we have to return to the branch instruction before it
	_cop0.SetRegister(14, isBranchDelay ? (_cw33300._currentInstructionAddress - 4) : _cw33300._currentInstructionAddress);

	uint32 causeReg = _cop0.GetRegister(13);
	causeReg = Math::SetBits(causeReg, 6, 2, uint32(exceptionType));
	causeReg = Math::ToggleBit(causeReg, 31, isBranchDelay);
	_cop0.SetRegisterImmediate(13, causeReg);

	uint32 cop0sr = _cop0.GetRegister(12);
	cop0sr = Math::SetBits(cop0sr, 5, 0, cop0sr << 2);
	_cop0.SetRegisterImmediate(12, cop0sr);

	uint32 handler 
		= Math::GetBit(cop0sr, 22)
		? MemoryMap::EXCEPTION_ROM_HANDLER
		: MemoryMap::EXCEPTION_RAM_HANDLER;

	switch (exceptionType)
	{
	case ExceptionType::Syscall:
		break;
	case ExceptionType::AdEL:
	case ExceptionType::AdES:
		_cop0.SetRegister(8, data);
		__debugbreak();
		break;
	default:
		__debugbreak();
		break;
	}

	_cw33300.MoveExecutionTo(handler);
}

CXD8530BQ::CXD8530BQ(Playstation* playstation)
	: PlaystationComponent(playstation)
	, _cw33300(this)
	, _cop0(this)
	, _gte(this)
	, _dma(playstation->dma())
{

}
