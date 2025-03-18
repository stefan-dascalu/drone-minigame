#include "Tema2.h"
#include "core/gpu/shader.h"
#include "core/engine.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <random>
#include <iostream>

using namespace m1;

Tema2::Tema2() {}

Tema2::~Tema2() {
    delete camera;
}

void Tema2::Init() {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glEnable(GL_DEPTH_TEST);

    camera = new implemented::Cameras();

    Mesh* box = CreateCubeMesh("box");
    meshes["box"] = box;

    Mesh* sphere = new Mesh("sphere");
    if (!sphere->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "sphere.obj")) {
        std::cerr << "Failed to load sphere mesh" << std::endl;
    }
    meshes["sphere"] = sphere;

    Mesh* cylinder = new Mesh("cylinder");
    if (!cylinder->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "quad.obj")) {
        std::cerr << "Failed to load cylinder mesh" << std::endl;
    }
    meshes["cylinder"] = cylinder;

    terrainMesh = CreateTerrainMesh();
    meshes["terrain"] = terrainMesh;

    meshes["axes_line"] = CreateLineMesh("axes_line");

    basicShader = new Shader("VertexColor");
    basicShader->AddShader(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::SHADERS, "VertexColor.glsl"), GL_VERTEX_SHADER);
    basicShader->AddShader(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::SHADERS, "FragmentColor.glsl"), GL_FRAGMENT_SHADER);
    if (!basicShader->CreateAndLink()) {
        std::cerr << "Failed to create and link VertexColor shader" << std::endl;
    }
    shaders["VertexColor"] = basicShader;

    terrainShader = new Shader("TerrainShader");
    terrainShader->AddShader(PATH_JOIN(window->props.selfDir, "src", "lab_m1", "Tema2", "TerrainVertexShader.glsl"), GL_VERTEX_SHADER);
    terrainShader->AddShader(PATH_JOIN(window->props.selfDir, "src", "lab_m1", "Tema2", "TerrainFragmentShader.glsl"), GL_FRAGMENT_SHADER);
    if (!terrainShader->CreateAndLink()) {
        std::cerr << "Failed to create and link TerrainShader" << std::endl;
    }
    shaders["TerrainShader"] = terrainShader;

    projectionMatrix = glm::perspective(glm::radians(60.0f), window->props.aspectRatio, 0.1f, 200.0f);

    GenerateTrees(10);
    GenerateRocks(10);
}

void Tema2::FrameStart() {
    glm::ivec2 resolution = window->GetResolution();
    glClearColor(0.5f, 0.8f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, resolution.x, resolution.y);
}

void Tema2::Update(float deltaTimeSeconds) {
    drone.Update(deltaTimeSeconds);
    UpdateCamera();

    glm::vec3 dronePos = drone.GetPosition();
    float terrainHeight = GetTerrainHeightAt(dronePos.x, dronePos.z);
    float minY = glm::max(terrainHeight + drone.GetMinHeightAboveTerrain(), 1.0f);
    
    if (dronePos.y < minY) {
        glm::vec3 newPos = glm::vec3(dronePos.x, minY, dronePos.z);
        drone.SetPosition(glm::mix(dronePos, newPos, 0.1f));
    }

    RenderScene(deltaTimeSeconds);
}

void Tema2::FrameEnd() {}

void Tema2::OnInputUpdate(float deltaTime, int mods) {
    float movementSpeed = 8.0f * deltaTime;
    float rotationSpeed = glm::radians(90.0f) * deltaTime;

    if (window->KeyHold(GLFW_KEY_W)) {
        drone.MoveForward(movementSpeed);
    }
    if (window->KeyHold(GLFW_KEY_S)) {
        drone.MoveForward(-movementSpeed);
    }
    if (window->KeyHold(GLFW_KEY_A)) {
        drone.MoveRight(-movementSpeed);
    }
    if (window->KeyHold(GLFW_KEY_D)) {
        drone.MoveRight(movementSpeed);
    }
    if (window->KeyHold(GLFW_KEY_Q)) {
        drone.MoveUp(movementSpeed);
    }
    if (window->KeyHold(GLFW_KEY_E)) {
        drone.MoveUp(-movementSpeed);
    }
    if (window->KeyHold(GLFW_KEY_R)) {
        drone.Rotate(rotationSpeed);
    }
    if (window->KeyHold(GLFW_KEY_T)) {
        drone.Rotate(-rotationSpeed);
    }
}

void Tema2::OnKeyPress(int key, int mods) {}
void Tema2::OnKeyRelease(int key, int mods) {}
void Tema2::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) {}
void Tema2::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) {}
void Tema2::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) {}
void Tema2::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) {}

void Tema2::OnWindowResize(int width, int height) {
    projectionMatrix = glm::perspective(glm::radians(60.0f), static_cast<float>(width) / height, 0.1f, 200.0f);
}

Mesh* Tema2::CreateLineMesh(const std::string& name) {
    std::vector<VertexFormat> vertices = {
        VertexFormat(glm::vec3(0,0,0), glm::vec3(1,1,1)),
        VertexFormat(glm::vec3(1,0,0), glm::vec3(1,1,1))
    };
    std::vector<unsigned int> indices = {0,1};

    Mesh* line = new Mesh(name);
    line->InitFromData(vertices, indices);
    line->SetDrawMode(GL_LINES);
    return line;
}

Mesh* Tema2::CreateTerrainMesh() {
    const int gridSize = 100;
    const float gridStep = 1.0f;

    std::vector<VertexFormat> vertices;
    std::vector<unsigned int> indices;

    for (int i = -gridSize; i <= gridSize; ++i) {
        for (int j = -gridSize; j <= gridSize; ++j) {
            vertices.emplace_back(glm::vec3(i * gridStep, 0, j * gridStep), glm::vec3(0, 1, 0));
        }
    }

    int dim = 2 * gridSize + 1;
    for (int i = 0; i < dim - 1; ++i) {
        for (int j = 0; j < dim - 1; ++j) {
            int start = i * dim + j;
            indices.push_back(start);
            indices.push_back(start + 1);
            indices.push_back(start + dim);

            indices.push_back(start + 1);
            indices.push_back(start + dim + 1);
            indices.push_back(start + dim);
        }
    }

    Mesh* terrain = new Mesh("terrain");
    terrain->InitFromData(vertices, indices);
    return terrain;
}

void Tema2::GenerateTrees(int count) {
    std::mt19937 rng(std::random_device{}());
    std::uniform_real_distribution<float> distPos(-50.0f, 50.0f);
    std::uniform_real_distribution<float> distScale(0.5f, 2.0f);

    for (int i = 0; i < count; ++i) {
        Tree t;
        bool placed = false;
        while (!placed) {
            float x = distPos(rng);
            float z = distPos(rng);
            float y = GetTerrainHeightAt(x, z);
            t.position = glm::vec3(x, y, z);
            t.scale = distScale(rng);

            bool overlap = false;
            for (const auto& other : trees) {
                float distanceXZ = glm::distance(glm::vec2(t.position.x, t.position.z), glm::vec2(other.position.x, other.position.z));
                if (distanceXZ < 2.0f * (t.scale + other.scale)) {
                    overlap = true;
                    break;
                }
            }

            if (!overlap) {
                trees.push_back(t);
                placed = true;
            }
        }
    }
}

void Tema2::GenerateRocks(int count) {
    std::mt19937 rng(std::random_device{}());
    std::uniform_real_distribution<float> distPos(-50.0f, 50.0f);
    std::uniform_real_distribution<float> distScale(0.3f, 1.5f);

    for (int i = 0; i < count; ++i) {
        Rock r;
        bool placed = false;
        while (!placed) {
            float x = distPos(rng);
            float z = distPos(rng);
            float y = GetTerrainHeightAt(x, z);
            r.position = glm::vec3(x, y, z);
            r.scale = distScale(rng);

            bool overlap = false;
            for (const auto& tree : trees) {
                float distanceXZ = glm::distance(glm::vec2(r.position.x, r.position.z), glm::vec2(tree.position.x, tree.position.z));
                if (distanceXZ < (r.scale + tree.scale)) {
                    overlap = true;
                    break;
                }
            }

            if (!overlap) {
                for (const auto& other : rocks) {
                    float distanceXZ = glm::distance(glm::vec2(r.position.x, r.position.z), glm::vec2(other.position.x, other.position.z));
                    if (distanceXZ < (r.scale + other.scale)) {
                        overlap = true;
                        break;
                    }
                }
            }

            if (!overlap) {
                rocks.push_back(r);
                placed = true;
            }
        }
    }
}

float Tema2::GetTerrainHeightAt(float x, float z) {
    float frequency = 0.1f;
    float amplitude = 2.0f;
    float noise = sin(x * frequency) * cos(z * frequency) * amplitude;
    return noise;
}

void Tema2::UpdateCamera() {
    glm::vec3 dronePos = drone.GetPosition();
    glm::vec3 droneForward = drone.GetForward();
    float distanceBehind = 5.0f;
    float heightAbove = 2.0f;
    glm::vec3 cameraPos = dronePos - droneForward * distanceBehind + glm::vec3(0, heightAbove, 0);
    camera->Set(cameraPos, dronePos, glm::vec3(0, 1, 0));
}

void Tema2::RenderTerrain() {
    terrainShader->Use();

    glm::mat4 modelMatrix = glm::mat4(1.0f);
    GLint loc_model = glGetUniformLocation(terrainShader->program, "Model");
    glUniformMatrix4fv(loc_model, 1, GL_FALSE, glm::value_ptr(modelMatrix));

    glm::mat4 viewMatrix = camera->GetViewMatrix();
    GLint loc_view = glGetUniformLocation(terrainShader->program, "View");
    glUniformMatrix4fv(loc_view, 1, GL_FALSE, glm::value_ptr(viewMatrix));

    GLint loc_projection = glGetUniformLocation(terrainShader->program, "Projection");
    glUniformMatrix4fv(loc_projection, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

    GLint loc_freq = glGetUniformLocation(terrainShader->program, "frequency");
    glUniform1f(loc_freq, 0.1f);

    GLint loc_low = glGetUniformLocation(terrainShader->program, "terrain_color_low");
    glUniform3f(loc_low, 0.1f, 0.4f, 0.1f);

    GLint loc_high = glGetUniformLocation(terrainShader->program, "terrain_color_high");
    glUniform3f(loc_high, 0.3f, 0.8f, 0.3f);

    GLint loc_light = glGetUniformLocation(terrainShader->program, "light_position");
    glUniform3f(loc_light, 10.0f, 50.0f, 10.0f);

    float maxAltitude = 50.0f;
    float droneAltitude = drone.GetPosition().y;

    GLint loc_max_altitude = glGetUniformLocation(terrainShader->program, "max_altitude");
    glUniform1f(loc_max_altitude, maxAltitude);

    GLint loc_drone_altitude = glGetUniformLocation(terrainShader->program, "drone_altitude");
    glUniform1f(loc_drone_altitude, droneAltitude);

    terrainMesh->Render();
}


void Tema2::RenderTrees() {
    basicShader->Use();
    float trunkHeight = 10.5f;
    float trunkWidth = 1.0f;
    float foliageRadius = 3.2f;

    for (auto& t : trees) {
        glm::mat4 viewMatrix = camera->GetViewMatrix();
        GLint loc_view = glGetUniformLocation(basicShader->program, "View");
        glUniformMatrix4fv(loc_view, 1, GL_FALSE, glm::value_ptr(viewMatrix));

        GLint loc_projection = glGetUniformLocation(basicShader->program, "Projection");
        glUniformMatrix4fv(loc_projection, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

        glm::mat4 trunkModel = glm::mat4(1.0f);
        trunkModel = glm::translate(trunkModel, t.position);
        trunkModel = glm::scale(trunkModel, glm::vec3(trunkWidth, trunkHeight, trunkWidth));

        GLint loc_model = glGetUniformLocation(basicShader->program, "Model");
        glUniformMatrix4fv(loc_model, 1, GL_FALSE, glm::value_ptr(trunkModel));

        glUniform4f(glGetUniformLocation(basicShader->program, "object_color"), 0.545f, 0.27f, 0.07f, 1.0f);
        meshes["box"]->Render();

        glm::mat4 foliageModel = glm::mat4(1.0f);
        float foliageOffset = trunkHeight / 2.0f;
        glm::vec3 foliagePosition = t.position + glm::vec3(0, foliageOffset, 0);
        foliageModel = glm::translate(foliageModel, foliagePosition);
        foliageModel = glm::scale(foliageModel, glm::vec3(foliageRadius));

        glUniformMatrix4fv(loc_model, 1, GL_FALSE, glm::value_ptr(foliageModel));

        glUniform4f(glGetUniformLocation(basicShader->program, "object_color"), 0.0f, 0.5f, 0.0f, 1.0f);
        meshes["sphere"]->Render();
    }
}

void Tema2::RenderRocks() {
    basicShader->Use();
    float baseHeight = 2.0f;
    float baseRadius = 0.8f;
    float capRadius = 0.7f;

    for (auto& r : rocks) {
        glm::mat4 viewMatrix = camera->GetViewMatrix();
        GLint loc_view = glGetUniformLocation(basicShader->program, "View");
        glUniformMatrix4fv(loc_view, 1, GL_FALSE, glm::value_ptr(viewMatrix));

        GLint loc_projection = glGetUniformLocation(basicShader->program, "Projection");
        glUniformMatrix4fv(loc_projection, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

        glm::mat4 baseModel = glm::mat4(1.0f);
        baseModel = glm::translate(baseModel, r.position);
        baseModel = glm::scale(baseModel, glm::vec3(baseRadius, baseHeight, baseRadius));

        GLint loc_model = glGetUniformLocation(basicShader->program, "Model");
        glUniformMatrix4fv(loc_model, 1, GL_FALSE, glm::value_ptr(baseModel));

        glUniform4f(glGetUniformLocation(basicShader->program, "object_color"), 0.5f, 0.5f, 0.5f, 1.0f);
        meshes["cylinder"]->Render();

        glm::mat4 capModel = glm::mat4(1.0f);
        capModel = glm::translate(capModel, r.position + glm::vec3(0, baseHeight, 0));
        capModel = glm::scale(capModel, glm::vec3(capRadius));

        glUniformMatrix4fv(loc_model, 1, GL_FALSE, glm::value_ptr(capModel));

        glUniform4f(glGetUniformLocation(basicShader->program, "object_color"), 0.6f, 0.6f, 0.6f, 1.0f);
        meshes["sphere"]->Render();
    }
}

void Tema2::RenderScene(float deltaTimeSeconds) {
    glm::vec3 dronePos = drone.GetPosition();
    glm::vec3 droneFwd = drone.GetForward();
    glm::vec3 droneRight = glm::normalize(glm::cross(glm::vec3(0, 1, 0), droneFwd));
    glm::vec3 droneUp = glm::normalize(glm::cross(droneFwd, droneRight));

    glm::vec3 cameraPos = dronePos - droneFwd * 3.0f + droneUp * 1.0f;
    glm::mat4 viewMatrix = glm::lookAt(cameraPos, dronePos, droneUp);

    basicShader->Use();
    {
        GLint loc_view = glGetUniformLocation(basicShader->program, "View");
        glUniformMatrix4fv(loc_view, 1, GL_FALSE, glm::value_ptr(viewMatrix));
        GLint loc_projection = glGetUniformLocation(basicShader->program, "Projection");
        glUniformMatrix4fv(loc_projection, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
    }
    drone.DrawDrone(meshes, basicShader, [&](Mesh* mesh, Shader* shd, const glm::mat4& modelMatrix) {
        shd->Use();
        GLint loc_model = glGetUniformLocation(shd->program, "Model");
        glUniformMatrix4fv(loc_model, 1, GL_FALSE, glm::value_ptr(modelMatrix));
        mesh->Render();
    });

    RenderTerrain();
    RenderTrees();
    RenderRocks();
}

Mesh* Tema2::CreateCubeMesh(const std::string& name) {
    std::vector<VertexFormat> vertices = {
        VertexFormat(glm::vec3(-0.5f, -0.5f,  0.5f)),
        VertexFormat(glm::vec3( 0.5f, -0.5f,  0.5f)),
        VertexFormat(glm::vec3( 0.5f,  0.5f,  0.5f)),
        VertexFormat(glm::vec3(-0.5f,  0.5f,  0.5f)),
        VertexFormat(glm::vec3(-0.5f, -0.5f, -0.5f)),
        VertexFormat(glm::vec3( 0.5f, -0.5f, -0.5f)),
        VertexFormat(glm::vec3( 0.5f,  0.5f, -0.5f)),
        VertexFormat(glm::vec3(-0.5f,  0.5f, -0.5f))
    };

    std::vector<unsigned int> indices = {
        0, 1, 2, 2, 3, 0,
        3, 2, 6, 6, 7, 3,
        7, 6, 5, 5, 4, 7,
        4, 5, 1, 1, 0, 4,
        4, 0, 3, 3, 7, 4,
        1, 5, 6, 6, 2, 1
    };

    Mesh* cube = new Mesh(name);
    cube->InitFromData(vertices, indices);
    return cube;
}
