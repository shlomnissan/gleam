/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include <benchmark/benchmark.h>

#include <vglx/math/utilities.hpp>

#include <cmath>
#include <random>
#include <vector>

namespace {

auto random_floats(std::size_t count) {
    auto rng = std::mt19937 {42};
    auto dist = std::uniform_real_distribution<float> {0.1f, 1000.0f};
    auto values = std::vector<float>(count);
    for (auto& value : values) value = dist(rng);
    return values;
}

auto BM_MathSqrt(benchmark::State& state) {
    const auto values = random_floats(state.range(0));
    for (auto _ : state) {
        for (const auto value : values) {
            benchmark::DoNotOptimize(vglx::math::Sqrt(value));
        }
    }
    state.SetItemsProcessed(state.iterations() * values.size());
}
BENCHMARK(BM_MathSqrt)->Range(1 << 10, 1 << 20);

auto BM_StdSqrt(benchmark::State& state) {
    const auto values = random_floats(state.range(0));
    for (auto _ : state) {
        for (const auto value : values) {
            benchmark::DoNotOptimize(std::sqrt(value));
        }
    }
    state.SetItemsProcessed(state.iterations() * values.size());
}
BENCHMARK(BM_StdSqrt)->Range(1 << 10, 1 << 20);

auto BM_MathInverseSqrt(benchmark::State& state) {
    const auto values = random_floats(state.range(0));
    for (auto _ : state) {
        for (const auto value : values) {
            benchmark::DoNotOptimize(vglx::math::InverseSqrt(value));
        }
    }
    state.SetItemsProcessed(state.iterations() * values.size());
}
BENCHMARK(BM_MathInverseSqrt)->Range(1 << 10, 1 << 20);

auto BM_StdInverseSqrt(benchmark::State& state) {
    const auto values = random_floats(state.range(0));
    for (auto _ : state) {
        for (const auto value : values) {
            benchmark::DoNotOptimize(1.0f / std::sqrt(value));
        }
    }
    state.SetItemsProcessed(state.iterations() * values.size());
}
BENCHMARK(BM_StdInverseSqrt)->Range(1 << 10, 1 << 20);

}
