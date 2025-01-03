#version 330 core

// 给光源数据一个结构体
struct Light {
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    vec3 direction;
    vec3 position;
};

// 给物体材质数据一个结构体
struct Material {
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;

    float shininess;
    float roughness;  // 粗糙度
    float metalness;  // 金属度
    float opacity;
};

in vec3 position;
in vec3 normal;
in vec2 texCoord;

// 相机坐标
uniform vec3 eye_position;
// 光源
uniform Light light;
// 物体材质
uniform Material material;
uniform mat4 view;

uniform int isShadow;
uniform float exposure; // 新增的曝光值

// 纹理数据
uniform sampler2D diffuseTexture;      // 基础颜色贴图
uniform sampler2D normalTexture;       // 法线贴图
uniform sampler2D specularTexture;     // 镜面反射贴图
uniform sampler2D metalnessTexture;    // 金属度贴图
uniform sampler2D roughnessTexture;    // 粗糙度贴图
uniform sampler2D ambientOcclusionTexture;      // 环境光遮蔽贴图

out vec4 fColor;

void main() {
    if (isShadow == 1) {
        fColor = vec4(0.0, 0.0, 0.0, 1.0);
    } else {
        // 采样贴图
        vec4 baseColor = texture(diffuseTexture, texCoord);
        baseColor = max(baseColor, vec4(0.1)); // 确保基础颜色不为0

        vec4 normalMap = texture(normalTexture, texCoord);
        vec4 specularMap = texture(specularTexture, texCoord);
        vec4 metalnessMap = texture(metalnessTexture, texCoord);
        vec4 roughnessMap = texture(roughnessTexture, texCoord);
        vec4 ambientMap = texture(ambientOcclusionTexture, texCoord);

        // 计算法线（使用法线贴图）
        vec3 tangentNormal = normalMap.xyz * 2.0 - 1.0; // 将法线从 [0, 1] 转换到 [-1, 1]
        vec3 Q1 = dFdx(position);
        vec3 Q2 = dFdy(position);
        vec2 st1 = dFdx(texCoord);
        vec2 st2 = dFdy(texCoord);

        vec3 N = normalize(normal);
        vec3 T = normalize(Q1 * st2.t - Q2 * st1.t);
        vec3 B = -normalize(cross(N, T));
        mat3 TBN = mat3(T, B, N);

        vec3 normal = normalize(TBN * tangentNormal); 

        // 计算光照向量（平行光方向固定，不需要计算距离）
        vec3 V = normalize(eye_position - position);
        vec3 L = normalize(-light.direction); // 平行光的方向
        vec3 R = normalize(reflect(-L, normal));

        // 环境光
        vec4 I_a = light.ambient * material.ambient * baseColor * ambientMap; // 使用环境光遮蔽贴图

        // 漫反射
        float diffuse_dot = max(dot(L, N), 0.0);
        vec4 I_d = diffuse_dot * light.diffuse * material.diffuse * baseColor;

        // 镜面反射
        float roughness = roughnessMap.r; // 从粗糙度贴图中获取粗糙度
        float specular_dot_pow = pow(max(dot(R, V), 0.0), material.shininess * (1.0 - roughness));
        float metalness = metalnessMap.r; // 从金属度贴图中获取金属度
        vec4 I_s = specular_dot_pow * light.specular * material.specular * metalness * specularMap;

        // 注意如果光源在背面则去除高光
        if (dot(L, normal) < 0.0) {
            I_s = vec4(0.0, 0.0, 0.0, 1.0);
        }

        // 合并三个分量的颜色，修正透明度
        fColor = I_a + I_d + I_s;
        fColor.a = material.opacity;

        // Tone Mapping
        vec4 toneMapped = vec4(1.0) - exp(-fColor * exposure); // 使用曝光映射

        // Gamma 校正
        fColor = pow(toneMapped, vec4(1.5 / 2.2));
    }
}