#pragma once

#include <glm/glm.hpp>
#include <unordered_map>
#include <functional>
#include <string>
#include "core/gpu/mesh.h"

class Shader;

class Drone {
public:
    Drone();
    ~Drone();

    // Position and orientation controls
    void SetPosition(const glm::vec3 &pos);
    void SetOrientation(float angleY);
    void MoveForward(float distance);
    void MoveUp(float distance);
    void MoveRight(float distance);
    void Rotate(float angle);
    void RevertToPreviousPosition();

    // Update state
    void Update(float deltaTime);

    // Render drone
    void DrawDrone(const std::unordered_map<std::string, Mesh*>& meshes,
                  Shader* shader,
                  std::function<void(Mesh*, Shader*, const glm::mat4&)> RenderMesh3D);

    // Getters
    glm::mat4 GetModelMatrix() const;
    glm::vec3 GetPosition() const { return position; }
    float GetOrientation() const { return angleOy; }
    glm::vec3 GetForward() const { return forward; }
    glm::vec3 GetRight() const { return right; }
    glm::vec3 GetUp() const { return up; }
    glm::vec3 GetPreviousPosition() const { return previousPosition; }
    float GetRadius() const { return 1.0f; }
    float GetMinHeightAboveTerrain() const { return 1.0f; }
    float GetMaxHeightAboveTerrain() const { return 20.0f; }

private:
    glm::vec3 position;
    glm::vec3 previousPosition;
    float angleOy; // Y-axis rotation
    glm::vec3 forward;
    glm::vec3 right;
    glm::vec3 up;
    float propellerAngle;

    // Update direction vectors based on orientation
    void UpdateDirectionVectors();
};
