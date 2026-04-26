#include "movable.h"
#include "signal/Oscilloscope/SignalDeviceOscilloscope.h"
#include <vector>



namespace app {

	struct State {
	public:
		// ”стройства
		std::shared_ptr<movable::MovableDeviceStage> stage;
		std::shared_ptr<signal::SignalDeviceOscilloscope> osc;
		// «десь будет сохран€тьс€ сигнал записанный с генератора, чтобы добавить его в ¬-скан и было пон€тно как тот получен
		std::vector<double> genSignal;
	};

	void equipMenu(State& AppState);

	void MeasureVoltage(State& AppState);

	void Ascan(State AppState);

	void Bscan(State AppState);

	void Cscan(State AppState);

	void Rscan(State AppState);

	void Oscan(State AppState);
}
