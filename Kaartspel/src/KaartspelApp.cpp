#include "Card.h"

class Kaartspel : public Card::Application {
public:
	Kaartspel() {

	}

	~Kaartspel() {

	}

};

Card::Application* Card::CreateApplication() {
	return new Kaartspel();
}

