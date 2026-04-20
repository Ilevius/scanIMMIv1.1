namespace signal {

	class SignalDevice {
		/* тут описываем то, что должно быть у любого оборудовани€ дающего сигнал */
	public:
		virtual ~SignalDevice() = default;  // ¬иртуальный деструктор
		virtual void connect() = 0;
		virtual void disconnect() = 0;
		virtual void setup() = 0;
		bool is_connected() { return connection; }

	protected:
		bool connection = false;
	};

};