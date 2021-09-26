#include "CXD8530BQ.h"
#include "Playstation.h"
#include "MathUtils.h"

void CXD8530BQ::Init()
{
	_cw33300.Init();
	_cop0.Init();
	_gte.Init();
}

void CXD8530BQ::Clock()
{
#if DEBUG_LOG_ENABLED
	std::cout << "\n\n			----CLOCK----\n";
#endif

	_cw33300.ProcessNextInstruction();

	ProcessDelayStores();
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

void CXD8530BQ::ExecuteInstruction(Opcode opcode)
{
	_cw33300.ExecuteInstruction(opcode);
}

void CXD8530BQ::RaiseException(ExceptionType exceptionType, std::uint32_t data)
{
	bool isBranchDelay = _cw33300.isExecutingDelaySlot();

	//If we're executing a branch delay slot, we have to return to the branch instruction before it
	_cop0.SetRegister(14, isBranchDelay ? (_cw33300._currentInstructionAddress - 4) : _cw33300._currentInstructionAddress);

	std::uint32_t causeReg = _cop0.GetRegister(13);
	causeReg = Math::SetBits(causeReg, 6, 2, std::uint32_t(exceptionType));
	causeReg = Math::ToggleBit(causeReg, 31, isBranchDelay);
	_cop0.SetRegister(13, causeReg);

	std::uint32_t cop0sr = _cop0.GetRegister(12);
	cop0sr = Math::SetBits(cop0sr, 5, 0, cop0sr << 2);
	_cop0.SetRegister(12, cop0sr);

	std::uint32_t handler =
		Math::GetBit(cop0sr, 22)
		? 0xbfc00180
		: 0x80000080;

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
	: _cw33300(this)
	, _cop0(this)
	, _gte(this)
	, _playstation(playstation)
{

}
