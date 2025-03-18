#include "Drone.h"
#include "core/gpu/shader.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cmath>
#include <iostream>

Drone::Drone() {
    // Initialize position and orientation
    position = glm::vec3(0, 1, 0);
    previousPosition = position;
    angleOy = 0.0f;
    forward = glm::vec3(0, 0, -1);
    right = glm::vec3(1, 0, 0);
    up = glm::vec3(0, 1, 0);
    propellerAngle = 0.0f;
}

Drone::~Drone() {}

void Drone::SetPosition(const glm::vec3 &pos) {
    position = pos;
}

void Drone::SetOrientation(float angleY) {
    angleOy = angleY;
    UpdateDirectionVectors();
}

void Drone::MoveForward(float distance) {
    previousPosition = position;
    position += forward * distance;
}

void Drone::MoveUp(float distance) {
    previousPosition = position;
    position += up * distance;
}

void Drone::MoveRight(float distance) {
    previousPosition = position;
    position += right * distance;
}

void Drone::Rotate(float angle) {
    angleOy += angle;
    UpdateDirectionVectors();
}

void Drone::RevertToPreviousPosition() {
    position = previousPosition;
}

void Drone::Update(float deltaTime) {
    // Update propeller rotation
    propellerAngle += deltaTime * 10.0f;
    if (propellerAngle > 2 * M_PI) {
        propellerAngle -= 2 * M_PI;
    }
}

void Drone::UpdateDirectionVectors() {
    // Update direction based on current orientation
    glm::mat4 rot = glm::rotate(glm::mat4(1.0f), angleOy, glm::vec3(0,1,0));
    forward = glm::normalize(glm::vec3(rot * glm::vec4(0,0,-1,0)));
    right = glm::normalize(glm::cross(forward, glm::vec3(0,1,0)));
    up = glm::normalize(glm::cross(right, forward));
}

void Drone::DrawDrone(const std::unordered_map<std::string, Mesh*>& meshes,
                      Shader* shader,
                      std::function<void(Mesh*, Shader*, const glm::mat4&)> RenderMesh3D)
{
    if (!shader || !shader->program) return;
    if (meshes.find("box") == meshes.end()) return;

    // Base transformation
    glm::mat4 modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, position);
    modelMatrix = glm::rotate(modelMatrix, angleOy, glm::vec3(0,1,0));

    // Set drone color
    GLint colorLoc = glGetUniformLocation(shader->program, "object_color");
    glUniform4f(colorLoc, 0.5f, 0.5f, 0.5f, 1.0f);

    // Render arms
    glm::mat4 arm1 = glm::scale(modelMatrix, glm::vec3(2.0f, 0.1f, 0.1f));
    RenderMesh3D(meshes.at("box"), shader, arm1);

    glm::mat4 arm2 = glm::rotate(modelMatrix, glm::radians(90.0f), glm::vec3(0,1,0));
    arm2 = glm::scale(arm2, glm::vec3(2.0f, 0.1f, 0.1f));
    RenderMesh3D(meshes.at("box"), shader, arm2);

    // Define propeller positions
    float armLength = 1.0f;
    glm::vec3 ends[4] = {
        glm::vec3( armLength, 0, 0),
        glm::vec3(-armLength, 0, 0),
        glm::vec3(0, 0,  armLength),
        glm::vec3(0, 0, -armLength)
    };

    // Render propellers
    for (int i = 0; i < 4; i++) {
        // Set arm color
        glUniform4f(colorLoc, 0.5f, 0.5f, 0.5f, 1.0f);
        glm::mat4 cubeM = glm::translate(modelMatrix, ends[i] + glm::vec3(0, 0.07f, 0));
        cubeM = glm::scale(cubeM, glm::vec3(0.2f));
        RenderMesh3D(meshes.at("box"), shader, cubeM);

        // Set propeller color
        glUniform4f(colorLoc, 0.0f, 0.0f, 0.0f, 1.0f);
        glm::mat4 propM = glm::translate(modelMatrix, ends[i] + glm::vec3(0, 0.2f, 0));
        propM = glm::rotate(propM, propellerAngle, glm::vec3(0,1,0));
        propM = glm::scale(propM, glm::vec3(0.6f, 0.01f, 0.1f));
        RenderMesh3D(meshes.at("box"), shader, propM);
    }
}

glm::mat4 Drone::GetModelMatrix() const {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    model = glm::rotate(model, angleOy, glm::vec3(0,1,0));
    return model;
}
