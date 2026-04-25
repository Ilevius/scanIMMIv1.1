#include <vector>
#include "movable.h"
#include "signal/Oscilloscope/SignalDeviceOscilloscope.h"
#include "math.h"
#include "files.h"
#include "signal_processing.h"
#include "settings.h"
//#pragma once
namespace scan {

	class Scan {
	public:
		// ¬иртуальный деструктор. ¬роде бы нужен дл€ корректного удалени€ дочерних классов
		virtual ~Scan() = default;
		//  онструктор. ѕри создании ј-скана, например, стол, вообще говор€ не нужен, а вот без осциллографа никак
		Scan(std::shared_ptr<signal::SignalDeviceOscilloscope> oscill)
			: oscill_(std::move(oscill)) {  // перемещаем ownership
			if (!oscill_) throw std::invalid_argument("Oscill не может быть nullptr");
		}

		Scan(
			std::shared_ptr<signal::SignalDeviceOscilloscope> oscill,
			std::shared_ptr<movable::MovableDeviceStage> stage
			)
			: oscill_(std::move(oscill)), stage_(std::move(stage)) {  // перемещаем ownership
			if (!oscill_) throw std::invalid_argument("Oscill не может быть nullptr");
			if (!stage_) throw std::invalid_argument("Stage не может быть nullptr");
		}


		virtual void setBasePoints() = 0;
		virtual void setPoints() = 0;
		std::vector<std::vector<double>> getBasePoints() { return basePoints; };
		void setPlateCoords();
		virtual void start() = 0;
		virtual void cancel() = 0;
		virtual void interrupt() = 0;
		std::vector<double> getMeasure();					//	ѕолучение осредненного сигнала с осциллографа
		std::shared_ptr<movable::MovableDeviceStage> stage_;
		std::shared_ptr<signal::SignalDeviceOscilloscope> oscill_;
		std::vector<std::vector<double>> basePoints;		//				Ѕазовые точки, например стартова€ и финишна€ дл€ ¬-скана
		std::vector<std::vector<double>> points;			//				÷епочка точек замера, даже в случае —-скана их последовательность	
	};


	class MeasureVoltage : public Scan {
	public:
		MeasureVoltage(std::shared_ptr<signal::SignalDeviceOscilloscope> oscill)
			: Scan(oscill) {
		}
		void setBasePoints() override {};
		void setPoints() override {};
		void start() override;
		void cancel() override {};
		void interrupt() override {};
		void getGeneratorSignal(std::vector<double> &signal) {
			signal = MeasuredSignal;
		}
	private:
		std::vector<double> MeasuredSignal;
	};

	//		~~~~~~~~~~~~~~~~			ј-— јЌ				~~~~~~~~~~~~~~~~~~~~~~
	//	класс замера в одной точке сигнал с виборометра или микрофона, но стол должен быть зафиксирован
	class Ascan : public Scan {
	public:
		// поскольку конструктор базового класса требует аргумент, передать нужно €вно
		Ascan(std::shared_ptr<signal::SignalDeviceOscilloscope> oscill, 
			std::shared_ptr<movable::MovableDeviceStage> stage)
			: Scan(oscill, stage) {
		}
		void setBasePoints() override;
		void setPoints() override;
		// «десь добавить цикл по всем точкам
		void start() override;	
		void cancel() override {};
		void interrupt() override {};
	};

	class Bscan : public Scan {
	public:
		// поскольку конструктор базового класса требует аргумент, передать нужно €вно
		Bscan(std::shared_ptr<signal::SignalDeviceOscilloscope> oscill, 
			std::shared_ptr<movable::MovableDeviceStage> stage)
			: Scan(oscill, stage) {
		}
		void setBasePoints() override;
		void setPoints() override;
		void start() override;
		void cancel() override {};
		void interrupt() override {};
		double DIST_STEP = 0;
	};

	class Cscan : public Scan {
	public:
		// поскольку конструктор базового класса требует аргумент, передать нужно €вно
		Cscan(std::shared_ptr<signal::SignalDeviceOscilloscope> oscill,
			std::shared_ptr<movable::MovableDeviceStage> stage)
			: Scan(oscill, stage) {
		}
		void setBasePoints() override;
		void setPoints() override;
		void start() override;
		void cancel() override {};
		void interrupt() override {};
	};

	//	—кан внутри выпуклого четырехугольника в N рандомных точках было бы здорово сделать до понедельника. 
	class Rscan : public Scan {
	public:
		// поскольку конструктор базового класса требует аргумент, передать нужно €вно
		Rscan(std::shared_ptr<signal::SignalDeviceOscilloscope> oscill,
			std::shared_ptr<movable::MovableDeviceStage> stage)
			: Scan(oscill, stage) {
		}
		//1 ¬ыбор 4 базовых точек
		void setBasePoints() override;
		//2 ѕолучение точек 
		void setPoints() override;
		void start() override;
		void cancel() override {};
		void interrupt() override {};
	};

	/*добавить при сохранении скана в матфайл писать там дату и врем€ создани€ в названии или в самом файле*/
}