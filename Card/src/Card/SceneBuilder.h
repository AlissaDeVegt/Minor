#pragma once
#include "base.h"
#include "graphics/Model.h"
#include "graphics/Camera.h"
#include "Util/ModelLoader.h"

namespace Card {

	class CARD_API SceneBuilder
	{
	public:
		SceneBuilder(Device* device);
		~SceneBuilder();
		Camera* getCamera();
		void setCamera();
		void setCamera(float positionX, float positionY, float positionZ );
		void setCaneraRotation(float lookatX, float lookatY, float lookatZ);
		void setCamera(float positionX, float positionY, float positionZ , float lookatX, float lookatY, float lookatZ);

		void addModeltoScene(std::string MODEL_PATH, std::string TEXTURE_PATH);
		void addModeltoScene(std::string MODEL_PATH, std::string TEXTURE_PATH,float x,float y, float z);
		std::vector<Model*> getModels();
		void removeModelfromScene(int i);
	private:
		Device* device;
		Camera* camera;

		std::vector<Model*>* models = new std::vector<Model*>();
		//lights

	};
}


