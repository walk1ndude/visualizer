#version 410
uniform highp sampler2D points;

uniform highp int pointsCount;

uniform highp mat4 model;
uniform highp mat4 view;
uniform highp mat4 scale;

uniform highp mat4 mvp;

struct Material {
    vec4 emissive;
    vec4 diffuse;
    vec4 specular;
    float shininess;
};

uniform highp Material material;

struct LightSource {
    vec4 position;
    vec4 color;
    float ambientIntensity;
    float attenuation;
};

uniform highp LightSource lightSource;

struct Ranges {
   vec2 xRange;
   vec2 yRange;
   vec2 zRange;
};

uniform highp Ranges ranges;

uniform highp vec4 cameraPosition;

bool needToRender(const vec3 position,
                  const vec2 xab, const vec2 yab, const vec2 zab);

vec4 calcFragColor(const vec4 position, const vec4 normal, const vec4 color,
                   const vec3 positionModel);

vec4 highlightColor(const vec3 position, const vec4 sColor);

bool needToRender(const vec3 position,
                  const vec2 xab, const vec2 yab, const vec2 zab) {
    return position.x >= ranges.xRange.s * xab.s + xab.t
           && position.x <= ranges.xRange.t * xab.s + xab.t
           && position.y >= ranges.yRange.s * yab.s + yab.t
           && position.y <= ranges.yRange.t * yab.s + yab.t
           && position.z >= ranges.zRange.s * zab.s + zab.t
           && position.z <= ranges.zRange.t * zab.s + zab.t;
}


vec4 calcFragColor(const vec4 position, const vec4 normal, const vec4 color,
                   const vec3 positionModel) {

    vec4 surfaceColor = highlightColor(positionModel, color);
    vec4 lightPos = view * lightSource.position;

    vec4 surfaceToLight = normalize(lightPos - position);
    vec4 surfaceToCamera = normalize(cameraPosition - position);

    vec4 ambient = lightSource.ambientIntensity * surfaceColor * lightSource.color;

    //diffuse
    float diffuseCoefficient = max(0.0f, dot(normal, surfaceToLight));
    vec4 diffuse = diffuseCoefficient * surfaceColor * lightSource.color;

    //specular
    float specularCoefficient = 0.0f;
    if (diffuseCoefficient > 0.0f) {
        specularCoefficient = pow(max(0.0f, dot(surfaceToCamera, reflect(- surfaceToLight, normal))), material.shininess);
    }
    vec4 specular = specularCoefficient * material.specular * lightSource.color;

    //attenuation
    float distanceToLight = length(lightPos - position);
    float attenuation = 1.0f / (1.0f + lightSource.attenuation * pow(distanceToLight, 2));

    //linear color (color before gamma correction)
    vec4 linearColor = ambient + attenuation * (diffuse + specular);

    //final color (after gamma correction)
    vec4 gamma = vec4(1.0f / 2.2f);

    return pow(linearColor, gamma);
}


vec4 highlightColor(const vec3 position, const vec4 sColor) {
    vec4 color = vec4(0.0f);

    vec4 pointPos;
    float i = 1.0f;
    
    if (pointsCount == 0) {
        return sColor;
    }
    
    float step = 1.0f / pointsCount;
    
    while (i >= 0.0f) {
        // x, y, z - point coords, w - color radius
        pointPos = texture(points, vec2(0.0f, i));

        if (length(position - pointPos.xyz) < pointPos.w * pointPos.w) {
            color += texture(points, vec2(0.5f, i));
        }
        
        i -= step;
    }

    return color;
}
