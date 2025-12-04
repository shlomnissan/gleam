/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include "vglx/geometries/box_geometry.hpp"

#include <cassert>

namespace vglx {

namespace {

struct PlaneParameters {
    char u;
    char v;
    char w;
    int udir;
    int vdir;
    float width;
    float height;
    float depth;
    unsigned grid_x;
    unsigned grid_y;
};

auto set_component(Vector3& vec, char axis, float value) {
    switch(axis) {
        case 'x': vec.x = value; break;
        case 'y': vec.y = value; break;
        case 'z': vec.z = value; break;
    }
}

auto build_plane(
    const PlaneParameters& params,
    unsigned int& vertex_counter,
    std::vector<float>& vertex_data,
    std::vector<unsigned int>& index_data
) {
    const auto width_half = params.width / 2;
    const auto height_half = params.height / 2;
    const auto depth_half = params.depth / 2;

    const auto grid_x1 = params.grid_x + 1;
    const auto grid_y1 = params.grid_y + 1;

    const auto segment_w = params.width / params.grid_x;
    const auto segment_h = params.height / params.grid_y;

    auto vec = Vector3 {};
    auto counter = 0;

    for (auto iy = 0; iy < grid_y1; ++iy) {
        const auto y = iy * segment_h - height_half;
        for (auto ix = 0; ix < grid_x1; ++ix) {
            const auto x = ix * segment_w - width_half;

            // set position
            set_component(vec, params.u, x * params.udir);
            set_component(vec, params.v, y * params.vdir);
            set_component(vec, params.w, depth_half);

            vertex_data.emplace_back(vec.x);
            vertex_data.emplace_back(vec.y);
            vertex_data.emplace_back(vec.z);

            // set normals
            set_component(vec, params.u, 0);
            set_component(vec, params.v, 0);
            set_component(vec, params.w, params.depth > 0 ? 1 : -1);

            vertex_data.emplace_back(vec.x);
            vertex_data.emplace_back(vec.y);
            vertex_data.emplace_back(vec.z);

            // set uvs
            const auto u = static_cast<float>(ix) / params.grid_x;
            const auto v = 1 - (static_cast<float>(iy) / params.grid_y);
            vertex_data.emplace_back(u);
            vertex_data.emplace_back(v);

            ++counter;
        }
    }

    for (auto iy = 0; iy < params.grid_y; ++iy) {
        for (auto ix = 0; ix < params.grid_x; ++ix) {
            const auto a = vertex_counter + ix + grid_x1 * iy;
            const auto b = vertex_counter + ix + grid_x1 * (iy + 1);
            const auto c = vertex_counter + ix + 1 + grid_x1 * (iy + 1);
            const auto d = vertex_counter + ix + 1 + grid_x1 * iy;

            index_data.emplace_back(a);
            index_data.emplace_back(b);
            index_data.emplace_back(d);
            index_data.emplace_back(b);
            index_data.emplace_back(c);
            index_data.emplace_back(d);
        }
    }

    vertex_counter += counter;
}

}

BoxGeometry::BoxGeometry(const Parameters& params) {
    assert(params.width > 0.0f);
    assert(params.height > 0.0f);
    assert(params.depth > 0.0f);
    assert(params.width_segments > 0);
    assert(params.height_segments > 0);
    assert(params.depth_segments > 0);

    auto vertex_counter = 0u;

    SetName("box geometry");

    build_plane({
        'z', 'y', 'x', -1, -1,
        params.depth, params.height, params.width,
        params.depth_segments, params.height_segments
    }, vertex_counter, vertex_data_, index_data_);

    build_plane({
        'z', 'y', 'x', 1, -1,
        params.depth, params.height, -params.width,
        params.depth_segments, params.height_segments
    }, vertex_counter, vertex_data_, index_data_);

    build_plane({
        'x', 'z', 'y', 1, 1,
        params.width, params.depth, params.height,
        params.width_segments, params.depth_segments
    }, vertex_counter, vertex_data_, index_data_);

    build_plane({
        'x', 'z', 'y', 1, -1,
        params.width, params.depth, -params.height,
        params.width_segments, params.depth_segments
    }, vertex_counter, vertex_data_, index_data_);

    build_plane({
        'x', 'y', 'z', 1, -1,
        params.width, params.height, params.depth,
        params.width_segments, params.height_segments
    }, vertex_counter, vertex_data_, index_data_);

    build_plane({
        'x', 'y', 'z', -1, -1,
        params.width, params.height, -params.depth,
        params.width_segments, params.height_segments
    }, vertex_counter, vertex_data_, index_data_);

    SetAttribute({.type = Geometry::VertexAttributeType::Position, .item_size = 3});
    SetAttribute({.type = Geometry::VertexAttributeType::Normal, .item_size = 3});
    SetAttribute({.type = Geometry::VertexAttributeType::UV, .item_size = 2});
}

}