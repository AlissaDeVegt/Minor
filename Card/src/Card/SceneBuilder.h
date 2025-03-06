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
		void setCamera(float X, float Y, float Z );
		void setViewDistance(float distance);
		void moveCamera(float X, float Y, float Z);
		void rotateCamera(float Xdegrees, float Ydegrees, float Zdegrees);
		void resetCanera();

		void updateModels();
		void addModeltoScene(std::string MODEL_PATH, std::string TEXTURE_PATH);
		void addModeltoScene(std::string MODEL_PATH, std::string TEXTURE_PATH,float x,float y, float z);
		std::vector<Model*> getModels();
		Model* getModel(int i);
		void removeModelfromScene(int i);

	private:
		Device* device;
		Camera* camera;

		std::vector<Model*>* models = new std::vector<Model*>();
		//lights

	};
}


