#include "Card.h"
#include "Card/graphics/Window.h"
#include "GLFW/glfw3.h"

class Kaartspel : public Card::Application {
public:
	int currentchosen = 0;
	int lastchosen = -1;

	bool keyA = false;
	bool keyD = false;
	bool keyEnter = false;
	bool playingchosencard = false;
	std::vector<int> playedCards;
	int tick = 0;

	Kaartspel() {
		glfwInit();
	}

	~Kaartspel() {

	}

	void update() override {

		keyboard();
		playCard();
	

	}

	void start() override {

		scenebuilder->setViewDistance(100.0f);
		scenebuilder->rotateCamera(-25.0f,0,0);

		scenebuilder->addModeltoScene("C:/Users/Alissa/source/repos/AlissaDeVegt/Minor/Card/src/Card/models/Models/Table.obj", "C:/Users/Alissa/source/repos/AlissaDeVegt/Minor/Card/src/Card/models/Models/Color ref1.jpg", 0.0f, -2.0f, -3.0f); 
		scenebuilder->addModeltoScene("C:/Users/Alissa/source/repos/AlissaDeVegt/Minor/Card/src/Card/models/Models/models/PLane_006.obj", "C:/Users/Alissa/source/repos/AlissaDeVegt/Minor/Card/src/Card/models/Models/models/cards.png", 0.9f, -0.7f, -1.0f); 
		scenebuilder->addModeltoScene("C:/Users/Alissa/source/repos/AlissaDeVegt/Minor/Card/src/Card/models/Models/models/PLane_009.obj", "C:/Users/Alissa/source/repos/AlissaDeVegt/Minor/Card/src/Card/models/Models/models/cards.png", 0.6f, -0.7f, -1.0f); 
		scenebuilder->addModeltoScene("C:/Users/Alissa/source/repos/AlissaDeVegt/Minor/Card/src/Card/models/Models/models/PLane_001.obj", "C:/Users/Alissa/source/repos/AlissaDeVegt/Minor/Card/src/Card/models/Models/models/cards.png", 0.3f, -0.7f, -1.0f); 
		scenebuilder->addModeltoScene("C:/Users/Alissa/source/repos/AlissaDeVegt/Minor/Card/src/Card/models/Models/models/PLane_002.obj", "C:/Users/Alissa/source/repos/AlissaDeVegt/Minor/Card/src/Card/models/Models/models/cards.png", 0.0f, -0.7f, -1.0f); 
		scenebuilder->addModeltoScene("C:/Users/Alissa/source/repos/AlissaDeVegt/Minor/Card/src/Card/models/Models/models/PLane_003.obj", "C:/Users/Alissa/source/repos/AlissaDeVegt/Minor/Card/src/Card/models/Models/models/cards.png",-0.3f, -0.7f, -1.0f); 
		scenebuilder->addModeltoScene("C:/Users/Alissa/source/repos/AlissaDeVegt/Minor/Card/src/Card/models/Models/models/PLane_007.obj", "C:/Users/Alissa/source/repos/AlissaDeVegt/Minor/Card/src/Card/models/Models/models/cards.png",-0.6f, -0.7f, -1.0f); 
		scenebuilder->addModeltoScene("C:/Users/Alissa/source/repos/AlissaDeVegt/Minor/Card/src/Card/models/Models/models/PLane_008.obj", "C:/Users/Alissa/source/repos/AlissaDeVegt/Minor/Card/src/Card/models/Models/models/cards.png",-0.9f, -0.7f, -1.0f); 

		/*
		scenebuilder->addModeltoScene("C:/dev/Models/Table.obj", "C:/dev/Models/Color ref1.jpg", 0.0f, -2.0f, -3.0f);
		scenebuilder->addModeltoScene("C:/dev/Models/models/PLane_006.obj", "C:/dev/Models/models/cards.png", 0.9f, -0.7f, -1.0f);
		scenebuilder->addModeltoScene("C:/dev/Models/models/PLane_009.obj", "C:/dev/Models/models/cards.png", 0.6f, -0.7f, -1.0f);
		scenebuilder->addModeltoScene("C:/dev/Models/models/PLane_001.obj", "C:/dev/Models/models/cards.png", 0.3f, -0.7f, -1.0f);
		scenebuilder->addModeltoScene("C:/dev/Models/models/PLane_002.obj", "C:/dev/Models/models/cards.png", 0.0f, -0.7f, -1.0f);
		scenebuilder->addModeltoScene("C:/dev/Models/models/PLane_003.obj", "C:/dev/Models/models/cards.png", -0.3f, -0.7f, -1.0f);
		scenebuilder->addModeltoScene("C:/dev/Models/models/PLane_007.obj", "C:/dev/Models/models/cards.png", -0.6f, -0.7f, -1.0f);
		scenebuilder->addModeltoScene("C:/dev/Models/models/PLane_008.obj", "C:/dev/Models/models/cards.png", -0.9f, -0.7f, -1.0f);*/

		scenebuilder->getModel(1)->setSize(4.0f);
		scenebuilder->getModel(2)->setSize(4.0f);
		scenebuilder->getModel(3)->setSize(4.0f);
		scenebuilder->getModel(4)->setSize(4.0f);
		scenebuilder->getModel(5)->setSize(4.0f);
		scenebuilder->getModel(6)->setSize(4.0f);
		scenebuilder->getModel(7)->setSize(4.0f);
								
		scenebuilder->getModel(0)->rotate(90.0f, 0.0f, 0.0f);
		scenebuilder->getModel(1)->rotate(25.0f, 0.0f, 0.0f);
		scenebuilder->getModel(2)->rotate(25.0f, 0.0f, 0.0f);
		scenebuilder->getModel(3)->rotate(25.0f, 0.0f, 0.0f);
		scenebuilder->getModel(4)->rotate(25.0f, 0.0f, 0.0f);
		scenebuilder->getModel(5)->rotate(25.0f, 0.0f, 0.0f);
		scenebuilder->getModel(6)->rotate(25.0f, 0.0f, 0.0f);
		scenebuilder->getModel(7)->rotate(25.0f, 0.0f, 0.0f);
		
		updatecurrentchosen();
	}


	void keyboard() {

		if (glfwGetKey(window->getGlfwWindow(), GLFW_KEY_D) == GLFW_PRESS && keyD == false) {
			keyD = true;
		}
		else if (glfwGetKey(window->getGlfwWindow(), GLFW_KEY_D) == GLFW_RELEASE && keyD == true)
		{
			if (playingchosencard == false) {
				lastchosen = currentchosen;
				currentchosen++;
				updatecurrentchosen();
			}
			keyD = false;
		}


		if (glfwGetKey(window->getGlfwWindow(), GLFW_KEY_A) == GLFW_PRESS && keyA == false) {
			keyA = true;
		}
		else if (glfwGetKey(window->getGlfwWindow(), GLFW_KEY_A) == GLFW_RELEASE && keyA == true)
		{
			if (playingchosencard == false) {
				lastchosen = currentchosen;
				currentchosen--;
				updatecurrentchosen();
			}
			keyA = false;
		}

		if (glfwGetKey(window->getGlfwWindow(), GLFW_KEY_ENTER) == GLFW_PRESS && keyEnter == false) {
			keyEnter = true;
		}
		else if (glfwGetKey(window->getGlfwWindow(), GLFW_KEY_ENTER) == GLFW_RELEASE && keyEnter == true)
		{
			keyEnter = false;

			if (currentchosen < scenebuilder->getModels().size() && currentchosen >= 1) {
				playingchosencard = true;
			}
		}

		if (glfwGetKey(window->getGlfwWindow(), GLFW_KEY_W) == GLFW_PRESS) {
			scenebuilder->moveCamera(0.00f, 0.01f, 0.0f);

		}

		if (glfwGetKey(window->getGlfwWindow(), GLFW_KEY_S) == GLFW_PRESS) {
			scenebuilder->moveCamera(0.0f, -0.01f, 0.0f);
		}

		if (glfwGetKey(window->getGlfwWindow(), GLFW_KEY_Q) == GLFW_PRESS) {
			scenebuilder->moveCamera(0.0f, 0.0f, -0.01f);
		}

		if (glfwGetKey(window->getGlfwWindow(), GLFW_KEY_E) == GLFW_PRESS) {
			scenebuilder->moveCamera(0.0f, 0.0f, 0.01f);
		}

		if (glfwGetKey(window->getGlfwWindow(), GLFW_KEY_UP) == GLFW_PRESS) {
			scenebuilder->rotateCamera(1.f,  0.0f, 0.0f);
		}

		if (glfwGetKey(window->getGlfwWindow(), GLFW_KEY_DOWN) == GLFW_PRESS) {
			scenebuilder->rotateCamera(-1.f, 0.0f, 0.0f);
		}

		if (glfwGetKey(window->getGlfwWindow(), GLFW_KEY_LEFT) == GLFW_PRESS) {
			scenebuilder->rotateCamera(0.0f, 1.f, 0.0f);
		}

		if (glfwGetKey(window->getGlfwWindow(), GLFW_KEY_RIGHT) == GLFW_PRESS) {
			scenebuilder->rotateCamera(0.0f, -1.f, 0.0f);
		}
	}

	void playCard() {
		if (playingchosencard == true)
		{
			for (int i = 0; i < playedCards.size(); i++) {
				if (playedCards[i] == currentchosen) {
					return;
				}
			}

			if (tick >= 90 - 25) {
				playingchosencard = false;
				tick = 0;
				playedCards.push_back(currentchosen);
				lastchosen = currentchosen;
				currentchosen = 0;
				updatecurrentchosen();
				return;
			}

			tick = tick + 1;

			scenebuilder->getModel(currentchosen)->rotate(1.0f, 0.0f, 0.0f);
			scenebuilder->getModel(currentchosen)->move(0.f, 0.03f, -0.03f);

		}
	}

	void updatecurrentchosen() {
		if (currentchosen < scenebuilder->getModels().size() && currentchosen >= 1) {
			scenebuilder->getModel(currentchosen)->move(0.0f,0.1f,0.0f );
		}
		
		if (lastchosen < scenebuilder->getModels().size() && lastchosen >= 1) {
			scenebuilder->getModel(lastchosen)->move(0.0f,-0.1f,0.0f);
		}
		
	}

};

Card::Application* Card::CreateApplication() {
	return new Kaartspel();
}

