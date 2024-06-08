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

		scenebuilder->setCamera(0.0f, 5.0f, 0.0f, 0.0f, 0.0f,0.0f);
		scenebuilder->addModeltoScene("C:/dev/Minor/Card/src/Card/models/vikingroom.obj", "C:/dev/Minor/Card/src/Card/textures/viking_room.png");
		scenebuilder->addModeltoScene("C:/dev/Minor/Card/src/Card/models/vikingroom.obj", "C:/dev/Minor/Card/src/Card/textures/viking_room.png",2.0f,0.0f,0.0f);
		scenebuilder->addModeltoScene("C:/dev/Minor/Card/src/Card/models/vikingroom.obj", "C:/dev/Minor/Card/src/Card/textures/viking_room.png",-2.0f,0.0f,0.0f);

	}
};

Card::Application* Card::CreateApplication() {
	return new Kaartspel();
}

