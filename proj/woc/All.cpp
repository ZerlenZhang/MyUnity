#include"All.h"

#pragma region Input
bool* Input::keyDown = new bool[KEYS];
bool* Input::Key = new bool[KEYS];
bool* Input::lastKey = new bool[KEYS];
bool* Input::keyUp = new bool[KEYS];

glm::vec2 Input::mouseMentDelta = glm::vec2(0, 0);

bool Input::GetKey(int key)
{
	return Key[key];
}

bool Input::GetKeyUp(int key)
{
	return keyUp[key];
}

bool Input::GetKeyDown(int key)
{
	return keyDown[key];
}

void Input::GetInput()
{
	for (int i = 0; i < KEYS; i++)
	{
		if (Key[i] && !lastKey[i])
			keyDown[i] = true;
		if (!Key[i] && lastKey[i])
			keyUp[i] = true;
	}
}



void Input::ClearInputEveryFrame()
{
	memcpy(lastKey, Key, sizeof(bool)*KEYS);
	memset(Key, 0, sizeof(bool)*KEYS);
	memset(keyDown, 0, sizeof(bool)*KEYS);
	memset(keyUp, 0, sizeof(bool)*KEYS);
	mouseMentDelta = vec2(0, 0);
}

void Input::InitInput()
{
	memset(Key, 0, sizeof(bool)*KEYS);
	memset(keyDown, 0, sizeof(bool)*KEYS);
	memset(keyUp, 0, sizeof(bool)*KEYS);
	memset(lastKey, 0, sizeof(bool)*KEYS);
}

#pragma endregion

#pragma region Monobehavior ： Object

MonoBehavior::MonoBehavior() :Object("MonoBehavior_")
{
	this->gameObject = nullptr;
}


MonoBehavior::~MonoBehavior()
{
}

void MonoBehavior::Start()
{
}

void MonoBehavior::RealUpdate()
{
}

void MonoBehavior::Update()
{
}

void MonoBehavior::OnGUI() const
{
	ImGui::Checkbox("Enable", (bool*)&enable);
}

#pragma endregion

#pragma region Transform : MonoBehavior


void Transform::Translate(vec3 movement)
{
	position += movement;
}

mat4 Transform::GetModelMaterix(mat4 world) const
{
	//std::cout << scanle.x<<scanle.y<<scanle.z << std::endl;

	auto model = translate(world, position);
	model = glm::scale(model, scale);
	model = rotate(model, radians(rotation.x), vec3(1, 0, 0));
	model = rotate(model, radians(rotation.y), vec3(0, 1, 0));
	model = rotate(model, radians(rotation.z), vec3(0, 0, 1));
	return model;
}

void Transform::OnGUI() const
{
	MonoBehavior::OnGUI();
	ImGui::DragFloat3((gameObject->name + "_position").c_str(), (float*)&position, 0.5f, -500, 500);
	ImGui::DragFloat3((gameObject->name + "_rotation").c_str(), (float*)&rotation, 0.5f, -500, 500);
	ImGui::DragFloat3((gameObject->name + "_scale").c_str(), (float*)&scale, 0.01f, -100, 100);
	ImGui::DragFloat((gameObject->name + "_Yaw").c_str(), (float*)&Yaw, 0.01f, -10, 10);
	ImGui::DragFloat((gameObject->name + "_Pitch").c_str(), (float*)&Pitch, 0.01f, -10, 10);

}

void Transform::RealUpdate()
{
	MonoBehavior::RealUpdate();
	Forward.x = cos(Pitch)*sin(Yaw);
	Forward.y = sin(Pitch);
	Forward.z = cos(Pitch)*cos(Yaw);

	Right = normalize(cross(Forward, WorldUp));
	Up = normalize(cross(Forward, Right));
}

Transform::Transform(vec3 pos, vec3 rotation, vec3 scanle) :
	position(pos),
	rotation(rotation),
	scale(scanle)
{
	name += "Transform";
	Forward.x = cos(Pitch)*sin(Yaw);
	Forward.y = sin(Pitch);
	Forward.z = cos(Pitch)*cos(Yaw);

	Right = normalize(cross(Forward, WorldUp));
	Up = normalize(cross(Right, Forward));
}


Transform::~Transform()
{
}
#pragma endregion

#pragma region Camera : MonoBehavior
Camera::Camera()
{
	name += "Camera";
}


Camera::~Camera()
{
}

void Camera::Start()
{
	MonoBehavior::Start();
	//如果是相机，设置一下初始方向
	transform = gameObject->transform();
	transform->Pitch = radians(15.0f);
	transform->Yaw = radians(180.0f);
	viewPort = vec4(0, 0, pWindowSize->x, pWindowSize->y);
}

void Camera::RealUpdate()
{
	MonoBehavior::RealUpdate();
	//视角矩阵更新
	glViewport(viewPort.x, viewPort.y, viewPort.z, viewPort.w);
	viewMat = lookAt(transform->position, transform->position + transform->Forward, transform->WorldUp);
	projMat = perspective(radians(this->angle), viewPort.z / viewPort.w, near, far);
}

void Camera::OnGUI() const
{
	MonoBehavior::OnGUI();
	ImGui::DragFloat4(("viewPort" + std::to_string(gameObject->id)).c_str(), (float*)&viewPort, 10, 0, 2000);
	ImGui::DragFloat(("viewAngle" + std::to_string(gameObject->id)).c_str(), (float*)&angle, 3, 0, 180.0f);
	ImGui::DragFloat(("near" + std::to_string(gameObject->id)).c_str(), (float*)&near, 0.01f, 0, 10);
	ImGui::DragFloat(("far" + std::to_string(gameObject->id)).c_str(), (float*)&far, 1.0f, 0, 1000);
}
#pragma endregion

#pragma region CameraMove : MonoBehvaior
void CameraMove::Start()
{
	MonoBehavior::Start();
	transform = gameObject->transform();
}
void CameraMove::Update()
{	
	MonoBehavior::Update();
	//这里实现主相机的移动控制
	if (Input::GetKeyDown(Shift_))
		currentSpeed = highSpeed;
	if (Input::GetKeyUp(Shift_))
		currentSpeed = normalSpeed;

	if (Input::GetKey(S_))
		transform->Translate(-currentSpeed * transform->Forward*Setting::deltaTime);
	if (Input::GetKey(W_))
		transform->Translate(currentSpeed * transform->Forward*Setting::deltaTime);
	if (Input::GetKey(A_))
		transform->Translate(-currentSpeed * transform->Right*Setting::deltaTime);
	if (Input::GetKey(D_))	
		transform->Translate(currentSpeed*transform->Right*Setting::deltaTime);
	if (Input::GetKey(Q_))
		transform->Translate(-currentSpeed * transform->Up*Setting::deltaTime);
	if (Input::GetKey(E_))
		transform->Translate(currentSpeed*transform->Up*Setting::deltaTime);

	//锁定和解锁鼠标
	if (Input::GetKeyDown(Space_))
	{
		Setting::lockMouse = !Setting::lockMouse;
		if (Setting::lockMouse)
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		else
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}





}

void CameraMove::RealUpdate()
{
	MonoBehavior::RealUpdate();
	if (!Setting::lockMouse)
		return;
	//根据鼠标更新transform方向
	transform->Pitch -= Input::mouseMentDelta.y * sencity;
	transform->Yaw -= Input::mouseMentDelta.x * sencity;
}

void CameraMove::OnGUI() const
{
	MonoBehavior::OnGUI();
	ImGui::DragFloat("sencity", (float*)&sencity, 0.001f, 0, 0.010f);
	ImGui::DragFloat("normalSpeed", (float*)&normalSpeed, 1, 1, 20);
	ImGui::DragFloat("highSpeed", (float*)&highSpeed, 1, 5, 50);
}

CameraMove::CameraMove()
{
	name += "CameraMove";
}


CameraMove::~CameraMove()
{
}

#pragma endregion

#pragma region Rotate : MonoBehavior


void Rotate::OnGUI() const
{
	MonoBehavior::OnGUI();
	ImGui::DragFloat("rotateSpeed", (float*)&rotateSpeed,0.05f);
}

void Rotate::Update()
{
	MonoBehavior::Update();
	gameObject->transform()->rotation += vec3(0, rotateSpeed, 0);
}

Rotate::Rotate()
{
	name += "Rotate";
}

Rotate::~Rotate()
{
}
#pragma endregion

#pragma region AbstractLight ：MonoBehavior
void AbstractLight::UpdateDirection()
{

	direction = vec3(0, 0, 1.0f);
	direction = rotateZ(direction, transform->rotation.z);
	direction = rotateX(direction, transform->rotation.x);
	direction = rotateY(direction, transform->rotation.y);
	direction *= -1;
}


void AbstractLight::ToJson(json & j)const
{
	j = json{
	{"direction",direction},
	{"color",color},
	{"strength",strength}
	};
}

void AbstractLight::FromJson(const json & j)
{
	color = j.at("color").get<vec3>();
	direction = j.at("direction").get<vec3>();
	strength = j.at("strength").get<float>();
}

AbstractLight::AbstractLight() :
	color(vec3(1, 1, 1))
{
	Setting::lights->push_back(this);
}

AbstractLight::~AbstractLight()
{
	MonoBehavior::~MonoBehavior();
}

void AbstractLight::OnGUI() const
{
	MonoBehavior::OnGUI();
	ImGui::SliderFloat("DirStrength", (float*)&strength, 0.0f, 10.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
	ImGui::DragFloat3("DirAngles", (float*)&transform->rotation, 0.1f, 0, 2 * 3.1415926535f);
	ImGui::ColorEdit3("LightColor", (float*)&color); // Edit 3 floats representing a color
}

void AbstractLight::Start()
{
	MonoBehavior::Start();
	transform = gameObject->transform();
}

void AbstractLight::Update()
{
	MonoBehavior::Update();
	UpdateDirection();
}

void AbstractLight::SetShader(Shader * shader, int index)
{
	//std::cout << Sign() + "[" + std::to_string(index) + "]." + "color" <<this->color.x<<" "<<color.y<<" "<<color.z<< std::endl;
	shader->setBool(Sign() + "[" + std::to_string(index) + "]." + "flag", true);
	shader->setVec3(Sign() + "[" + std::to_string(index) + "]." + "color", this->color*strength);
	shader->setVec3(Sign() + "[" + std::to_string(index) + "]." + "pos", transform->position);
	shader->setVec3(Sign() + "[" + std::to_string(index) + "]." + "dirToLight", this->direction);
}

void to_json(json & j, const AbstractLight & l)
{
	l.ToJson(j);
}

void from_json(const json & j, AbstractLight & l)
{
	l.FromJson(j);
}


#pragma endregion

#pragma region DirectionalLight : AbstractLight
LightDirectional::LightDirectional()
{
	name += "DirectionalLight";
}

LightDirectional::~LightDirectional()
{
}

void LightDirectional::Start()
{
	AbstractLight::Start();
	//设置
	transform->position = vec3(1, 1, -1);
	transform->rotation = vec3(radians(45.0f), radians(90.0f), radians(0.0f));
	color = vec3(1, 1, 1);
}

void to_json(json & j, const LightDirectional & l)
{
	j = json{
	{"direction",l.direction},
	{"color",l.color},
	{"strength",l.strength}
	};
}

void from_json(const json & j, LightDirectional & l)
{
	l.color = j.at("color").get<vec3>();
	l.direction = j.at("direction").get<vec3>();
	l.strength = j.at("strength").get<float>();

}


#pragma endregion

#pragma region PointLight : AbstractLight
LightPoint::LightPoint()
	: AbstractLight()
{
	name += "PointLight";
}


LightPoint::~LightPoint()
{
}

void LightPoint::SetShader(Shader * shader, int index)
{
	AbstractLight::SetShader(shader, index);
	//std::cout << Sign() + "[" + std::to_string(index) + "]." + "constant" << std::endl;
	shader->setFloat(Sign() + "[" + std::to_string(index) + "]." + "constant", this->constant);
	shader->setFloat(Sign() + "[" + std::to_string(index) + "]." + "linear", this->linear);
	shader->setFloat(Sign() + "[" + std::to_string(index) + "]." + "quadratic", this->quadratic);
}
void LightPoint::Start()
{
	AbstractLight::Start();
	transform->position = vec3(0, 0, 10);
	color = vec3(5, 5, 5);
}
void LightPoint::OnGUI() const
{
	AbstractLight::OnGUI();
	ImGui::DragFloat("constant", (float*)&constant, 0.01f, 0, 2);
	ImGui::DragFloat("linear", (float*)&linear, 0.1f, 0, 10);
	ImGui::DragFloat("quadratic", (float*)&quadratic, 0.01f, 0, 0.5f);

}
void LightPoint::FromJson(const json & j)
{

	AbstractLight::FromJson(j);
	constant = j.at("constant").get<float>();
	linear = j.at("linear").get<float>();
	quadratic = j.at("quadratic").get<float>();
}
void LightPoint::ToJson(json & j) const
{
	AbstractLight::ToJson(j);
	j.push_back({ "constant",constant });
	j.push_back({ "linear" ,linear });
	j.push_back({ "quadratic",quadratic });
}

#pragma endregion

#pragma region SpotLight : PointLight

void LightSpot::Start()
{
	LightPoint::Start();
	transform->position = vec3(5, 3, 20);
	transform->rotation = vec3(radians(90.0f), radians(0.0f), radians(0.0f));
}
void LightSpot::OnGUI() const
{
	LightPoint::OnGUI();
	ImGui::DragFloat("cosPhyInner", (float*)&cosPhyInner, 0.01f, 0, 1);
	ImGui::DragFloat("cosPhyOuter", (float*)&cosPhyOuter, 0.01f, 0, 1);
}

LightSpot::LightSpot()
{
	name = "MonoBehavior_SpotLight";
}


LightSpot::~LightSpot()
{
}

void LightSpot::FromJson(const json & j)
{
	LightPoint::FromJson(j);
	cosPhyInner = j.at("cosPhyInner").get<float>();
	cosPhyOuter = j.at("cosPhyOuter").get<float>();
}

void LightSpot::ToJson(json & j) const
{
	LightPoint::ToJson(j);
	j.push_back(
		{ "cosPhyInner",cosPhyInner }
	);
	j.push_back(
		{ "cosPhyOuter",cosPhyOuter });
}

void LightSpot::SetShader(Shader * shader, int index)
{
	LightPoint::SetShader(shader, index);

	shader->setFloat(Sign() + "[" + std::to_string(index) + "]." + "cosPhyInner", this->cosPhyInner);
	shader->setFloat(Sign() + "[" + std::to_string(index) + "]." + "cosPhyOuter", this->cosPhyOuter);
}
#pragma endregion

#pragma region Mesh

void Mesh::Draw(Shader * shader)
{
	// bind appropriate textures
	unsigned int diffuseNr = 0;
	unsigned int specularNr = 0;
	unsigned int normalNr = 0;
	unsigned int heightNr = 0;
	for (unsigned int i = 0; i < textures.size(); i++)
	{

		glActiveTexture(GL_TEXTURE0 + i); // active proper texture unit before binding
		// retrieve texture number (the N in diffuse_textureN)
		string number;
		string name = textures[i].type;
		if (name == "texture_diffuse")
			number = std::to_string(diffuseNr++);
		else if (name == "texture_specular")
			number = std::to_string(specularNr++); // transfer unsigned int to stream
		else if (name == "texture_normal")
			number = std::to_string(normalNr++); // transfer unsigned int to stream
		else if (name == "texture_height")
			number = std::to_string(heightNr++); // transfer unsigned int to stream
												 // now set the sampler to the correct texture unit
		//std::cout << "material." + (name + number) << "\t" << i << std::endl;
		//glBindTexture(GL_TEXTURE_2D, textures[i].id);
		shader->setInt("material." + (name + number), i);
		glBindTexture(GL_TEXTURE_2D, textures[i].id);
		//shader->setInt( (name + number), i);
		// and finally bind the texture
	}

	// draw mesh
	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	// always good practice to set everything back to defaults once configured.
	glActiveTexture(GL_TEXTURE0);
}

Mesh::Mesh(float vertices[])
{
	this->vertices.resize(36);
	memcpy(&(this->vertices[0]), vertices, 36 * 8 * sizeof(float));
	SetUpMesh();
}

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures) :
	vertices(vertices),
	indices(indices),
	textures(textures)
{
	SetUpMesh();
}


Mesh::Mesh()
{
}


Mesh::~Mesh()
{
}

/*
	生成vao,vbo,ebo, 绑定数据，设置顶点属性及偏移
 */
void Mesh::SetUpMesh()
{
	// create buffers/arrays
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ebo);

	glBindVertexArray(vao);
	// load data into vertex buffers
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	// A great thing about structs is that their memory layout is sequential for all its items.
	// The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
	// again translates to 3/2 floats which translates to a byte array.
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	// set the vertex attribute pointers
	// vertex Positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	// vertex normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
	// vertex texture coords
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoord));
	// vertex tangent
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tangent));
	// vertex bitangent
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, bitangent));

	glBindVertexArray(0);

}
#pragma endregion

#pragma region Model : Object
void Model::LoadModel(string path)
{
	std::cout << path << std::endl;
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_FlipUVs | aiProcess_Triangulate | aiProcess_CalcTangentSpace);

	if (!scene || scene->mFlags&AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "Assimp error" << std::endl;
		return;
	}

	Directory = path.substr(0, path.find_last_of('\\'));
	name += path.substr(path.find_last_of('\\') + 1, path.length());
	std::cout << "Success!! " + Directory << std::endl;
	ProcessNode(scene->mRootNode, scene);

}

void Model::ProcessNode(aiNode * node, const aiScene * scene)
{
	//std::cout << node->mName.data << std::endl;

	for (size_t i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh * curMesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(processAiMesh(curMesh, scene));
	}

	for (size_t i = 0; i < node->mNumChildren; i++)
	{

		ProcessNode(node->mChildren[i], scene);
	}
}

Mesh Model::processAiMesh(aiMesh * aiMesh, const aiScene * aiscene)
{
	std::vector<Vertex> temVertexes;
	std::vector<unsigned int> tempIndices;
	std::vector<Texture> tempTextures;

	Vertex tempVer;
	for (size_t i = 0; i < aiMesh->mNumVertices; i++)
	{
		tempVer.position = vec3(
			aiMesh->mVertices[i].x,
			aiMesh->mVertices[i].y,
			aiMesh->mVertices[i].z);
		tempVer.normal = vec3(
			aiMesh->mNormals[i].x,
			aiMesh->mNormals[i].y,
			aiMesh->mNormals[i].z);

		if (aiMesh->mTextureCoords[0])
		{
			tempVer.texCoord = vec2(
				aiMesh->mTextureCoords[0][i].x,
				aiMesh->mTextureCoords[0][i].y);
		}
		else
		{
			tempVer.texCoord = vec2(0, 0);
		}

		if (aiMesh->HasTangentsAndBitangents())
		{
			//std::cout << name << " has tangents and bitangents" << std::endl;
			tempVer.tangent = vec3(
				aiMesh->mTangents[i].x,
				aiMesh->mTangents[i].y,
				aiMesh->mTangents[i].z);

			tempVer.bitangent = vec3(
				// bitangent
				aiMesh->mBitangents[i].x,
				aiMesh->mBitangents[i].y,
				aiMesh->mBitangents[i].z);
		}

		temVertexes.push_back(tempVer);
	}


	for (size_t i = 0; i < aiMesh->mNumFaces; i++)
	{
		for (size_t j = 0; j < aiMesh->mFaces[i].mNumIndices; j++)
		{
			tempIndices.push_back(aiMesh->mFaces[i].mIndices[j]);

		}
	}



	// process materials
	aiMaterial* material = aiscene->mMaterials[aiMesh->mMaterialIndex];


	std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
	tempTextures.insert(tempTextures.end(), diffuseMaps.begin(), diffuseMaps.end());
	// 2. specular maps
	std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
	tempTextures.insert(tempTextures.end(), specularMaps.begin(), specularMaps.end());
	// 3. normal maps
	std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
	tempTextures.insert(tempTextures.end(), normalMaps.begin(), normalMaps.end());
	// 4. height maps
	std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
	tempTextures.insert(tempTextures.end(), heightMaps.begin(), heightMaps.end());
	// 1. diffuse maps


	return Mesh(temVertexes, tempIndices, tempTextures);
}

std::vector<Texture> Model::loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName)
{
	std::cout << name << " is trying to load materials" << std::endl;
	std::vector<Texture> textures;
	if (mat->GetTextureCount(type) == 0)
	{
		Texture texture;
		std::cout << typeName << " " << std::endl;
		texture.id = TextureFromFile((typeName + ".jpg").c_str(), this->Directory);
		texture.type = typeName;
		texture.path = typeName.c_str();
		textures.push_back(texture);
		textures_loaded.push_back(texture);  // store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.

	}
	else
	{
		for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
		{
			aiString str;
			mat->GetTexture(type, i, &str);
			// check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
			bool skip = false;
			for (unsigned int j = 0; j < textures_loaded.size(); j++)
			{
				if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
				{
					textures.push_back(textures_loaded[j]);
					skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
					break;
				}
			}
			if (!skip)
			{   // if texture hasn't been loaded already, load it
				Texture texture;
				std::cout << str.C_Str() << " " << std::endl;
				texture.id = TextureFromFile(str.C_Str(), this->Directory);
				texture.type = typeName;
				texture.path = str.C_Str();
				textures.push_back(texture);
				textures_loaded.push_back(texture);  // store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
			}
		}
	}

	return textures;
}



void Model::OnGUI() const
{
	if (ImGui::TreeNode(name.c_str()))
	{
		ImGui::Text("name: %s", name.c_str());
		ImGui::TreePop();
		ImGui::Spacing();
	}
}

void Model::Draw(Shader * shader)
{
	for (size_t i = 0; i < meshes.size(); i++)
	{
		meshes[i].Draw(shader);
	}
}

Model::Model(string path) :Object("Model_")
{
	LoadModel(path);
}

Model::~Model()
{
}
#pragma endregion

#pragma region Shader : Object
void Shader::AddLight(AbstractLight * light)
{
	light->SetShader(this, Setting::LightCount(light->Type()) - 1);
}

void Shader::OnGUI() const
{
	if (ImGui::TreeNode(name.c_str()))
	{
		ImGui::Text("name: %s", name.c_str());
		ImGui::TreePop();
		ImGui::Spacing();
	}
}

Shader::Shader(string sign, const char* geometryPath) :Object("Shader_" + sign)
{
	std::cout << "Shader Name: " << sign << std::endl;
	// 1. retrieve the vertex/fragment source code from filePath
	std::string vertexCode;
	std::string fragmentCode;
	std::string geometryCode;
	std::ifstream vShaderFile;
	std::ifstream fShaderFile;
	std::ifstream gShaderFile;
	// ensure ifstream objects can throw exceptions:
	vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	gShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try
	{
		// open files
		vShaderFile.open(sign + ".vert");
		fShaderFile.open(sign + ".frag");
		std::stringstream vShaderStream, fShaderStream;
		// read file's buffer contents into streams
		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();
		// close file handlers
		vShaderFile.close();
		fShaderFile.close();
		// convert stream into string
		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();
		// if geometry shader path is present, also load a geometry shader
		if (geometryPath != nullptr)
		{
			gShaderFile.open(geometryPath);
			std::stringstream gShaderStream;
			gShaderStream << gShaderFile.rdbuf();
			gShaderFile.close();
			geometryCode = gShaderStream.str();
		}
	}
	catch (std::ifstream::failure e)
	{
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
	}

	const char* vShaderCode = vertexCode.c_str();
	const char * fShaderCode = fragmentCode.c_str();

	// 2. compile shaders
	unsigned int vertex, fragment;
	// vertex shader
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vShaderCode, NULL);
	glCompileShader(vertex);
	checkCompileErrors(vertex, "VERTEX");
	// fragment Shader
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fShaderCode, NULL);
	glCompileShader(fragment);
	checkCompileErrors(fragment, "FRAGMENT");
	// if geometry shader is given, compile geometry shader
	unsigned int geometry;
	if (geometryPath != nullptr)
	{
		const char * gShaderCode = geometryCode.c_str();
		geometry = glCreateShader(GL_GEOMETRY_SHADER);
		glShaderSource(geometry, 1, &gShaderCode, NULL);
		glCompileShader(geometry);
		checkCompileErrors(geometry, "GEOMETRY");
	}
	// shader Program
	ID = glCreateProgram();
	glAttachShader(ID, vertex);
	glAttachShader(ID, fragment);
	if (geometryPath != nullptr)
		glAttachShader(ID, geometry);
	glLinkProgram(ID);
	checkCompileErrors(ID, "PROGRAM");
	// delete the shaders as they're linked into our program now and no longer necessery
	glDeleteShader(vertex);
	glDeleteShader(fragment);
	if (geometryPath != nullptr)
		glDeleteShader(geometry);

}
void Shader::use()
{
	glUseProgram(ID);
}
void Shader::setBool(const std::string &name, bool value) const
{
	glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}
void Shader::setInt(const std::string &name, int value) const
{
	glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}
void Shader::setFloat(const std::string &name, float value) const
{
	glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}
void Shader::setVec2(const std::string &name, const glm::vec2 &value) const
{
	glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}
void Shader::setVec2(const std::string &name, float x, float y) const
{
	glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
}
void Shader::setVec3(const std::string &name, const glm::vec3 &value) const
{
	glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}
void Shader::setVec3(const std::string &name, float x, float y, float z) const
{
	glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
}
void Shader::setVec4(const std::string &name, const glm::vec4 &value) const
{
	glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}
void Shader::setVec4(const std::string &name, float x, float y, float z, float w)
{
	glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
}
void Shader::setMat2(const std::string &name, const glm::mat2 &mat) const
{
	glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
void Shader::setMat3(const std::string &name, const glm::mat3 &mat) const
{
	glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
void Shader::setMat4(const std::string &name, const glm::mat4 &mat) const
{
	glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}


void Shader::checkCompileErrors(GLuint shader, std::string type)
{
	GLint success;
	GLchar infoLog[1024];
	if (type != "PROGRAM")
	{
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(shader, 1024, NULL, infoLog);
			std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
		}
	}
	else
	{
		glGetProgramiv(shader, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(shader, 1024, NULL, infoLog);
			std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
		}
	}
}

#pragma endregion

#pragma region AbstractMaterial : Object

AbstractMaterial::AbstractMaterial(Shader * shader, vec3 color, float skininess) :
	Object("Material_"),
	shader(shader),
	shininess(skininess),
	color(color)
{
}



void AbstractMaterial::OnGUI() const
{
	if (ImGui::TreeNode(name.c_str()))
	{
		ImGui::Text("name: %s", name.c_str());
		ImGui::Checkbox("EnableSpecular", (bool*)&specular);
		ImGui::DragFloat("Skininess", (float*)&shininess, 0.5f, 1, 64);
		ImGui::ColorEdit3((name + "_color").c_str(), (float*)&color);
		shader->OnGUI();
		ImGui::TreePop();
		ImGui::Spacing();
	}
}



AbstractMaterial::~AbstractMaterial()
{
}

void AbstractMaterial::Use(mat4 & view, mat4 & proj, mat4 model)
{
	shader->use();
	shader->setMat4("viewMat", view);
	shader->setMat4("projMat", proj);
	shader->setMat4("modelMat", model);
	shader->setFloat("material.shininess", shininess);
	shader->setVec3("material.color", color);
	shader->setBool("specular", specular);
	shader->setVec3("cameraPos", Setting::MainCamera->gameObject->transform()->position);
}

#pragma endregion

#pragma region StandardMaterial : AbstractMaterial
StandandMaterial::StandandMaterial(Shader * shader, vec3 color, float skininess) :
	AbstractMaterial(shader, color, skininess)
{
	name += "StandandMaterial";
}


StandandMaterial::~StandandMaterial()
{
}

void StandandMaterial::Use(mat4 & view, mat4 & proj, mat4 model)
{
	AbstractMaterial::Use(view, proj, model);
	for (auto light : *Setting::lights)
		shader->AddLight(light);
}
#pragma endregion

#pragma region BoxMaterial : AbstractMaterial
BoxMaterial::BoxMaterial(Shader * shader, string specularPath, string diffusePath, vec3 color, float skininess) :
	AbstractMaterial(shader, color, skininess),
	specularPath(specularPath),
	diffusePath(diffusePath)
{
}

void BoxMaterial::OnGUI() const
{
	AbstractMaterial::OnGUI();
}

void BoxMaterial::Use(mat4 & view, mat4 & proj, mat4 model)
{
	AbstractMaterial::Use(view, proj, model);

	shader->setInt("material.texture_diffuse0", TextureFromFile(diffusePath));
	shader->setInt("material.texture_specular0", TextureFromFile(specularPath));

	for (auto light : *Setting::lights)
		shader->AddLight(light);
}

BoxMaterial::~BoxMaterial()
{
}

#pragma endregion

#pragma region ModelRender

void ModelRender::OnGUI()const
{
	MonoBehavior::OnGUI();
	material->OnGUI();
	model->OnGUI();
}

void ModelRender::RealUpdate()
{
	MonoBehavior::RealUpdate();
	material->Use(viewMat, projMat, gameObject->transform()->GetModelMaterix());
	model->Draw(material->shader);
}

void ModelRender::Start()
{
	MonoBehavior::Start();
	material = new StandandMaterial(new Shader(shaderName));
	model = new Model(workDir.substr(0, workDir.find_last_of('\\')) + "\\" + modelName);
}

ModelRender::ModelRender()
{
	name += "ModelRender";
}


ModelRender::~ModelRender()
{
}

#pragma endregion

#pragma region SkyboxRender : MonoBehavior
#pragma region Date
vector<std::string> faces
{
	"right.jpg",
	"left.jpg",
	"top.jpg",
	"bottom.jpg",
	"front.jpg",
	"back.jpg"
};

float skyboxVertices[] = {
	// positions          
	-200.0f,  200.0f, -200.0f,
	-200.0f, -200.0f, -200.0f,
	 200.0f, -200.0f, -200.0f,
	 200.0f, -200.0f, -200.0f,
	 200.0f,  200.0f, -200.0f,
	-200.0f,  200.0f, -200.0f,

	-200.0f, -200.0f,  200.0f,
	-200.0f, -200.0f, -200.0f,
	-200.0f,  200.0f, -200.0f,
	-200.0f,  200.0f, -200.0f,
	-200.0f,  200.0f,  200.0f,
	-200.0f, -200.0f,  200.0f,

	 200.0f, -200.0f, -200.0f,
	 200.0f, -200.0f,  200.0f,
	 200.0f,  200.0f,  200.0f,
	 200.0f,  200.0f,  200.0f,
	 200.0f,  200.0f, -200.0f,
	 200.0f, -200.0f, -200.0f,

	-200.0f, -200.0f,  200.0f,
	-200.0f,  200.0f,  200.0f,
	 200.0f,  200.0f,  200.0f,
	 200.0f,  200.0f,  200.0f,
	 200.0f, -200.0f,  200.0f,
	-200.0f, -200.0f,  200.0f,

	-200.0f,  200.0f, -200.0f,
	 200.0f,  200.0f, -200.0f,
	 200.0f,  200.0f,  200.0f,
	 200.0f,  200.0f,  200.0f,
	-200.0f,  200.0f,  200.0f,
	-200.0f,  200.0f, -200.0f,

	-200.0f, -200.0f, -200.0f,
	-200.0f, -200.0f,  200.0f,
	 200.0f, -200.0f, -200.0f,
	 200.0f, -200.0f, -200.0f,
	-200.0f, -200.0f,  200.0f,
	 200.0f, -200.0f,  200.0f
};


#pragma endregion




void SkyboxRender::Start()
{
	MonoBehavior::Start();
	unsigned int skyboxVAO, skyboxVBO;
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);


	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glActiveTexture(GL_TEXTURE0);

	this->vao = skyboxVAO;

	material = new StandandMaterial(new Shader("sky"));
	material->shader->setInt("skybox", 0);
}

void SkyboxRender::OnGUI() const
{
	MonoBehavior::OnGUI();
}

void SkyboxRender::RealUpdate()
{
	MonoBehavior::RealUpdate();
	material->Use(viewMat, projMat, mat4(mat3(gameObject->transform()->GetModelMaterix())));
	auto x = gameObject->transform()->scale;
	//std::cout << x.x << " " << x.y << " " << x.z << std::endl;
	material->shader->setInt("skybox", 0);
	glDepthMask(GL_FALSE);
	glBindVertexArray(vao);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureId);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
	glDepthMask(GL_TRUE);
}

SkyboxRender::SkyboxRender()
{
	name += "SkyboxRender";
	textureId = loadCubemap(faces);
}


SkyboxRender::~SkyboxRender()
{
}
#pragma endregion

#pragma region GameObject : Object
template<typename T>
T* GameObject::AddComponentStart()
{
	var mb = (MonoBehavior*)new T();
	mb->gameObject = this;
	mb->Start();
	scripts->push_back(mb);
	return (T*)mb;
}

template<typename T>
T* GameObject::AddComponentNoStart()
{
	var mb = (MonoBehavior*)new T();
	mb->gameObject = this;
	scripts->push_back(mb);
	return (T*)mb;
}

template<typename T>
T* GameObject::AddComponentStart(MonoBehavior* component)
{
	component->gameObject = this;
	scripts->push_back(component);
	return (T*)component;
}

template<typename T>
T* GameObject::GetComponent() const
{
	for (auto mb : *scripts)
		if (typeid(*mb) == typeid(T))
			return (T*)mb;
	return nullptr;
}

template<typename T>
void  GameObject::RemoveComponent()
{
	for (auto x : *scripts)
		if(typeid(*x)==typeid(T))
			scripts->remove(x);
}

Transform * GameObject::transform() const
{
	return GetComponent<Transform>();
}
void GameObject::OnGUI() const
{
	ImGui::Checkbox("Enable", (bool*)&enable);
	if (ImGui::Button("Go to here"))
	{
		if (Setting::MainCamera&&Setting::MainCamera->gameObject->enable)
			Setting::MainCamera->gameObject->transform()->position = transform()->position;
	}
}
static int idS = 0;

GameObject::GameObject(string name, Type type) :Object("GameObject_" + name)
{

	Camera* camera;
	this->id = idS++;

	Setting::gameObjects->push_back(this);
	scripts = new std::list<MonoBehavior*>();
	AddComponentStart<Transform>();
	switch (type)
	{
	case Cameras:
		camera = AddComponentStart<Camera>();
		if (!Setting::MainCamera)
		{
			Setting::MainCamera = camera;
			AddComponentStart<CameraMove>();

			auto jb=AddComponentStart<Rotate>();
			delete jb;
			scripts->remove(jb);
			
		}
		break;
	case Directional:
		AddComponentStart<LightDirectional>();
		break;
	case Point:
		AddComponentStart<LightPoint>();
		break;
	case Spot:
		AddComponentStart<LightSpot>();
		break;
	case Box:
		AddComponentStart<SkyboxRender>();
		break;
	case Model:
		AddComponentStart<ModelRender>();
		break;
	case Empty:
		break;
	default:
		break;
	}
}

GameObject::GameObject(string name, string modelName, string shaderSign) :Object("GameObject_" + name)
{
	this->id = idS++;

	Setting::gameObjects->push_back(this);
	scripts = new std::list<MonoBehavior*>();
	AddComponentStart<Transform>();
	auto modelRender = AddComponentNoStart<ModelRender>();
	modelRender->modelName = modelName;
	modelRender->shaderName = shaderSign;
	modelRender->Start();

}



GameObject::~GameObject()
{
	for (auto x : *scripts)
		delete x;
	delete scripts;
}

#pragma endregion

#pragma region Setting
std::vector<AbstractLight*>* Setting::lights = nullptr;
std::list<GameObject*>* Setting::gameObjects = nullptr;

string const Setting::settingDir = workDir + "\\settings";
Camera* Setting::MainCamera = nullptr;
bool Setting::lockMouse = false;
float Setting::deltaTime = 0.02f;


vec2 Setting::windowSize = vec2(1200, 1000);


int Setting::LightCount(AbstractLight::LightType type)
{
	int n = 0;
	for (auto x : *lights)
		if (x->Type() == type)
			n++;
	return n;

}


void Setting::InitSettings()
{
	pWindowSize = &windowSize;
	lights = new std::vector<AbstractLight*>();
	gameObjects = new std::list<GameObject*>();
}
#pragma endregion


