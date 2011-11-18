
#include <xpcc/architecture.hpp>

using namespace xpcc::stm32;

GPIO__OUTPUT(LedStat, C, 12);	// inverted, 0=on, 1=off

extern "C" void
TIM3_IRQHandler(void)
{
	Timer3::acknowledgeInterrupt(Timer3::UPDATE_INTERRUPT);
	
	LedStat::toggle();
}

MAIN_FUNCTION
{
	LedStat::setOutput(xpcc::gpio::HIGH);
	
	Timer3::enable();
	Timer3::configure();
	
	// enable Update Interrupt
	Timer3::enableInterrupt(Timer3::UPDATE_INTERRUPT);
	Timer3::setPeriod(200000);	// 200 ms
	
	// enable timer
	Timer3::resume();

	while (1)
	{
	}
}
