#version 410
layout(location = 0) in highp vec4 pos;
layout(location = 1) in highp vec3 vertexTest;
layout(location = 2) in highp vec4 N;

uniform highp vec4 colorU;

struct Ranges {
   vec2 xRange;
   vec2 yRange;
   vec2 zRange;
};

uniform highp Ranges ranges;

struct Material {
    vec4 emissive;
    vec4 diffuse;
    vec4 specular;
    float shininess;
};

uniform highp Material stlMaterial;

struct LightSource {
    vec4 position;
    vec4 color;
    float ambientIntensity;
};

uniform highp LightSource lightSource;

layout(location = 0) out highp vec4 fragColor;


bool needToRender(const vec3 point,
                  const vec2 xRange, const vec2 yRange, const vec2 zRange,
                  const vec2 xab, const vec2 yab, const vec2 zab);

vec4 highlightColor(const vec3 position);


void main(void) {
    if (needToRender(vertexTest, ranges.xRange, ranges.yRange, ranges.zRange,
                     vec2(1.0, 0.0), vec2(1.0, 0.0), vec2(1.0, 0.0))) {

        vec4 L = normalize(lightSource.position - pos);

        float NdotL = max(dot(N, L), 0);
        vec4 diffuse =  NdotL * lightSource.color * stlMaterial.diffuse;

        vec4 V = normalize(N - pos);
        vec4 H = normalize(L + V);
        vec4 R = reflect(-L, N);

        float RdotV = max(dot(R, V), 0);
        float NdotH = max(dot(N, H), 0);

        vec4 specular = pow(RdotV, stlMaterial.shininess) * lightSource.color * stlMaterial.specular;

        vec4 hColor = highlightColor(vec3(pos));
        
        if (hColor != vec4(0.0)) {
            fragColor = hColor;
            return;
        }
        
        fragColor = (hColor != vec4(0.0)) ? hColor : colorU;
        
        fragColor = (stlMaterial.emissive + lightSource.ambientIntensity + diffuse + specular) * fragColor;
/*
        if (fragColor.r > 0.75 && fragColor.r < 0.85) {
        //if (headColor.r > 0.82) {//&& fragPos.p < 0.8 && fragPos.s > 0.33 && fragPos.s < 0.67) {
            fragColor.a *= (0.08 * fragColor.r);
        }
        else {
            fragColor.a *= 0.2;
        }*/
    }
    else {
        discard;
    }
}
