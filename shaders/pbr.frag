#version 330 core

in vec2 vTexCoords;
in vec3 vWorldPos;
in vec3 vNormal;

out vec4 FragColor;

// material parameters
uniform vec3  albedo;
uniform float metallic;
uniform float roughness;
uniform float ao;

// lights
uniform vec3 lightPos[4];
uniform vec3 lightColor[4];

uniform vec3 camPos;

const float PI = 3.14159265359;

float DistributionGGX(vec3 N, vec3 H, float roughness);
float GeometrySchlickGGX(float NdotV, float roughness);
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness);
vec3 FresnelSchlick(float cosTheta, vec3 F0);

void main()
{
    vec3 N = normalize(vNormal);
    vec3 V = normalize(camPos - vWorldPos);

    // Pre-compute F0 (reflectance at normal incident angle) for both
    // dielectric/insulator materials (non-metal) and conductors (metals).
    // Use 0.04 for dielectric (average for most materials) and the albedo color for metal
    vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedo, metallic);

    // Reflectance equation (there are 4 point light sources in the scene)
    vec3 L_out = vec3(0.0);
    for(int i = 0; i < 4; ++i) {

        // Calculate per-light radiance
        vec3 L = normalize(lightPos[i] - vWorldPos);
        vec3 H = normalize(V + L);
        float distance    = length(lightPos[i] - vWorldPos);
        float attenuation = 1.0 / (distance * distance); // Inverse-square law
        vec3 radiance     = lightColor[i] * attenuation;

        // Cook-Torrance BRDF
        float D = DistributionGGX(N, H, roughness);
        vec3 F  = FresnelSchlick(max(dot(H, V), 0.0), F0);
        float G = GeometrySmith(N, V, L, roughness);

        vec3 numerator    = D * G * F;
        float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0);
        vec3 specular     = numerator / max(denominator, 0.001);

        // Fresnel basically is the specular factor
        vec3 kS = F;
        // For energy conservation, the diffuse and specular light can't
        // be above 1.0 (unless the surface emits light); to preserve this
        // relationship the diffuse component (kD) should equal 1.0 - kS.
        vec3 kD = vec3(1.0) - kS;
        // Multiply kD by the inverse metalness such that only non-metals
        // have diffuse lighting, or a linear blend if partly metal (pure metals
        // have no diffuse light).
        kD *= 1.0 - metallic;

        // Add to outgoing radiance L_out
        float NdotL = max(dot(N, L), 0.0);
        L_out += (kD * albedo / PI + specular) * radiance * NdotL;
    }

    // "Improvised" ambient light
    // (note that the next IBL tutorial will replace this ambient lighting with environment lighting).
    vec3 ambient = vec3(0.03) * albedo * ao;
    vec3 color = ambient + L_out;

    // HDR tonemapping
    color = color / (color + vec3(1.0));
    // Gamma correction (Reinhard operator)
    color = pow(color, vec3(1.0/2.2));

    FragColor = vec4(color, 1.0);
}


// D = Normal distribution function (Trowbridge-Reitx GGX)
float DistributionGGX(vec3 N, vec3 H, float roughness) {
    float a      = roughness*roughness;
    float a2     = a*a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom    = a2;
    float denom  = (NdotH2 * (a2 - 1.0) + 1.0);
    denom        = PI * denom * denom;

    // Prevent divide by zero for roughness=0.0 and NdotH=1.0
    return nom / max(denom, 0.001);
}

// G_sub - Geometry sub-function (Schlick-GGX)
float GeometrySchlickGGX(float NdotV, float roughness) {
    float r = roughness + 1.0;
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}

// G - Geometry function (Smith's method)
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness) {
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx1 = GeometrySchlickGGX(NdotV, roughness);
    float ggx2 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

// F - Fresnel equation (Fresnel-Schilck approximation)
// cosTheta is the dot product between surface normal and view direction (n dot v)
vec3 FresnelSchlick(float cosTheta, vec3 F0) {
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}
