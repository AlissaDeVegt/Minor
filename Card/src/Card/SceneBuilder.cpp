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
		camera->setLookAt(glm::vec3{ 0.0f,0.0f,0.0f });
	}

	/// <summary>
	/// move camera without changing the rotation.
	/// can result in change of rotation.
	/// </summary>
	/// <param name="positionX">camera position X</param>
	/// <param name="positionY">camera position Y</param>
	/// <param name="positionZ">camera position Z</param>
	void SceneBuilder::setCamera(float positionX, float positionY, float positionZ)
	{
		camera->setPosition(glm::vec3{ positionX, positionZ, positionY });
	}

	/// <summary>
	/// rotate camera to look at certain location
	/// </summary>
	/// <param name="lookatX">the x point which the camera will look at</param>
	/// <param name="lookatY">the y point which the camera will look at</param>
	/// <param name="lookatZ">the z point which the camera will look at</param>
	void SceneBuilder::setCaneraRotation(float lookatX, float lookatY, float lookatZ)
	{
		camera->setLookAt(glm::vec3{ lookatX, lookatZ, lookatY });

	}

	/// <summary>
	/// set camera with and change lookpoint
	/// </summary>
	/// <param name="positionX">camera position X</param>
	/// <param name="positionY">camera position Y</param>
	/// <param name="positionZ">camera position Z</param>
	/// <param name="lookatX">the x point which the camera will look at</param>
	/// <param name="lookatY">the y point which the camera will look at</param>
	/// <param name="lookatZ">the z point which the camera will look at</param>
	void SceneBuilder::setCamera(float positionX, float positionY, float positionZ, float lookatX, float lookatY, float lookatZ)
	{
		camera->setPosition(glm::vec3{ positionX, positionZ, positionY });
		camera->setLookAt(glm::vec3{ lookatX, lookatZ, lookatY });
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
		models->push_back(ModelLoader::readModelFile(MODEL_PATH, TEXTURE_PATH, device, camera)->moveObject(glm::vec3{x,y,z}));
	}

	/// <summary>
	/// get list of models in scene
	/// </summary>
	/// <returns>a vector list with pointers to the models</returns>
	std::vector<Model*> SceneBuilder::getModels()
	{
		return *models;
	}

	/// <summary>
	/// remove specific model at index i
	/// </summary>
	/// <param name="i">the index of the model that will be removed</param>
	void SceneBuilder::removeModelfromScene(int i)
	{
		models->erase(models->begin()+i);
	}


}