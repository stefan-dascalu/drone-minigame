#pragma once

#include "components/simple_scene.h"
#include "Drone.h"
#include "lab_m1/Tema2/cameras.h"
#include <vector>
#include <memory>

// Structure to represent a tree
struct Tree {
    glm::vec3 position;
    float scale;
};

// Structure to represent a rock
struct Rock {
    glm::vec3 position;
    float scale;
};

namespace m1
{
    class Tema2 : public gfxc::SimpleScene
    {
    public:
        Tema2();
        ~Tema2();

        void Init() override;

    private:
        // Core loop methods
        void FrameStart() override;
        void Update(float deltaTimeSeconds) override;
        void FrameEnd() override;

        // Input handling
        void OnInputUpdate(float deltaTime, int mods) override;
        void OnKeyPress(int key, int mods) override;
        void OnKeyRelease(int key, int mods) override;
        void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
        void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
        void OnWindowResize(int width, int height) override;

        // Rendering methods
        void RenderScene(float deltaTimeSeconds);
        void RenderTerrain();
        void RenderTrees();
        void RenderRocks();

        // Mesh creation
        Mesh* CreateCubeMesh(const std::string& name);
        Mesh* CreateTerrainMesh();
        Mesh* CreateLineMesh(const std::string& name);

        // Environment generation
        void GenerateTrees(int count);
        void GenerateRocks(int count);

        // Utility methods
        void UpdateCamera();
        float GetTerrainHeightAt(float x, float z);

    private:
        Drone drone;
        implemented::Cameras* camera;
        Mesh* terrainMesh;
        Shader* basicShader;
        Shader* terrainShader; 
        glm::mat4 projectionMatrix;
        std::vector<Tree> trees;
        std::vector<Rock> rocks;
    };
}
