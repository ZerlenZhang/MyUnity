
#include"All.h"
#include"Save.h"
vec2* pWindowSize;

#pragma region GloalVar
GLFWwindow* window;

mat4 modelMat;

mat4 viewMat;

mat4 projMat;

string workDir;

#pragma endregion


#pragma region 函数声明
unsigned int loadCubemap(vector<std::string> faces);
void KeyBoardCallBack(GLFWwindow* window);
void MouseCallBack(GLFWwindow* window, double xPos, double yPos);
void OnSize(GLFWwindow* window, int width, int height);
static void glfw_error_callback(int error, const char* description)
{
	fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}
unsigned int TextureFromFile(string filename);
unsigned int TextureFromFile(const char *path, const string &directory, bool gamma);
unsigned int LoadIamgeToGPU(const char* filename, GLint internalFormat, GLenum format, int textureSlot);
#pragma endregion


#pragma region Data

#pragma region SkyBoxes

#pragma endregion

#pragma endregion

class TTT :public MonoBehavior {};

int main(int argc, char* argv[])
{
	workDir = argv[0];


#pragma region Open a window



	glfwSetErrorCallback(glfw_error_callback);
	if (!glfwInit())
		return 1;
	const char* glsl_version = "#version 130";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	window = glfwCreateWindow(Setting::windowSize.x, Setting::windowSize.y, "nothing here", NULL, NULL);
	if (window == NULL)
		return 1;
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);
	bool err = glewInit() != GLEW_OK;
	if (err)
	{
		fprintf(stderr, "Failed to initialize OpenGL loader!\n");
		return 1;
	}

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);
#pragma endregion

#pragma region Opengl_Setting

	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


	glfwSetCursorPosCallback(window, MouseCallBack);
	glfwSetFramebufferSizeCallback(window, OnSize);//拖拽的时候设置屏幕大小

	glViewport(0, 0, Setting::windowSize.x, Setting::windowSize.y);//起始坐标在左下角，0,0，宽800，高600

	glEnable(GL_DEPTH_TEST);//深度缓冲


#pragma endregion

#pragma region LoadSetting

	Setting::InitSettings();

#pragma endregion
	Input::InitInput();
	ScreenshotMaker sm;

	//new GameObject("NewDirectionalLight", GameObject::Directional);
	//auto g=new GameObject("NewCamera", GameObject::Cameras);
	static GameObject* selected = nullptr;

	// Main loop
	while (!glfwWindowShouldClose(window))
	{
		//cout <<"???"<< std::endl;
		static float lastTime = 0;


#pragma region Others

		static ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
		glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//用户输入
		KeyBoardCallBack(window);

#pragma endregion


#pragma region Init_ImGui
		glfwPollEvents();


		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

#pragma endregion





#pragma region MultyCameraRender

		vector<Camera*> cameras;
		for (auto go : *Setting::gameObjects)
			if (go->enable)
				for (auto mb : *(go->scripts))
					if (typeid(*mb) == typeid(Camera) && mb->enable)
						cameras.push_back((Camera*)mb);

		for (auto camera : cameras)
		{
			camera->Update();

#pragma region MainLoop_RealUpdate
			for (auto go : *Setting::gameObjects)
				if (go->enable)
					for (auto mb : *(go->scripts))
						if (mb->enable)
							mb->RealUpdate();

#pragma endregion

		}


#pragma endregion


#pragma region DrawImGui




		static float f = 0.0f;
		static int counter = 0;

		ImGui::Begin("Inspector");                          // Create a window called "Hello, world!" and append into it.

#pragma region Menu


		static float colors[3];
		if (ImGui::BeginMenu("GameObject"))
		{
			if (ImGui::MenuItem("DeleteGameObject"))
				if (selected)
				{
					Setting::gameObjects->remove(selected);
					delete selected;
					selected = nullptr;
				}
			if (ImGui::MenuItem("CreateEmpty"))
				new GameObject("NewEmpty", GameObject::Empty);

			if (ImGui::MenuItem("CreateCamera", NULL, true))
				new GameObject("NewCamera", GameObject::Cameras);


			if (ImGui::MenuItem("CreateSkybox", NULL, true))
				new GameObject("NewSkyBox", GameObject::Box);

			if (ImGui::BeginMenu("Light"))
			{
				if (ImGui::MenuItem("CreateDirectional", NULL, true))
					new GameObject("NewDirectionalLight", GameObject::Directional);

				if (ImGui::MenuItem("CreatePoint", NULL, true))
					new GameObject("NewPointLight", GameObject::Point);

				if (ImGui::MenuItem("CreateSpot", NULL, true))
					new GameObject("NewSpotLight", GameObject::Spot);

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Model"))
			{
				if (ImGui::MenuItem("CreatePerson", NULL, true))
					new GameObject("NewPerson", GameObject::Model);
				if (ImGui::MenuItem("CreateHouse", NULL, true))
					new GameObject("NewHouse", "东方场景01.obj", "standrand");

				if (ImGui::MenuItem("CreateRobot", NULL, true))
					new GameObject("NewRobot", "source\\Robot.obj", "standrand");
				ImGui::EndMenu();
			}




			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Component"))
		{
			if (ImGui::MenuItem("Camera"))
				if (selected)
					selected->AddComponentStart<Camera>();

			if (ImGui::BeginMenu("Light"))
			{
				if (ImGui::MenuItem("DirectionalLight"))
					if (selected)
						selected->AddComponentStart<LightDirectional>();
				if (ImGui::MenuItem("PointLight"))
					if (selected)
						selected->AddComponentStart<LightPoint>();
				if (ImGui::MenuItem("SpotLight"))
					if (selected)
						selected->AddComponentStart<LightSpot>();
				ImGui::EndMenu();
			}

			if (ImGui::MenuItem("Rotate"))
				if (selected)
					selected->AddComponentStart<Rotate>();


			ImGui::EndMenu();
		}


		if (ImGui::CollapsingHeader("Global"))
		{
			ImGui::ColorEdit4("ClearColor", (float*)&clear_color);
		}



#pragma region MainLoop_OnGUI

		static bool camein = false;
		if (ImGui::CollapsingHeader("GameObjects"))
		{
			for (auto go : *Setting::gameObjects)
			{
				if (ImGui::TreeNode((go->name + std::to_string(go->id)).c_str()))
				{
					camein = true;
					selected = go;
					go->OnGUI();
					for (auto mb : *(go->scripts))
					{
						if (ImGui::CollapsingHeader(mb->name.c_str()))
						{
							mb->OnGUI();
						}
					}

					ImGui::TreePop();
					ImGui::Spacing();
				}
			}
		}

		if (!camein)
			selected = nullptr;
#pragma endregion		


		sm.Draw();


		ImGui::End();


#pragma endregion	


#pragma endregion

#pragma region MainLoop_Update

		if (glfwGetTime() - lastTime >= Setting::deltaTime)
		{

			Input::GetInput();

			//update
			for (auto go : *Setting::gameObjects)
				if (go->enable)
					for (auto mb : *(go->scripts))
						if (mb->enable)
							mb->Update();

			//clearframeinput
			Input::ClearInputEveryFrame();
			lastTime = glfwGetTime();
		}



#pragma endregion

#pragma region RenderImGui
		// Rendering
		ImGui::Render();
		int display_w, display_h;
		glfwMakeContextCurrent(window);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		glfwMakeContextCurrent(window);


		glfwSwapBuffers(window);
#pragma endregion

	}


#pragma region Clear_ImGui
	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

#pragma endregion



	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}





#pragma region CallBack

void KeyBoardCallBack(GLFWwindow* window)
{
	static bool laststate = false;
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}


	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		Input::Key[W_] = true;
	}
	else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		Input::Key[S_] = true;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		Input::Key[A_] = true;
	}
	else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		Input::Key[D_] = true;
	}
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
	{
		Input::Key[Q_] = true;
	}
	else if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
	{
		Input::Key[E_] = true;
	}



#pragma region Acceselate
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
	{
		Input::Key[Shift_] = true;
	}
#pragma endregion

#pragma region LockMouse

	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		Input::Key[Space_] = true;
		//if (!Setting::lockMouse == laststate)
		//{
		//	Setting::lockMouse = !Setting::lockMouse;
		//	laststate = Setting::lockMouse;

		//	if (Setting::lockMouse)
		//	{
		//		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		//	}
		//	else
		//	{
		//		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		//	}
		//}

	}
	else if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE)
	{
		if (Setting::lockMouse == laststate)
			laststate = !Setting::lockMouse;
	}

#pragma endregion

#pragma region Debug
	static bool store = false;
	if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS)
	{
		//cout << "h";


		cout << glfwGetTime() << std::endl;
	}
#pragma endregion



}

void MouseCallBack(GLFWwindow* window, double xPos, double yPos)
{
	static double lastX, lastY;
	static bool firstCome = true;

	if (firstCome)
	{
		lastX = xPos;
		lastY = yPos;
		firstCome = false;
	}

	auto deltaX = xPos - lastX;
	auto deltaY = yPos - lastY;

	lastX = xPos;
	lastY = yPos;

	Input::mouseMentDelta = vec2(deltaX, deltaY);

}

void OnSize(GLFWwindow* window, int width, int height)
{
	Setting::windowSize = vec2(width, height);

	projMat = perspective(radians(60.0f), Setting::windowSize.x / Setting::windowSize.y, 0.1f, 100.0f);
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}
#pragma endregion

#pragma region Functions

unsigned int TextureFromFile(string filename)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load " << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}

unsigned int TextureFromFile(const char * path, const string & directory, bool gamma)
{
	string filename = string(path);
	filename = directory + '/' + filename;

	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}
unsigned int LoadIamgeToGPU(const char* filename, GLint internalFormat, GLenum format, int textureSlot)
{
	unsigned int tex1;
	glGenTextures(1, &tex1);
	glActiveTexture(GL_TEXTURE0 + textureSlot);
	glBindTexture(GL_TEXTURE_2D, tex1);


	stbi_set_flip_vertically_on_load(true);

	int weight, height, nrVhannel;

	unsigned char* data = stbi_load(filename, &weight, &height, &nrVhannel, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, weight, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		printf("no image");
	}
	stbi_image_free(data);
	return tex1;
}

unsigned int loadCubemap(vector<std::string> faces)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, nrChannels;
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
			);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
			stbi_image_free(data);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureID;
}
#pragma endregion

