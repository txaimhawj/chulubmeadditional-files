#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../core/ecs.h"

namespace CHULUBME {

// Forward declarations
class Shader;
class Texture;
class Mesh;
class Material;
class Camera;
class RenderSystem;

/**
 * @brief Renderer class that manages the rendering pipeline
 */
class Renderer {
public:
    // Singleton instance
    static Renderer& Instance();

    // Initialize the renderer
    bool Initialize(int width, int height);

    // Shutdown the renderer
    void Shutdown();

    // Begin frame rendering
    void BeginFrame();

    // End frame rendering
    void EndFrame();

    // Set the main camera
    void SetMainCamera(Camera* camera) { m_mainCamera = camera; }

    // Get the main camera
    Camera* GetMainCamera() const { return m_mainCamera; }

    // Get the window width
    int GetWidth() const { return m_width; }

    // Get the window height
    int GetHeight() const { return m_height; }

    // Resize the renderer
    void Resize(int width, int height);

    // Create a shader
    std::shared_ptr<Shader> CreateShader(const std::string& vertexSource, const std::string& fragmentSource);

    // Create a texture
    std::shared_ptr<Texture> CreateTexture(const std::string& filename);

    // Create a mesh
    std::shared_ptr<Mesh> CreateMesh(const std::vector<float>& vertices, const std::vector<unsigned int>& indices);

    // Create a material
    std::shared_ptr<Material> CreateMaterial(std::shared_ptr<Shader> shader);

    // Draw a mesh with a material
    void DrawMesh(std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material, const glm::mat4& transform);

private:
    // Private constructor for singleton
    Renderer();
    ~Renderer();

    // Deleted copy constructor and assignment operator
    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;

    // Window dimensions
    int m_width;
    int m_height;

    // Main camera
    Camera* m_mainCamera;

    // Resource caches
    std::unordered_map<std::string, std::shared_ptr<Shader>> m_shaderCache;
    std::unordered_map<std::string, std::shared_ptr<Texture>> m_textureCache;

    // Friend classes
    friend class RenderSystem;
};

/**
 * @brief Shader class for GPU programs
 */
class Shader {
public:
    Shader(const std::string& vertexSource, const std::string& fragmentSource);
    ~Shader();

    // Bind the shader for rendering
    void Bind() const;

    // Unbind the shader
    void Unbind() const;

    // Set uniform values
    void SetBool(const std::string& name, bool value) const;
    void SetInt(const std::string& name, int value) const;
    void SetFloat(const std::string& name, float value) const;
    void SetVec2(const std::string& name, const glm::vec2& value) const;
    void SetVec3(const std::string& name, const glm::vec3& value) const;
    void SetVec4(const std::string& name, const glm::vec4& value) const;
    void SetMat3(const std::string& name, const glm::mat3& value) const;
    void SetMat4(const std::string& name, const glm::mat4& value) const;

private:
    // Shader program ID
    unsigned int m_id;

    // Compile and link shader
    bool Compile(const std::string& vertexSource, const std::string& fragmentSource);
};

/**
 * @brief Texture class for 2D images
 */
class Texture {
public:
    Texture(const std::string& filename);
    ~Texture();

    // Bind the texture for rendering
    void Bind(unsigned int slot = 0) const;

    // Unbind the texture
    void Unbind() const;

    // Get texture dimensions
    int GetWidth() const { return m_width; }
    int GetHeight() const { return m_height; }

private:
    // Texture ID
    unsigned int m_id;

    // Texture dimensions
    int m_width;
    int m_height;

    // Texture format
    int m_channels;
};

/**
 * @brief Mesh class for 3D geometry
 */
class Mesh {
public:
    Mesh(const std::vector<float>& vertices, const std::vector<unsigned int>& indices);
    ~Mesh();

    // Bind the mesh for rendering
    void Bind() const;

    // Unbind the mesh
    void Unbind() const;

    // Draw the mesh
    void Draw() const;

private:
    // Vertex Array Object
    unsigned int m_vao;

    // Vertex Buffer Object
    unsigned int m_vbo;

    // Element Buffer Object
    unsigned int m_ebo;

    // Number of indices
    unsigned int m_indexCount;
};

/**
 * @brief Material class for surface properties
 */
class Material {
public:
    Material(std::shared_ptr<Shader> shader);
    ~Material() = default;

    // Bind the material for rendering
    void Bind() const;

    // Unbind the material
    void Unbind() const;

    // Get the shader
    std::shared_ptr<Shader> GetShader() const { return m_shader; }

    // Set texture
    void SetTexture(const std::string& name, std::shared_ptr<Texture> texture);

    // Set properties
    void SetColor(const std::string& name, const glm::vec4& color);
    void SetFloat(const std::string& name, float value);
    void SetInt(const std::string& name, int value);
    void SetBool(const std::string& name, bool value);
    void SetVec2(const std::string& name, const glm::vec2& value);
    void SetVec3(const std::string& name, const glm::vec3& value);
    void SetVec4(const std::string& name, const glm::vec4& value);
    void SetMat3(const std::string& name, const glm::mat3& value);
    void SetMat4(const std::string& name, const glm::mat4& value);

private:
    // Shader
    std::shared_ptr<Shader> m_shader;

    // Textures
    std::unordered_map<std::string, std::shared_ptr<Texture>> m_textures;

    // Properties
    std::unordered_map<std::string, glm::vec4> m_colors;
    std::unordered_map<std::string, float> m_floats;
    std::unordered_map<std::string, int> m_ints;
    std::unordered_map<std::string, bool> m_bools;
    std::unordered_map<std::string, glm::vec2> m_vec2s;
    std::unordered_map<std::string, glm::vec3> m_vec3s;
    std::unordered_map<std::string, glm::vec4> m_vec4s;
    std::unordered_map<std::string, glm::mat3> m_mat3s;
    std::unordered_map<std::string, glm::mat4> m_mat4s;
};

/**
 * @brief Camera component for viewing the scene
 */
class Camera : public Component {
public:
    Camera(float fov = 45.0f, float aspectRatio = 16.0f / 9.0f, float nearPlane = 0.1f, float farPlane = 1000.0f);
    ~Camera() = default;

    // Initialize the camera
    void Initialize() override;

    // Get the view matrix
    glm::mat4 GetViewMatrix() const;

    // Get the projection matrix
    glm::mat4 GetProjectionMatrix() const;

    // Set camera position
    void SetPosition(const glm::vec3& position) { m_position = position; }

    // Get camera position
    glm::vec3 GetPosition() const { return m_position; }

    // Set camera rotation (Euler angles in degrees)
    void SetRotation(const glm::vec3& rotation);

    // Get camera rotation (Euler angles in degrees)
    glm::vec3 GetRotation() const { return m_rotation; }

    // Set camera field of view
    void SetFOV(float fov) { m_fov = fov; UpdateProjectionMatrix(); }

    // Get camera field of view
    float GetFOV() const { return m_fov; }

    // Set camera aspect ratio
    void SetAspectRatio(float aspectRatio) { m_aspectRatio = aspectRatio; UpdateProjectionMatrix(); }

    // Get camera aspect ratio
    float GetAspectRatio() const { return m_aspectRatio; }

    // Set camera near plane
    void SetNearPlane(float nearPlane) { m_nearPlane = nearPlane; UpdateProjectionMatrix(); }

    // Get camera near plane
    float GetNearPlane() const { return m_nearPlane; }

    // Set camera far plane
    void SetFarPlane(float farPlane) { m_farPlane = farPlane; UpdateProjectionMatrix(); }

    // Get camera far plane
    float GetFarPlane() const { return m_farPlane; }

private:
    // Update the projection matrix
    void UpdateProjectionMatrix();

    // Camera position and rotation
    glm::vec3 m_position;
    glm::vec3 m_rotation;

    // Camera front, right, and up vectors
    glm::vec3 m_front;
    glm::vec3 m_right;
    glm::vec3 m_up;

    // Camera projection parameters
    float m_fov;
    float m_aspectRatio;
    float m_nearPlane;
    float m_farPlane;

    // Camera matrices
    glm::mat4 m_viewMatrix;
    glm::mat4 m_projectionMatrix;
};

/**
 * @brief Transform component for entity positioning
 */
class Transform : public Component {
public:
    Transform(const glm::vec3& position = glm::vec3(0.0f), 
              const glm::vec3& rotation = glm::vec3(0.0f), 
              const glm::vec3& scale = glm::vec3(1.0f));
    ~Transform() = default;

    // Initialize the transform
    void Initialize() override;

    // Set position
    void SetPosition(const glm::vec3& position) { m_position = position; m_dirty = true; }

    // Get position
    glm::vec3 GetPosition() const { return m_position; }

    // Set rotation (Euler angles in degrees)
    void SetRotation(const glm::vec3& rotation) { m_rotation = rotation; m_dirty = true; }

    // Get rotation (Euler angles in degrees)
    glm::vec3 GetRotation() const { return m_rotation; }

    // Set scale
    void SetScale(const glm::vec3& scale) { m_scale = scale; m_dirty = true; }

    // Get scale
    glm::vec3 GetScale() const { return m_scale; }

    // Get the model matrix
    glm::mat4 GetModelMatrix();

private:
    // Update the model matrix if dirty
    void UpdateModelMatrix();

    // Transform properties
    glm::vec3 m_position;
    glm::vec3 m_rotation;
    glm::vec3 m_scale;

    // Model matrix
    glm::mat4 m_modelMatrix;

    // Dirty flag for matrix recalculation
    bool m_dirty;
};

/**
 * @brief MeshRenderer component for rendering meshes
 */
class MeshRenderer : public Component {
public:
    MeshRenderer(std::shared_ptr<Mesh> mesh = nullptr, std::shared_ptr<Material> material = nullptr);
    ~MeshRenderer() = default;

    // Initialize the mesh renderer
    void Initialize() override;

    // Finalize the mesh renderer
    void Finalize() override;

    // Set mesh
    void SetMesh(std::shared_ptr<Mesh> mesh) { m_mesh = mesh; }

    // Get mesh
    std::shared_ptr<Mesh> GetMesh() const { return m_mesh; }

    // Set material
    void SetMaterial(std::shared_ptr<Material> material) { m_material = material; }

    // Get material
    std::shared_ptr<Material> GetMaterial() const { return m_material; }

private:
    // Mesh and material
    std::shared_ptr<Mesh> m_mesh;
    std::shared_ptr<Material> m_material;

    // Friend classes
    friend class RenderSystem;
};

/**
 * @brief RenderSystem for rendering all mesh renderers
 */
class RenderSystem : public System {
public:
    RenderSystem(EntityManager* manager);
    ~RenderSystem() = default;

    // Initialize the render system
    void Initialize() override;

    // Update the render system
    void Update(float deltaTime) override;

    // Render all mesh renderers
    void Render() override;

private:
    // Renderer reference
    Renderer& m_renderer;
};

} // namespace CHULUBME
