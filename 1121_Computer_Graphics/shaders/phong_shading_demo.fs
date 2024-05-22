#version 330 core

in vec3 position;
in vec2 texcoord;
in vec3 normal;

uniform vec3 cameraPos;
uniform vec3 Ka;
uniform vec3 Kd;
uniform vec3 Ks;
uniform sampler2D mapKd;
uniform bool useMapKd;
uniform float Ns;
uniform vec3 ambientLight;
uniform vec3 dirLightDir;
uniform vec3 dirLightRadiance;
uniform vec3 pointLightPos;
uniform vec3 pointLightIntensity;
uniform vec3 spotLightPos;
uniform vec3 spotLightIntensity;
uniform vec3 spotLightDir;
uniform float spotLightTotalWidthCos;
uniform float spotLightEpsilon;

out vec4 FragColor;

vec3 diffuseAndSpecular(const vec3, const vec3, const float, const vec3, const vec3, const vec3, const vec3);
vec3 diffuseAndSpecularAndDistanceAttenuation(const vec3, const vec3, const float, const vec3, const vec3, const vec3, const vec3, const float);

void main()
{
    vec3 normal = normalize(normal);
    vec3 viewDir = normalize(cameraPos - position);
    vec3 textureKd = useMapKd ? texture2D(mapKd, texcoord).rgb : Kd;

    // Ambient light.
    vec3 ambient = Ka * ambientLight;

    // Direction light.
    vec3 directionLight = diffuseAndSpecular(
        textureKd, Ks, Ns,
        dirLightRadiance, 
        normalize(-dirLightDir), 
        viewDir, 
        normal
    );

    // Point light.
    vec3 pointLight = diffuseAndSpecularAndDistanceAttenuation(
        textureKd, Ks, Ns, 
        pointLightIntensity, 
        normalize(pointLightPos - position), 
        viewDir, 
        normal,
        distance(pointLightPos, position)
    );

    // Spot light.
    vec3 spotLightenedDir = normalize(spotLightPos - position);
    vec3 spotLight = diffuseAndSpecularAndDistanceAttenuation(
        textureKd, Ks, Ns, 
        (spotLightIntensity * clamp((dot(-spotLightDir, spotLightenedDir) - spotLightTotalWidthCos) / spotLightEpsilon, 0.0, 1.0)), 
        spotLightenedDir,
        viewDir, 
        normal, 
        distance(spotLightPos, position)
    );
    
    // Total lights.
    vec3 lights = ambient + directionLight + pointLight + spotLight;

    FragColor = vec4(lights, 1.0);
}

vec3 diffuseAndSpecular(
    const vec3 Kd, 
    const vec3 Ks, 
    const float Ns, 
    const vec3 intensity, 
    const vec3 lightDir, 
    const vec3 viewDir, 
    const vec3 normal
) { return intensity * (Kd * max(dot(normal, lightDir), 0.0) + Ks * pow(max(dot(normalize(lightDir + viewDir), normal), 0.0), Ns)); }

vec3 diffuseAndSpecularAndDistanceAttenuation(
    const vec3 Kd, 
    const vec3 Ks, 
    const float Ns, 
    const vec3 intensity, 
    const vec3 lightDir, 
    const vec3 viewDir, 
    const vec3 normal, 
    const float distance
) { return diffuseAndSpecular(Kd, Ks, Ns, intensity, lightDir, viewDir, normal) / (distance * distance); }