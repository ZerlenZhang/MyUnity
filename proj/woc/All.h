#pragma once


#pragma region ‘§¥¶¿Ì
#define GLEW_STATIC
#define var auto
#include <iostream>
#include <stdio.h>

#include<fstream>
using std::ofstream;
#include<sstream>
#include<string>
using std::string;
#include<vector>
using std::vector;
#include<list>
using std::list;

using std::cout;
using std::endl;


#include "../../3rd/glm/glm.hpp"
#include "../../3rd/glm/gtc/matrix_transform.hpp"
#include "../../3rd/glm/gtx/rotate_vector.hpp"
#include "../../3rd/glm/gtc/type_ptr.hpp"
using namespace glm;
using mat4 = glm::mat4;
using mat3 = glm::mat3;
using vec2 = glm::vec2;
using vec3 = glm::vec3;
using vec4 = glm::vec4;


#include<GL/glew.h>
#include <GLFW/glfw3.h>


#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_glfw.h"
#include "ImGui/imgui_impl_opengl3.h"

#include<assimp/Importer.hpp>
#include<assimp/scene.h>
#include<assimp/postprocess.h>

#include"json.hpp"
using json = nlohmann::json;

#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif






#pragma endregion


#pragma region extern
extern string workDir;
extern mat4 viewMat;
extern mat4 projMat;
extern GLFWwindow* window;
extern vec2* pWindowSize;


extern unsigned int loadCubemap(vector<std::string> faces);
extern unsigned int TextureFromFile(const char *path, const string &directory, bool gamma = false);
extern unsigned int TextureFromFile(string filename);

#pragma endregion


#pragma region Delcare
class Input;
class Object;
class MonoBehavior;
class Rotate;
class GameObject;
class Transform;
class CameraMove;

class AbstractLight;
class LightDirectional;
class LightPoint;
struct Vertex;
struct Texture;
class Mesh;
class Camera;
class Model;

class Shader;
class AbstractMaterial;
class StandandMaterial;
class BoxMaterial;
class ModelRender;
class SkyboxRender;
class GameObject;
class Setting;

#pragma endregion


#pragma region input
#define KEYS 100
#define W_ 0
#define A_ 1
#define S_ 2
#define D_ 3
#define Q_ 4
#define E_ 5
#define Shift_ 6
#define Space_ 7

static class Input
{
public:
	static bool* lastKey;
	static bool* Key;
	static bool* keyDown;
	static bool* keyUp;
	static glm::vec2 mouseMentDelta;
	static bool GetKeyDown(int key);
	static bool GetKey(int key);
	static bool GetKeyUp(int key);
	static void GetInput();
	static void ClearInputEveryFrame();
	static void InitInput();
};

#pragma endregion


#pragma region Object

class Object
{
public:
	string name;
	Object(string name) :name(name) {}
	virtual void OnGUI()const = 0;
	virtual ~Object() {}
};

#pragma endregion

#pragma region MonoBehavior : Object

class MonoBehavior :public Object
{
public:
	bool enable = true;
	GameObject* gameObject;

	MonoBehavior();
	virtual ~MonoBehavior()override;

	virtual void Start();
	virtual void RealUpdate();
	virtual void OnGUI()const override;
	virtual void Update();
};


#pragma endregion

#pragma region Rotate : MonoBehavior

class Rotate :public MonoBehavior
{
public:
	float rotateSpeed;
	void OnGUI()const override;
	void Update()override;
	Rotate();
	virtual ~Rotate()override;
};

#pragma endregion

#pragma region Transform : MonoBehavior

class Transform :public MonoBehavior
{
public:
	vec3 position = vec3(0, 0, 0);
	vec3 rotation = vec3(0, 0, 0);
	vec3 scale = vec3(1, 1, 1);

	vec3 Forward;
	vec3 Right;
	vec3 Up;

	vec3 WorldUp = vec3(0, 1.0f, 0);

	float Pitch = .0f;
	float Yaw = .0f;

	void Translate(vec3 movement);
	mat4 GetModelMaterix(mat4 world = mat4(1.0f))const;


	void OnGUI()const override;
	void RealUpdate()override;

	Transform(vec3 pos = vec3(0, 0, 0), vec3 rotation = vec3(0, 0, 0), vec3 scanle = vec3(1, 1, 1));
	virtual ~Transform()override;
};
#pragma endregion

#pragma region Camera : MonoBehavior

class Camera :public MonoBehavior
{
public:
	Camera();
	virtual ~Camera()override;


	vec4 viewPort;
	float angle = 60;
	float near = 0.1f;
	float far = 500.0f;

	void Start()override;
	void RealUpdate()override;
	void OnGUI()const override;
private:
	Transform* transform;
};
#pragma endregion

#pragma region CameraMove : MonoBehavior
class CameraMove :
	public MonoBehavior
{
private:
	float currentSpeed = 1.0f;
	Transform* transform;
public:
	float sencity = 0.006f;
	float normalSpeed = 5.0f;
	float highSpeed = 15.0f;

	void Start()override;
	void Update()override;
	void RealUpdate()override;
	void OnGUI()const override;
	CameraMove();
	virtual ~CameraMove()override;
};
#pragma endregion

#pragma region AbstractLight £∫MonoBehavior

class AbstractLight :public MonoBehavior
{

protected:
	Transform* transform;
	void UpdateDirection();
	virtual string Sign()const = 0;
	vec3 direction;
	vec3 color;
	void virtual FromJson(const json& j);
	void virtual ToJson(json& j)const;
public:
	enum class LightType
	{
		Directional,
		Spot,
		Point
	};

	float strength = 1.0f;
	virtual LightType Type()const = 0;

	AbstractLight();
	virtual ~AbstractLight()override;

	void OnGUI()const override;
	void Start()override;
	void Update()override;

	virtual void SetShader(Shader* shader, int index);
	friend void to_json(json& j, const AbstractLight& l);
	friend void from_json(const json& j, AbstractLight& l);
};
#pragma endregion

#pragma region DirectionalLight : AbstractLight
class LightDirectional :public AbstractLight
{
public:

	string Sign()const override { return "lightDirectional"; }

	LightDirectional();
	virtual ~LightDirectional()override;

	void Start()override;


	LightType Type()const override { return LightType::Directional; }

	friend void to_json(json& j, const LightDirectional& l);
	friend void from_json(const json& j, LightDirectional& l);
};
#pragma endregion

#pragma region PointLight : AbstractLight

class LightPoint :public AbstractLight
{
public:
	float constant = 1.0f;
	float linear = 5.0f;
	float quadratic = 0.32f;

	LightPoint();
	virtual ~LightPoint()override;

	string Sign()const override { return "lightPoint"; }
	void SetShader(Shader* shader, int index)override;

	void Start()override;
	void OnGUI()const override;

	void FromJson(const json& j)override;
	void ToJson(json& j)const override;

	LightType Type()const override { return LightType::Point; }

	//friend void to_json(json& j, const LightPoint& l);
	//friend void from_json(const json& j, LightPoint& l);
};
#pragma endregion

#pragma region SpotLight : PointLight

class LightSpot :public LightPoint
{
public:
	float cosPhyInner = 0.9f;
	float cosPhyOuter = 0.85f;

	void Start()override;
	void OnGUI()const override;

	LightSpot();
	virtual ~LightSpot()override;

	void FromJson(const json& j)override;
	void ToJson(json& j)const override;

	string Sign()const override { return "lightSpot"; }
	LightType Type()const override { return LightType::Spot; }

	void SetShader(Shader* shader, int index)override;
	//friend void to_json(json& j, const LightSpot& l);
	//friend void from_json(const json& j, LightSpot& l);
};
#pragma endregion

#pragma region Shader : Object
class Shader :public Object
{
public:
	Shader(string sign, const char* geometryPath = nullptr);


	unsigned int ID;//shader program ID
	const int maxImage = 4;


	void use();
	void AddLight(AbstractLight* light);
	void OnGUI()const override;


	void setBool(const std::string & name, bool value) const;
	void setInt(const std::string & name, int value) const;
	void setFloat(const std::string & name, float value) const;
	void setVec2(const std::string & name, const glm::vec2 & value) const;
	void setVec2(const std::string & name, float x, float y) const;
	void setVec3(const std::string & name, const glm::vec3 & value) const;
	void setVec3(const std::string & name, float x, float y, float z) const;
	void setVec4(const std::string & name, const glm::vec4 & value) const;
	void setVec4(const std::string & name, float x, float y, float z, float w);
	void setMat2(const std::string & name, const glm::mat2 & mat) const;
	void setMat3(const std::string & name, const glm::mat3 & mat) const;
	void setMat4(const std::string & name, const glm::mat4 & mat) const;

private:
	int currentImages = 0;
	std::string vertexString;
	std::string fragmentString;
	const char* vertexSource;
	const char* fragmentSource;
	void checkCompileErrors(GLuint shader, std::string type);
};
#pragma endregion

#pragma region Mesh
struct Vertex
{
	glm::vec3 position;
	// normal
	glm::vec3 normal;
	// texCoords
	glm::vec2 texCoord;
	// tangent
	glm::vec3 tangent;
	// bitangent
	glm::vec3 bitangent;
};

struct Texture
{
	unsigned int id;
	string type;
	string path;
};


class Mesh
{
public:
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;
	void Draw(Shader* shader);


	Mesh(float vertices[]);
	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);
	Mesh();
	~Mesh();
private:
	unsigned int vao, vbo, ebo;
	void SetUpMesh();
};

#pragma endregion

#pragma region Model : Object

class Model :public Object
{
private:
	void LoadModel(string path);
	void ProcessNode(aiNode* node, const aiScene* scene);
	Mesh processAiMesh(aiMesh* aiMesh, const aiScene* aiscene);
	std::vector<Texture> loadMaterialTextures(aiMaterial * mat, aiTextureType type, string typeName);
public:
	string Directory;
	std::vector<Mesh> meshes;
	std::vector<Texture> textures_loaded;
	void OnGUI()const override;
	void Draw(Shader * shader);
	Model(string path);
	~Model();
};
#pragma endregion



#pragma region AbstractMaterial : Object

class AbstractMaterial :
	public Object
{
public:
	Shader* shader;
	float shininess;
	vec3 color;
	bool specular;
	void OnGUI()const override;
	AbstractMaterial(Shader* shader, vec3 color = vec3(1, 1, 1), float skininess = 1.0f);
	~AbstractMaterial();

	virtual void Use(mat4& view, mat4& proj, mat4 model = mat4(1.0f));
};
#pragma endregion

#pragma region StandardMaterial : AbstractMaterial

class StandandMaterial :public AbstractMaterial
{
public:
	StandandMaterial(Shader* shader, vec3 color = vec3(1, 1, 1), float skininess = 1.0f);
	~StandandMaterial();

	void Use(mat4& view, mat4& proj, mat4 model = mat4(1.0f))override;
};

#pragma endregion

#pragma region BoxMaterial : AbstractMaterial

class BoxMaterial :public AbstractMaterial
{
public:
	string specularPath;
	string diffusePath;
	BoxMaterial(Shader* shader, string specularPath, string diffusePath, vec3 color = vec3(1, 1, 1), float skininess = 32.0f);
	void OnGUI()const override;
	void Use(mat4& view, mat4& proj, mat4 model = mat4(1.0f))override;
	~BoxMaterial();
};
#pragma endregion

#pragma region ModelRender : MonoBehavior
class ModelRender :public MonoBehavior
{
public:
	string shaderName = "standrand";
	string modelName = "model\\nanosuit.obj";
	StandandMaterial* material;
	Model* model;
	void OnGUI()const override;
	void RealUpdate()override;

	void Start()override;
	ModelRender();
	~ModelRender();
};
#pragma endregion

#pragma region SkyboxRender : MonoBehavior
class SkyboxRender :
	public MonoBehavior
{
public:
	StandandMaterial* material;
	unsigned int  textureId;
	unsigned int vao;
	void Start()override;
	void OnGUI()const override;
	void RealUpdate()override;

	SkyboxRender();
	~SkyboxRender();
};
#pragma endregion

#pragma region GameObject : Object
class GameObject :public Object
{

public:
	int id;
	enum Type
	{
		Empty,
		Cameras,
		Directional,
		Spot,
		Point,
		Box,
		Model
	};
	GameObject* Parent;
	std::list<MonoBehavior*>* scripts;

	Transform* transform()const;

	bool enable = true;

	void OnGUI()const override;

	template<typename T>
	T* AddComponentStart();
	template<typename T>
	T * AddComponentNoStart();
	template<typename T>
	T* AddComponentStart(MonoBehavior* component);
	template<typename T>
	T* GetComponent()const;
	template<typename T>
	void RemoveComponent();


	


	GameObject(string name, Type type);
	GameObject(string name, string modelName, string shaderSign);
	~GameObject();
};
#pragma endregion

#pragma region Setting 
static class Setting
{
private:

	const static string settingDir;


public:
	static Camera* MainCamera;
	static std::vector<AbstractLight*>* lights;
	static std::list<GameObject*>* gameObjects;
	static vec2 windowSize;
	static float deltaTime;



	static bool lockMouse;

	static int LightCount(AbstractLight::LightType type);


	static void InitSettings();


};
#pragma endregion

#pragma region json

namespace nlohmann
{
	template<>
	struct adl_serializer<glm::vec3>
	{
		static void to_json(json& j, const glm::vec3 & v)
		{
			j = json{
				{"x",v.x},
			{"y",v.y},
			{"z",v.z}
			};
		}
		static void from_json(const json& j, glm::vec3 & v)
		{
			v = glm::vec3(
				j.at("x").get<float>(),
				j.at("y").get<float>(),
				j.at("z").get<float>()
			);
		}
	};
}
#pragma endregion

