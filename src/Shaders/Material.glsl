
struct Material{
    vec3 albedo;
    float pad0;
    vec3 emission;
    float pad1;
    float roughness;
    float metallic;
    float transmission;
    float ior;
    uint64_t albedoTexture;
    uint64_t metallicRoughnessTexture;
    uint64_t emissionTexture;
    uint64_t normalTexture;
};