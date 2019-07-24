#version 330 core

struct Material
{
	//vec3 ambient;
	sampler2D texture_diffuse0;
	sampler2D texture_specular0;
	sampler2D texture_normal0;
	vec3 color;
	float shininess;
};


struct LightDirectional
{
	bool flag;
	vec3 pos;
	vec3 color;
	vec3 dirToLight;
};

struct LightSpot
{
	bool flag;
	vec3 pos;
	vec3 color;
	vec3 dirToLight;

	float constant;
	float linear;
	float quadratic;

	float cosPhyInner;
	float cosPhyOuter;
};

struct LightPoint
{
	bool flag; 
	vec3 pos;
	vec3 color;
	vec3 dirToLight;

	float constant;
	float linear;
	float quadratic;
};

#define NR_POINT_LIGHTS 4

in vec3 fragPos;
in vec3 Normal;
in vec2 TexCoord;
in mat3 tbn;

uniform Material material;
uniform LightDirectional lightDirectional[NR_POINT_LIGHTS];
uniform LightPoint lightPoint[NR_POINT_LIGHTS];
uniform LightSpot lightSpot[NR_POINT_LIGHTS];

uniform vec3 cameraPos;
uniform bool specular;
uniform bool normal;

out vec4 FragColor;

vec3 CalcLightDirectional(LightDirectional light,vec3 uNormal,vec3 dirToCamera);
vec3 CalcLightPoint(LightPoint light,vec3 uNormal,vec3 dirToCamera);
vec3 CalcLightSpot(LightSpot light,vec3 uNormal,vec3 dirToCamera);
void main()
{
	vec3 finalColor=vec3(0,0,0);
	//vec3 finalColor=vec3(1,1,1);

	vec3 uNormal=normalize(texture(material.texture_normal0,TexCoord).rgb*2.0f-1.0f);
	uNormal=normalize(tbn*uNormal);

	vec3 dirToCamera=normalize(cameraPos-fragPos);

	//finalColor+=vec3(1,1,1);
	for(int i=0;i<NR_POINT_LIGHTS;i++)
	{
		if(lightDirectional[i].flag)
			finalColor+=CalcLightDirectional(lightDirectional[i],uNormal,dirToCamera);
		
		if(lightPoint[i].flag)
			finalColor+=CalcLightPoint(lightPoint[i],uNormal,dirToCamera);
		
		if(lightSpot[i].flag)
			finalColor+=CalcLightSpot(lightSpot[i],uNormal,dirToCamera);
	}
	FragColor=vec4(finalColor*material.color,1.0f);
}

vec3 CalcLightDirectional(LightDirectional light,vec3 uNormal,vec3 dirToCamera)
{
	//diffuse	max(dot(L,N),0)
	float diffIntensity=max(0,dot(light.dirToLight,uNormal));
	vec3 diffColor=diffIntensity*light.color*texture(material.texture_diffuse0,TexCoord).rgb;

	//specular	pos(max(0,dot(R,Cam),shininess)
	vec3 R=normalize(reflect(-light.dirToLight,uNormal));
	float specIntensity=pow(max(0,dot(R,dirToCamera)),material.shininess);
	vec3 specularColor=specIntensity*light.color*texture(material.texture_specular0,TexCoord).rgb;

	if(specular)
		return diffColor+specularColor;
	else 
		return diffColor;
}

vec3 CalcLightPoint(LightPoint light,vec3 uNormal,vec3 dirToCamera)
{
	//attenuation
	float dist=length(light.pos-fragPos);
	float attenuation=1.0f/(light.constant+light.linear*dist+light.quadratic*(dist*dist));

	//diffuse
	float diffIntensity=max(0,dot(light.pos-fragPos,uNormal))*attenuation;
	vec3 diffColor=diffIntensity*light.color*texture(material.texture_diffuse0,TexCoord).rgb;

	//specular
	vec3 R=normalize(reflect(-light.pos-fragPos,uNormal));
	float specIntensity=pow(max(0,dot(R,dirToCamera)),material.shininess)*attenuation;
	vec3 specularColor=specIntensity*light.color*texture(material.texture_specular0,TexCoord).rgb;

	
	if(specular)
		return diffColor+specularColor;
	else 
		return diffColor;
}

vec3 CalcLightSpot(LightSpot light,vec3 uNormal,vec3 dirToCamera)
{
	//attenuation
	float dist=length(light.pos-fragPos);
	float attenuation=1.0f/(light.constant+light.linear*dist+light.quadratic*(dist*dist));
	float spotRatio;
	float cosTheta=dot(normalize(fragPos-light.pos),-light.dirToLight);
	 if(cosTheta>light.cosPhyInner)
	{
		//inside
		spotRatio=1.0f;
	}
	else if(cosTheta>light.cosPhyOuter)
	{
		//middle
		spotRatio=1.0-(cosTheta-light.cosPhyInner)/(light.cosPhyOuter-light.cosPhyInner);
	}
	else
	{
		//outside
		spotRatio=0.0f;
	}

	//diffuse
	float diffIntensity=max(0,dot(light.pos-fragPos,uNormal))*attenuation*spotRatio;
	vec3 diffColor=diffIntensity*light.color*texture(material.texture_diffuse0,TexCoord).rgb;

	//specular
	vec3 R=normalize(reflect(-light.pos-fragPos,uNormal));
	float specIntensity=pow(max(0,dot(R,dirToCamera)),material.shininess)*attenuation;
	vec3 specularColor=specIntensity*light.color*texture(material.texture_specular0,TexCoord).rgb;

	if(specular)
		return diffColor+specularColor;
	else 
		return diffColor;
}

