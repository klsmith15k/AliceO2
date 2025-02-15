// Copyright 2019-2020 CERN and copyright holders of ALICE O2.
// See https://alice-o2.web.cern.ch/copyright for details of the copyright holders.
// All rights not expressly granted are reserved.
//
// This software is distributed under the terms of the GNU General Public
// License v3 (GPL Version 3), copied verbatim in the file "COPYING".
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

#include "Framework/TableBuilder.h"

#include <benchmark/benchmark.h>

using namespace o2::framework;
using namespace o2::soa;

static void BM_TableBuilderOverhead(benchmark::State& state)
{
  using namespace o2::framework;

  for (auto _ : state) {
    TableBuilder builder;
    [[maybe_unused]] auto rowWriter = builder.persist<float, float, float>({"x", "y", "z"});
    auto table = builder.finalize();
  }
}

BENCHMARK(BM_TableBuilderOverhead);

static void BM_TableBuilderScalar(benchmark::State& state)
{
  using namespace o2::framework;
  for (auto _ : state) {
    TableBuilder builder;
    auto rowWriter = builder.persist<float>({"x"});
    for (auto i = 0; i < state.range(0); ++i) {
      rowWriter(0, 0.f);
    }
    auto table = builder.finalize();
  }
}

BENCHMARK(BM_TableBuilderScalar)->Arg(1 << 21);
BENCHMARK(BM_TableBuilderScalar)->Range(8, 8 << 16);

static void BM_TableBuilderScalarReserved(benchmark::State& state)
{
  using namespace o2::framework;
  for (auto _ : state) {
    TableBuilder builder;
    auto rowWriter = builder.persist<float>({"x"});
    builder.reserve(o2::framework::pack<float>{}, state.range(0));
    for (auto i = 0; i < state.range(0); ++i) {
      rowWriter(0, 0.f);
    }
    auto table = builder.finalize();
  }
}

BENCHMARK(BM_TableBuilderScalarReserved)->Arg(1 << 21);
BENCHMARK(BM_TableBuilderScalarReserved)->Range(8, 8 << 16);

static void BM_TableBuilderScalarPresized(benchmark::State& state)
{
  using namespace o2::framework;
  for (auto _ : state) {
    TableBuilder builder;
    auto rowWriter = builder.preallocatedPersist<float>({"x"}, state.range(0));
    for (auto i = 0; i < state.range(0); ++i) {
      rowWriter(0, 0.f);
    }
    auto table = builder.finalize();
  }
}

BENCHMARK(BM_TableBuilderScalarPresized)->Arg(1 << 20);
BENCHMARK(BM_TableBuilderScalarPresized)->Range(8, 8 << 16);

static void BM_TableBuilderScalarBulk(benchmark::State& state)
{
  using namespace o2::framework;
  auto chunkSize = state.range(0) / 256;
  std::vector<float> buffer(chunkSize, 0.); // We assume data is chunked in blocks 256th of the total size
  for (auto _ : state) {
    TableBuilder builder;
    auto bulkWriter = builder.bulkPersist<float>({"x"}, state.range(0));
    for (auto i = 0; i < state.range(0) / chunkSize; ++i) {
      bulkWriter(0, chunkSize, buffer.data());
    }
    auto table = builder.finalize();
  }
}

BENCHMARK(BM_TableBuilderScalarBulk)->Range(256, 1 << 20);

static void BM_TableBuilderSimple(benchmark::State& state)
{
  using namespace o2::framework;
  for (auto _ : state) {
    TableBuilder builder;
    auto rowWriter = builder.persist<float, float, float>({"x", "y", "z"});
    for (auto i = 0; i < state.range(0); ++i) {
      rowWriter(0, 0.f, 0.f, 0.f);
    }
    auto table = builder.finalize();
  }
}

BENCHMARK(BM_TableBuilderSimple)->Arg(1 << 20);

static void BM_TableBuilderSimple2(benchmark::State& state)
{
  using namespace o2::framework;
  for (auto _ : state) {
    TableBuilder builder;
    auto rowWriter = builder.persist<float, float, float>({"x", "y", "z"});
    for (auto i = 0; i < state.range(0); ++i) {
      rowWriter(0, 0.f, 0.f, 0.f);
    }
    auto table = builder.finalize();
  }
}

BENCHMARK(BM_TableBuilderSimple2)->Range(8, 8 << 16);

namespace test
{
DECLARE_SOA_COLUMN(X, x, float);
DECLARE_SOA_COLUMN(Y, y, float);
DECLARE_SOA_COLUMN(Z, z, float);
} // namespace test

using TestVectors = o2::soa::InPlaceTable<"TST/0"_h, test::X, test::Y, test::Z>;

static void BM_TableBuilderSoA(benchmark::State& state)
{
  using namespace o2::framework;
  for (auto _ : state) {
    TableBuilder builder;
    auto rowWriter = builder.cursor<TestVectors>();
    for (auto i = 0; i < state.range(0); ++i) {
      rowWriter(0, 0.f, 0.f, 0.f);
    }
    auto table = builder.finalize();
  }
}

BENCHMARK(BM_TableBuilderSoA)->Range(8, 8 << 16);

static void BM_TableBuilderComplex(benchmark::State& state)
{
  using namespace o2::framework;
  for (auto _ : state) {
    TableBuilder builder;
    auto rowWriter = builder.persist<int, float, std::string, bool>({"x", "y", "s", "b"});
    for (auto i = 0; i < state.range(0); ++i) {
      rowWriter(0, 0, 0., "foo", true);
    }
    auto table = builder.finalize();
  }
}

BENCHMARK(BM_TableBuilderComplex)->Range(8, 8 << 16);

BENCHMARK_MAIN();
