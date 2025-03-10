#include "SceneBuilder.h"
#include "graphics/Device.h"

namespace Card {
	SceneBuilder::SceneBuilder(Device* device)
	{
		this->device = device;

		this->camera = camera = new Camera(device);
		setCamera();
	}

	SceneBuilder::~SceneBuilder()
	{

	}

#pragma region ------------------------------------Camera--------------------------------------------------

	Camera* SceneBuilder::getCamera()
	{
		return camera;
	}

	/// <summary>
	/// setcamera to base location
	/// </summary>
	void SceneBuilder::setCamera()
	{
		camera->setPosition(glm::vec3{ 0.0f,0.0f,0.0f });
	}

	/// <summary>
	/// set the position of the camera
	/// </summary>
	/// <param name="X">X axis</param>
	/// <param name="Y">Y axis</param>
	/// <param name="Z">z axis</param>
	void SceneBuilder::setCamera(float X, float Y, float Z)
	{
		camera->setPosition(glm::vec3{ X, Z, Y });
	}

	/// <summary>
	/// set view distance of the camera
	/// </summary>
	/// <param name="distance">view distance</param>
	void SceneBuilder::setViewDistance(float distance)
	{
		camera->setViewDistance(distance);
	}

	/// <summary>
	/// move the camera
	/// </summary>
	/// <param name="X">X axis</param>
	/// <param name="Y">Y axis</param>
	/// <param name="Z">z axis</param>
	void SceneBuilder::moveCamera(float X, float Y, float Z)
	{
		camera->move(X, Z, Y);
	}

	void SceneBuilder::rotateCamera(float Xdegrees, float Ydegrees, float Zdegrees)
	{
		camera->rotate(glm::vec3{ Xdegrees,  Zdegrees,  Ydegrees });
	}

	void SceneBuilder::resetCanera()
	{
		camera->reset();
	}


#pragma endregion

#pragma region ------------------------------------Models---------------------------------------------------



	void SceneBuilder::updateModels()
	{
		for (int i = 0; i < models->size(); i++) {
			models->at(i)->updateVertexBuffer();
		}

	}

	/// <summary>
	/// add a model to scene.
	/// </summary>
	/// <param name="MODEL_PATH">path to the model obj file</param>
	/// <param name="TEXTURE_PATH">path to the model texture file</param>
	void SceneBuilder::addModeltoScene(std::string MODEL_PATH, std::string TEXTURE_PATH)
	{
		models->push_back(ModelLoader::readModelFile(MODEL_PATH, TEXTURE_PATH, device, camera));
	}


	/// <summary>
	/// add model to scene with a starting position
	/// </summary>
	/// <param name="MODEL_PATH">path to the model obj file</param>
	/// <param name="TEXTURE_PATH">path to the model texture file</param>
	/// <param name="x">starting position x</param>
	/// <param name="y">starting position y</param>
	/// <param name="z">starting position z</param>
	void SceneBuilder::addModeltoScene(std::string MODEL_PATH, std::string TEXTURE_PATH, float x, float y, float z)
	{
		models->push_back(ModelLoader::readModelFile(MODEL_PATH, TEXTURE_PATH, device, camera)->moveObject(glm::vec3{x,z,y}));
	}

	/// <summary>
	/// get list of models in scene
	/// </summary>
	/// <returns>a vector list with pointers to the models</returns>
	std::vector<Model*> SceneBuilder::getModels()
	{
		return *models;
	}

	Model* SceneBuilder::getModel(int i)
	{
		return models->at(i);
	}

	/// <summary>
	/// remove specific model at index i
	/// </summary>
	/// <param name="i">the index of the model that will be removed</param>
	void SceneBuilder::removeModelfromScene(int i)
	{
		models->erase(models->begin()+i);
	}

#pragma endregion
}