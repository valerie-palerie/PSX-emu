#pragma once
#include "PlaystationComponent.h"
#include "CW33300.h"
#include "COP0.h"
#include "GTE.h"
#include "DelayedMemStore.h"

class Playstation;

class CXD8530BQ : public PlaystationComponent
{
private:
	CW33300 _cw33300;
	COP0 _cop0;
	GTE _gte;

	std::vector<std::unique_ptr<BaseDelayedMemStore>> _delayMemStores;

public:
	CW33300* cw33300() { return &_cw33300; }
	COP0* cop0() { return &_cop0; }
	GTE* gte() { return &_gte; }

	Playstation* playstation() { return _playstation; }

	virtual void Init() override;
	virtual void Tick(double deltaT) override;

	void Clock();
	void ProcessDelayStores();
	void CheckPendingInterrupts();

	void ExecuteInstruction(Opcode opcode);
	void RaiseException(ExceptionType exceptionType, uint32 data = 0x0);

	template<typename StoreT, class... Args>
	void QueueDelayMemStore(Args...);

	CXD8530BQ(Playstation* playstation);
};

template<typename StoreT, class ...Args>
inline void CXD8530BQ::QueueDelayMemStore(Args ... args)
{
	_delayMemStores.push_back(std::make_unique<StoreT>(args...));
}
