#include "Card.h"

class Sandbox : public Card::Application {
public:
	Sandbox() {

	}

	~Sandbox() {

	}

};

Card::Application* Card::CreateApplication() {
	return new Sandbox();
}