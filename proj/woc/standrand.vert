#version 330 core										//那个版本使用这个shader
layout(location = 0) in vec3 aPos;		
layout(location = 1) in vec3 aNormal;  
layout(location = 2) in vec2 aTexCoord;
layout (location = 3) in vec3 tangent;
layout (location = 4) in vec3 bitangent;



uniform mat4 modelMat; 
uniform mat4 viewMat;
uniform mat4 projMat;

//out vec4 vertexColor;
out vec2 TexCoord;
out vec3 fragPos;
out vec3 Normal;
out mat3 tbn;

void main(){	


	gl_Position = projMat*viewMat*modelMat*vec4(aPos,1.0f);//*transform;
	fragPos=(modelMat*vec4(aPos.xyz,1.0f)).xyz;
	TexCoord=aTexCoord;
	//Normal=mat3((modelMat))*aNormal;
	Normal=mat3(transpose(inverse(modelMat)))*aNormal;
	vec3 T = normalize(vec3(modelMat * vec4(tangent,   0.0)));
	vec3 B = normalize(vec3(modelMat * vec4(bitangent, 0.0)));
	vec3 N = normalize(vec3(modelMat * vec4(aNormal,    0.0)));
	tbn = mat3(T, B, N);
	//vertexColor=vec4(aColor.x+0.5f,aColor.y,aColor.z+1.1f,1.0);
	//TexCoord=aTexCoord;
}//四元数纯量位置1，变量位置

