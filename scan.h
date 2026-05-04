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
		// Виртуальный деструктор. Вроде бы нужен для корректного удаления дочерних классов
		virtual ~Scan() = default;
		// Конструктор. При создании А-скана, например, стол, вообще говоря не нужен, а вот без осциллографа никак
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

		struct BasicData {
			std::vector<std::vector<double>> Volt_ticks;
			std::vector<std::vector<double>> points;
			std::vector<double> load;
			std::string specimenName;
		};
		virtual void manualSetBasePoints() = 0;
		virtual void setPoints() = 0;
		std::vector<std::vector<double>> getBasePoints() { return basePoints; };
		void start();
		virtual void saveRawData(std::shared_ptr<BasicData> data) = 0;
		virtual void cancel() = 0;
		virtual void interrupt() = 0;
		std::vector<double> getMeasure();					//	Получение осредненного сигнала с осциллографа
		std::shared_ptr<movable::MovableDeviceStage> stage_;
		std::shared_ptr<signal::SignalDeviceOscilloscope> oscill_;
		std::vector<std::vector<double>> basePoints;		//				Базовые точки, например стартовая и финишная для В-скана
		std::vector<std::vector<double>> points;			//				Цепочка точек замера, даже в случае С-скана их последовательность	

		
	};

	//		~~~~~~~~~~~~~~~~			Замер напряжения				~~~~~~~~~~~~~~~~~~~~~~
	//	Просто сохранение напряжения на осциллографе в файл 
	class MeasureVoltage : public Scan {
	public:
		MeasureVoltage(std::shared_ptr<signal::SignalDeviceOscilloscope> oscill)
			: Scan(oscill) {
		}
		void manualSetBasePoints() override {};
		void setPoints() override {};
		void start();
		virtual void saveRawData(std::shared_ptr<BasicData> data) override {};
		void cancel() override {};
		void interrupt() override {};
		void getGeneratorSignal(std::vector<double> &signal) {
			signal = MeasuredSignal;
		}
	private:
		std::vector<double> MeasuredSignal;
	};

	//		~~~~~~~~~~~~~~~~			А-СКАН				~~~~~~~~~~~~~~~~~~~~~~
	//	класс замера в одной точке сигнал с виборометра или микрофона, но стол должен быть зафиксирован
	class Ascan : public Scan {
	public:
		// поскольку конструктор базового класса требует аргумент, передать нужно явно
		Ascan(std::shared_ptr<signal::SignalDeviceOscilloscope> oscill, 
			std::shared_ptr<movable::MovableDeviceStage> stage)
			: Scan(oscill, stage) {
		}
		void manualSetBasePoints() override;
		void setPoints() override;
		virtual void saveRawData(std::shared_ptr<BasicData> data) override;
		void cancel() override {};
		void interrupt() override {};
	};

	//		~~~~~~~~~~~~~~~~			В-СКАН				~~~~~~~~~~~~~~~~~~~~~~
	//	Скан на отрезке. 
	class Bscan : public Scan {
	public:
		// поскольку конструктор базового класса требует аргумент, передать нужно явно
		Bscan(std::shared_ptr<signal::SignalDeviceOscilloscope> oscill, 
			std::shared_ptr<movable::MovableDeviceStage> stage)
			: Scan(oscill, stage) {
		}
		void manualSetBasePoints() override;
		void setPoints() override;
		/*void start();*/
		void saveRawData(std::shared_ptr<BasicData> data) override;
		void cancel() override {};
		void interrupt() override {};
		double DIST_STEP = 0;
	};

	//		~~~~~~~~~~~~~~~~			С-СКАН				~~~~~~~~~~~~~~~~~~~~~~
	//	Скан на прямоугольной сетке точек. 
	class Cscan : public Scan {
	public:
		// поскольку конструктор базового класса требует аргумент, передать нужно явно
		Cscan(std::shared_ptr<signal::SignalDeviceOscilloscope> oscill,
			std::shared_ptr<movable::MovableDeviceStage> stage)
			: Scan(oscill, stage) {
		}
		void manualSetBasePoints() override;
		void setPoints() override;
		void start();
		virtual void saveRawData(std::shared_ptr<BasicData> data) override;
		void cancel() override {};
		void interrupt() override {};
	};

	//		~~~~~~~~~~~~~~~~			R-СКАН				~~~~~~~~~~~~~~~~~~~~~~
	//	Скан внутри выпуклого четырехугольника в N рандомных точках было бы здорово сделать до понедельника. 
	class Rscan : public Scan {
	public:
		// поскольку конструктор базового класса требует аргумент, передать нужно явно
		Rscan(std::shared_ptr<signal::SignalDeviceOscilloscope> oscill,
			std::shared_ptr<movable::MovableDeviceStage> stage)
			: Scan(oscill, stage) {
		}
		//1 Выбор 4 базовых точек
		void manualSetBasePoints() override;
		//2 Получение точек 
		void setPoints() override;
		void start();
		virtual void saveRawData(std::shared_ptr<BasicData> data) override {};
		void cancel() override {};
		void interrupt() override {};
	};


	class Oscan : public Scan {
	public:
		Oscan(std::shared_ptr<signal::SignalDeviceOscilloscope> oscill,
			std::shared_ptr<movable::MovableDeviceStage> stage)
			: Scan(oscill, stage) {
		}
		void manualSetBasePoints() override {};
		//2 Получение точек 
		void setPoints() override;
		void start();
		virtual void saveRawData(std::shared_ptr<BasicData> data) override {};
		void cancel() override {};
		void interrupt() override {};
		
	};

	/* В функции старт у всех сканов почти одно и то же, надо универсифицировать, прописать в родительском 
	классе и переписать разве что у замера напряжения, при этом функция созранения матфайла будет одна*/

	/*добавить при сохранении скана в матфайл писать там дату и время создания в названии или в самом файле*/
}