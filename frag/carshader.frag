#version 330 core

//材质顶点坐标
in vec2 TexCoords;
//法线
in vec3 Normal;
//物体坐标
in vec3 FragPos;

//物体最终颜色
out vec4 FragColor;

//物体原本颜色
uniform vec3 objectColor;
//光照颜色
uniform vec3 lightColor;
//照相机位置
uniform vec3 viewPos;

struct Material
{
    //环境因子
    //不透明贴图
    sampler2D texture_diffuse1;
    sampler2D texture_specular1;
    float shininess;
};
uniform Material material;

//定向光
struct DirLight
{
    vec3 direction;
    
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
vec3 CalcDirLight(DirLight light);
uniform DirLight dirlight;

//点光源
struct PointLight 
{
    vec3 position;

    //光线衰减变量
    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};  
//点光源数量
#define NR_POINT_LIGHTS 1
vec3 CalcPointLight(PointLight light);
uniform PointLight pointlights[NR_POINT_LIGHTS];

struct SpotLight 
{
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    //光线衰减变量
    float constant;
    float linear;
    float quadratic;

    //手电筒变量
    vec3 direction;
    float cutOff;
    //软化边缘变量
    float outerCutOff;
};
vec3 CalcSpotLight(SpotLight light);
uniform SpotLight spotLight;

void main()
{
    //平行光
    vec3 result = CalcDirLight(dirlight);

    // 点光源
    for(int i = 0; i < NR_POINT_LIGHTS; i++)
        result += CalcPointLight(pointlights[i]);

    // 聚光
    //result += CalcSpotLight(spotLight); 

    FragColor = vec4(result, 1.0);
}

vec3 CalcDirLight(DirLight light)
{
    //观测方向
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 lightDir = normalize(-light.direction);
    //法向量
    vec3 normal = normalize(Normal);
    
    // 漫反射
    // 求法向量夹角
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse  = light.diffuse  * diff * vec3(texture(material.texture_diffuse1, TexCoords));

    // 高光
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * vec3(texture(material.texture_specular1, TexCoords));

    // 环境光
    vec3 ambient  = light.ambient  * vec3(texture(material.texture_diffuse1, TexCoords));

    return (ambient + diffuse + specular);
}

vec3 CalcPointLight(PointLight light)
{
    //观测方向
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 lightDir = normalize(light.position - FragPos);
    //法向量
    vec3 normal = normalize(Normal);
    
    // 漫反射
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse  = light.diffuse  * diff * vec3(texture(material.texture_diffuse1, TexCoords));

    // 高光
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * vec3(texture(material.texture_specular1, TexCoords));

    // 环境光
    vec3 ambient  = light.ambient  * vec3(texture(material.texture_diffuse1, TexCoords));

    // 衰减
    float distance    = length(light.position - FragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    

    return (ambient*attenuation + diffuse*attenuation + specular*attenuation);
}

vec3 CalcSpotLight(SpotLight light)
{
    //观测方向
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 lightDir = normalize(light.position - FragPos);
    //法向量
    vec3 normal = normalize(Normal);

    //环境光
    vec3 ambient = vec3(texture(material.texture_diffuse1, TexCoords)) * light.ambient;

    //漫反射
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = vec3(texture(material.texture_diffuse1, TexCoords)) * diff * light.diffuse;

    //高光
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = vec3(texture(material.texture_specular1, TexCoords)) * spec * light.specular;

    // 衰减
    float distance    = length(light.position - FragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    //手电筒
    float theta     = dot(lightDir, normalize(-light.direction));
    //软化边缘所需变量
    float epsilon   = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);   
   
    return (ambient*attenuation + diffuse*attenuation*intensity + specular*attenuation*intensity)* lightColor;
}