#include "Card.h"

class Kaartspel : public Card::Application {
public:
	Kaartspel() {

	}

	~Kaartspel() {

	}

	void Update() override {
		
	}

};

Card::Application* Card::CreateApplication() {
	return new Kaartspel();
}

