/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include <gtest/gtest.h>

#include <vglx/geometries/geometry.hpp>
#include <vglx/scene/mesh.hpp>

#include "loaders/mesh_loader_xyz.hpp"

TEST(MeshLoader, LoadMesh) {
    auto result = vglx::load_mesh("assets/plane.msh");
    EXPECT_TRUE(result.has_value());

    auto root = std::move(result.value());
    EXPECT_EQ(root->Children().size(), 1);

    auto mesh = static_cast<vglx::Mesh*>(root->Children().front().get());
    auto geometry = mesh->GetGeometry();
    EXPECT_EQ(geometry->VertexCount(), 4);
    EXPECT_EQ(geometry->IndexCount(), 6);
}

TEST(MeshLoader, LoadMeshInvalidFileFormat) {
    auto result = vglx::load_mesh("assets/plane.obj");

    EXPECT_EQ(false, result.has_value());
    EXPECT_EQ(result.error(), "Invalid mesh file 'assets/plane.obj'");
}

TEST(MeshLoader, LoadMeshInvalidPath) {
    auto result = vglx::load_mesh("assets/invalid_path.msh");

    EXPECT_EQ(false, result.has_value());
    EXPECT_EQ(result.error(), "Unable to open file 'assets/invalid_path.msh'");
}