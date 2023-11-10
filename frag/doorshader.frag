#version 330 core

struct Material
{
    //环境因子
    //不透明贴图
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
}; 

struct Light 
{
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;

out vec4 FragColor;

uniform Light light;
uniform Material material;
uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform sampler2D matrixLight;

uniform int haveenter;

void main()
{
    //环境光
    vec3 ambient = vec3(texture(material.diffuse, TexCoords)) * light.ambient;

    //漫反射
    //标准化顶点法向量
    vec3 norm = normalize(Normal);
    //指向光源的法向量
    vec3 lightDir = normalize(lightPos - FragPos);
    //求法向量夹角
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = vec3(texture(material.diffuse, TexCoords)) * diff * light.diffuse;

    //高光
    //观测方向
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    //计算镜面分量
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = vec3(texture(material.specular, TexCoords)) * spec * light.specular;

    vec3 highlight = vec3(texture(matrixLight, TexCoords));

    //考虑环境光照，漫反射光照以及高光
    vec3 result = ambient + diffuse + specular + highlight;
    
    if(haveenter==0)
        FragColor = vec4(result, 1.0f);
    else    
        FragColor = vec4(0.2226f, 0.769f,  0.7304f, 1.0f);
}