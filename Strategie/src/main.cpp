#include "Card.h"

class Strategie : public Card:: Application{
	public: 
	Strategie() {

	}

	~Strategie() {

	}
};


Card::Application* Card::CreateApplication() {
	return new Strategie();
}