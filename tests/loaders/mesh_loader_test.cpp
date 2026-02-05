/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include <gtest/gtest.h>

#include <memory>

#include <vglx/geometries/geometry.hpp>
#include <vglx/loaders/load_scheduler.hpp>
#include <vglx/loaders/mesh_loader.hpp>
#include <vglx/scene/mesh.hpp>

class MeshLoaderTest : public ::testing::Test {
public:
    MeshLoaderTest()
      : loader(std::make_unique<vglx::MeshLoader>(scheduler.get())) {}

    std::unique_ptr<vglx::LoadScheduler> scheduler = std::make_unique<vglx::LoadScheduler>();
    std::unique_ptr<vglx::MeshLoader> loader;
};

TEST_F(MeshLoaderTest, LoadMesh) {
    auto result = loader->Load("assets/plane.msh");
    EXPECT_TRUE(result.has_value());

    auto root = std::move(result.value());
    EXPECT_EQ(root->GetChildren().size(), 1);

    auto mesh = static_cast<vglx::Mesh*>(root->GetChildren().front().get());
    auto geometry = mesh->GetGeometry();
    EXPECT_EQ(geometry->VertexCount(), 4);
    EXPECT_EQ(geometry->IndexCount(), 6);
}

TEST_F(MeshLoaderTest, LoadMeshInvalidFileFormat) {
    auto result = loader->Load("assets/plane.obj");

    EXPECT_EQ(false, result.has_value());
    EXPECT_EQ(result.error(), "Invalid mesh file 'assets/plane.obj'");
}

TEST_F(MeshLoaderTest, LoadMeshInvalidPath) {
   auto result = loader->Load("assets/invalid_path.msh");

    EXPECT_EQ(false, result.has_value());
    EXPECT_EQ(result.error(), "Unable to open file 'assets/invalid_path.msh'");
}