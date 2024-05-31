#include "Card.h"

class Kaartspel : public Card::Application {
public:
	Kaartspel() {
		
	}

	~Kaartspel() {

	}

	void update() override {
		
	}

	void start() override {

	}
};

Card::Application* Card::CreateApplication() {
	return new Kaartspel();
}

