// ======================================================================== //
// Copyright 2009-2014 Intel Corporation                                    //
//                                                                          //
// Licensed under the Apache License, Version 2.0 (the "License");          //
// you may not use this file except in compliance with the License.         //
// You may obtain a copy of the License at                                  //
//                                                                          //
//     http://www.apache.org/licenses/LICENSE-2.0                           //
//                                                                          //
// Unless required by applicable law or agreed to in writing, software      //
// distributed under the License is distributed on an "AS IS" BASIS,        //
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. //
// See the License for the specific language governing permissions and      //
// limitations under the License.                                           //
// ======================================================================== //

#include "bvh4.h"

#include "geometry/bezier1.h"
#include "geometry/bezier1i.h"
#include "geometry/triangle1.h"
#include "geometry/triangle4.h"
#include "geometry/triangle8.h"
#include "geometry/triangle1v.h"
#include "geometry/triangle4v.h"
#include "geometry/triangle4i.h"

#include "common/accelinstance.h"

namespace embree
{
  DECLARE_SYMBOL(Accel::Intersector1,BVH4Bezier1Intersector1);
  DECLARE_SYMBOL(Accel::Intersector1,BVH4Bezier1iIntersector1);
  DECLARE_SYMBOL(Accel::Intersector1,BVH4Triangle1Intersector1Moeller);
  DECLARE_SYMBOL(Accel::Intersector1,BVH4Triangle4Intersector1Moeller);
  DECLARE_SYMBOL(Accel::Intersector1,BVH4Triangle8Intersector1Moeller);
  DECLARE_SYMBOL(Accel::Intersector1,BVH4Triangle1vIntersector1Pluecker);
  DECLARE_SYMBOL(Accel::Intersector1,BVH4Triangle4vIntersector1Pluecker);
  DECLARE_SYMBOL(Accel::Intersector1,BVH4Triangle4iIntersector1Pluecker);
  DECLARE_SYMBOL(Accel::Intersector1,BVH4VirtualIntersector1);

  DECLARE_SYMBOL(Accel::Intersector4,BVH4Bezier1Intersector4Chunk);
  DECLARE_SYMBOL(Accel::Intersector4,BVH4Bezier1iIntersector4Chunk);
  DECLARE_SYMBOL(Accel::Intersector4,BVH4Triangle1Intersector4ChunkMoeller);
  DECLARE_SYMBOL(Accel::Intersector4,BVH4Triangle4Intersector4ChunkMoeller);
  DECLARE_SYMBOL(Accel::Intersector4,BVH4Triangle4Intersector4ChunkMoellerNoFilter);
  DECLARE_SYMBOL(Accel::Intersector4,BVH4Triangle8Intersector4ChunkMoeller);
  DECLARE_SYMBOL(Accel::Intersector4,BVH4Triangle8Intersector4ChunkMoellerNoFilter);
  DECLARE_SYMBOL(Accel::Intersector4,BVH4Triangle4Intersector4HybridMoeller);
  DECLARE_SYMBOL(Accel::Intersector4,BVH4Triangle4Intersector4HybridMoellerNoFilter);
  DECLARE_SYMBOL(Accel::Intersector4,BVH4Triangle8Intersector4HybridMoeller);
  DECLARE_SYMBOL(Accel::Intersector4,BVH4Triangle8Intersector4HybridMoellerNoFilter);
  DECLARE_SYMBOL(Accel::Intersector4,BVH4Triangle1vIntersector4ChunkPluecker);
  DECLARE_SYMBOL(Accel::Intersector4,BVH4Triangle4vIntersector4ChunkPluecker);
  DECLARE_SYMBOL(Accel::Intersector4,BVH4Triangle4vIntersector4HybridPluecker);
  DECLARE_SYMBOL(Accel::Intersector4,BVH4Triangle4iIntersector4ChunkPluecker);
  DECLARE_SYMBOL(Accel::Intersector4,BVH4VirtualIntersector4Chunk);

  DECLARE_SYMBOL(Accel::Intersector8,BVH4Bezier1Intersector8Chunk);
  DECLARE_SYMBOL(Accel::Intersector8,BVH4Bezier1iIntersector8Chunk);
  DECLARE_SYMBOL(Accel::Intersector8,BVH4Triangle1Intersector8ChunkMoeller);
  DECLARE_SYMBOL(Accel::Intersector8,BVH4Triangle4Intersector8ChunkMoeller);
  DECLARE_SYMBOL(Accel::Intersector8,BVH4Triangle4Intersector8ChunkMoellerNoFilter);
  DECLARE_SYMBOL(Accel::Intersector8,BVH4Triangle8Intersector8ChunkMoeller);
  DECLARE_SYMBOL(Accel::Intersector8,BVH4Triangle8Intersector8ChunkMoellerNoFilter);
  DECLARE_SYMBOL(Accel::Intersector8,BVH4Triangle4Intersector8HybridMoeller);
  DECLARE_SYMBOL(Accel::Intersector8,BVH4Triangle4Intersector8HybridMoellerNoFilter);
  DECLARE_SYMBOL(Accel::Intersector8,BVH4Triangle8Intersector8HybridMoeller);
  DECLARE_SYMBOL(Accel::Intersector8,BVH4Triangle8Intersector8HybridMoellerNoFilter);
  DECLARE_SYMBOL(Accel::Intersector8,BVH4Triangle1vIntersector8ChunkPluecker);
  DECLARE_SYMBOL(Accel::Intersector8,BVH4Triangle4vIntersector8ChunkPluecker);
  DECLARE_SYMBOL(Accel::Intersector8,BVH4Triangle4vIntersector8HybridPluecker);
  DECLARE_SYMBOL(Accel::Intersector8,BVH4Triangle4iIntersector8ChunkPluecker);
  DECLARE_SYMBOL(Accel::Intersector8,BVH4VirtualIntersector8Chunk);

  DECLARE_TOPLEVEL_BUILDER(BVH4BuilderTopLevelFast);

  DECLARE_BUILDER(BVH4BuilderObjectSplit4Fast);

  DECLARE_SCENE_BUILDER(BVH4Triangle1Builder);
  DECLARE_SCENE_BUILDER(BVH4Triangle4Builder);
  DECLARE_SCENE_BUILDER(BVH4Triangle8Builder);
  DECLARE_SCENE_BUILDER(BVH4Triangle1vBuilder);
  DECLARE_SCENE_BUILDER(BVH4Triangle4vBuilder);
  DECLARE_SCENE_BUILDER(BVH4Triangle4iBuilder);

  DECLARE_TRIANGLEMESH_BUILDER(BVH4Triangle1MeshBuilder);
  DECLARE_TRIANGLEMESH_BUILDER(BVH4Triangle4MeshBuilder);
  DECLARE_TRIANGLEMESH_BUILDER(BVH4Triangle8MeshBuilder);
  DECLARE_TRIANGLEMESH_BUILDER(BVH4Triangle1vMeshBuilder);
  DECLARE_TRIANGLEMESH_BUILDER(BVH4Triangle4vMeshBuilder);
  DECLARE_TRIANGLEMESH_BUILDER(BVH4Triangle4iMeshBuilder);

  DECLARE_SCENE_BUILDER(BVH4Bezier1BuilderFast);
  DECLARE_SCENE_BUILDER(BVH4Bezier1iBuilderFast);
  DECLARE_SCENE_BUILDER(BVH4Triangle1BuilderFast);
  DECLARE_SCENE_BUILDER(BVH4Triangle4BuilderFast);
  DECLARE_SCENE_BUILDER(BVH4Triangle8BuilderFast);
  DECLARE_SCENE_BUILDER(BVH4Triangle1vBuilderFast);
  DECLARE_SCENE_BUILDER(BVH4Triangle4vBuilderFast);
  DECLARE_SCENE_BUILDER(BVH4Triangle4iBuilderFast);
  DECLARE_SCENE_BUILDER(BVH4UserGeometryBuilderFast);

  DECLARE_TRIANGLEMESH_BUILDER(BVH4Triangle1MeshBuilderFast);
  DECLARE_TRIANGLEMESH_BUILDER(BVH4Triangle4MeshBuilderFast);
  DECLARE_TRIANGLEMESH_BUILDER(BVH4Triangle8MeshBuilderFast);
  DECLARE_TRIANGLEMESH_BUILDER(BVH4Triangle1vMeshBuilderFast);
  DECLARE_TRIANGLEMESH_BUILDER(BVH4Triangle4vMeshBuilderFast);
  DECLARE_TRIANGLEMESH_BUILDER(BVH4Triangle4iMeshBuilderFast);
  DECLARE_USERGEOMETRY_BUILDER(BVH4UserGeometryMeshBuilderFast);

  DECLARE_TRIANGLEMESH_BUILDER(BVH4Triangle1MeshRefitFast);
  DECLARE_TRIANGLEMESH_BUILDER(BVH4Triangle4MeshRefitFast);
  DECLARE_TRIANGLEMESH_BUILDER(BVH4Triangle8MeshRefitFast);
  DECLARE_TRIANGLEMESH_BUILDER(BVH4Triangle1vMeshRefitFast);
  DECLARE_TRIANGLEMESH_BUILDER(BVH4Triangle4vMeshRefitFast);
  DECLARE_TRIANGLEMESH_BUILDER(BVH4Triangle4iMeshRefitFast);

  DECLARE_SCENE_BUILDER(BVH4Triangle1BuilderMorton);
  DECLARE_SCENE_BUILDER(BVH4Triangle4BuilderMorton);
  DECLARE_SCENE_BUILDER(BVH4Triangle8BuilderMorton);
  DECLARE_SCENE_BUILDER(BVH4Triangle1vBuilderMorton);
  DECLARE_SCENE_BUILDER(BVH4Triangle4vBuilderMorton);
  DECLARE_SCENE_BUILDER(BVH4Triangle4iBuilderMorton);

  DECLARE_TRIANGLEMESH_BUILDER(BVH4Triangle1MeshBuilderMorton);
  DECLARE_TRIANGLEMESH_BUILDER(BVH4Triangle4MeshBuilderMorton);
  DECLARE_TRIANGLEMESH_BUILDER(BVH4Triangle8MeshBuilderMorton);
  DECLARE_TRIANGLEMESH_BUILDER(BVH4Triangle1vMeshBuilderMorton);
  DECLARE_TRIANGLEMESH_BUILDER(BVH4Triangle4vMeshBuilderMorton);
  DECLARE_TRIANGLEMESH_BUILDER(BVH4Triangle4iMeshBuilderMorton);

  void BVH4Register () 
  {
    int features = getCPUFeatures();

    /* select builders */
    SELECT_SYMBOL_DEFAULT_SSE41(features,BVH4BuilderTopLevelFast);

    SELECT_SYMBOL_DEFAULT_AVX(features,BVH4Triangle1Builder);
    SELECT_SYMBOL_DEFAULT_AVX(features,BVH4Triangle4Builder);
    SELECT_SYMBOL_AVX(features,BVH4Triangle8Builder);
    SELECT_SYMBOL_DEFAULT_AVX(features,BVH4Triangle1vBuilder);
    SELECT_SYMBOL_DEFAULT_AVX(features,BVH4Triangle4vBuilder);
    SELECT_SYMBOL_DEFAULT_AVX(features,BVH4Triangle4iBuilder);
    
    SELECT_SYMBOL_DEFAULT_AVX(features,BVH4Triangle1MeshBuilder);
    SELECT_SYMBOL_DEFAULT_AVX(features,BVH4Triangle4MeshBuilder);
    SELECT_SYMBOL_AVX(features,BVH4Triangle8MeshBuilder);
    SELECT_SYMBOL_DEFAULT_AVX(features,BVH4Triangle1vMeshBuilder);
    SELECT_SYMBOL_DEFAULT_AVX(features,BVH4Triangle4vMeshBuilder);
    SELECT_SYMBOL_DEFAULT_AVX(features,BVH4Triangle4iMeshBuilder);
  
    SELECT_SYMBOL_DEFAULT_AVX(features,BVH4Bezier1BuilderFast);
    SELECT_SYMBOL_DEFAULT_AVX(features,BVH4Bezier1iBuilderFast);
    SELECT_SYMBOL_DEFAULT_AVX(features,BVH4Triangle1BuilderFast);
    SELECT_SYMBOL_DEFAULT_AVX(features,BVH4Triangle4BuilderFast);
    SELECT_SYMBOL_AVX        (features,BVH4Triangle8BuilderFast);
    SELECT_SYMBOL_DEFAULT_AVX(features,BVH4Triangle1vBuilderFast);
    SELECT_SYMBOL_DEFAULT_AVX(features,BVH4Triangle4vBuilderFast);
    SELECT_SYMBOL_DEFAULT_AVX(features,BVH4Triangle4iBuilderFast);
    SELECT_SYMBOL_DEFAULT_AVX(features,BVH4UserGeometryBuilderFast);
    
    SELECT_SYMBOL_DEFAULT_AVX(features,BVH4Triangle1MeshBuilderFast);
    SELECT_SYMBOL_DEFAULT_AVX(features,BVH4Triangle4MeshBuilderFast);
    SELECT_SYMBOL_AVX        (features,BVH4Triangle8MeshBuilderFast);
    SELECT_SYMBOL_DEFAULT_AVX(features,BVH4Triangle1vMeshBuilderFast);
    SELECT_SYMBOL_DEFAULT_AVX(features,BVH4Triangle4vMeshBuilderFast);
    SELECT_SYMBOL_DEFAULT_AVX(features,BVH4Triangle4iMeshBuilderFast);
    SELECT_SYMBOL_DEFAULT_AVX(features,BVH4UserGeometryMeshBuilderFast);

    SELECT_SYMBOL_DEFAULT_AVX(features,BVH4Triangle1MeshRefitFast);
    SELECT_SYMBOL_DEFAULT_AVX(features,BVH4Triangle4MeshRefitFast);
    SELECT_SYMBOL_AVX        (features,BVH4Triangle8MeshRefitFast);
    SELECT_SYMBOL_DEFAULT_AVX(features,BVH4Triangle1vMeshRefitFast);
    SELECT_SYMBOL_DEFAULT_AVX(features,BVH4Triangle4vMeshRefitFast);
    SELECT_SYMBOL_DEFAULT_AVX(features,BVH4Triangle4iMeshRefitFast);

    SELECT_SYMBOL_DEFAULT_AVX(features,BVH4Triangle1BuilderMorton);
    SELECT_SYMBOL_DEFAULT_AVX(features,BVH4Triangle4BuilderMorton);
    SELECT_SYMBOL_AVX        (features,BVH4Triangle8BuilderMorton);
    SELECT_SYMBOL_DEFAULT_AVX(features,BVH4Triangle1vBuilderMorton);
    SELECT_SYMBOL_DEFAULT_AVX(features,BVH4Triangle4vBuilderMorton);
    SELECT_SYMBOL_DEFAULT_AVX(features,BVH4Triangle4iBuilderMorton);
    
    SELECT_SYMBOL_DEFAULT_AVX(features,BVH4Triangle1MeshBuilderMorton);
    SELECT_SYMBOL_DEFAULT_AVX(features,BVH4Triangle4MeshBuilderMorton);
    SELECT_SYMBOL_AVX        (features,BVH4Triangle8MeshBuilderMorton);
    SELECT_SYMBOL_DEFAULT_AVX(features,BVH4Triangle1vMeshBuilderMorton);
    SELECT_SYMBOL_DEFAULT_AVX(features,BVH4Triangle4vMeshBuilderMorton);
    SELECT_SYMBOL_DEFAULT_AVX(features,BVH4Triangle4iMeshBuilderMorton);

    /* select intersectors1 */
    SELECT_SYMBOL_DEFAULT_AVX_AVX2      (features,BVH4Bezier1Intersector1);
    SELECT_SYMBOL_DEFAULT_AVX_AVX2      (features,BVH4Bezier1iIntersector1);
    SELECT_SYMBOL_DEFAULT_SSE41_AVX_AVX2(features,BVH4Triangle1Intersector1Moeller);
    SELECT_SYMBOL_DEFAULT_SSE41_AVX_AVX2(features,BVH4Triangle4Intersector1Moeller);
    SELECT_SYMBOL_AVX_AVX2              (features,BVH4Triangle8Intersector1Moeller);
    SELECT_SYMBOL_DEFAULT_SSE41_AVX     (features,BVH4Triangle1vIntersector1Pluecker);
    SELECT_SYMBOL_DEFAULT_SSE41_AVX     (features,BVH4Triangle4vIntersector1Pluecker);
    SELECT_SYMBOL_DEFAULT_SSE41_AVX     (features,BVH4Triangle4iIntersector1Pluecker);
    SELECT_SYMBOL_DEFAULT_SSE41_AVX_AVX2(features,BVH4VirtualIntersector1);

    /* select intersectors4 */
    SELECT_SYMBOL_DEFAULT_AVX_AVX2      (features,BVH4Bezier1Intersector4Chunk);
    SELECT_SYMBOL_DEFAULT_AVX_AVX2      (features,BVH4Bezier1iIntersector4Chunk);
    SELECT_SYMBOL_DEFAULT_SSE41_AVX_AVX2(features,BVH4Triangle1Intersector4ChunkMoeller);
    SELECT_SYMBOL_DEFAULT_SSE41_AVX_AVX2(features,BVH4Triangle4Intersector4ChunkMoeller);
    SELECT_SYMBOL_DEFAULT_SSE41_AVX_AVX2(features,BVH4Triangle4Intersector4ChunkMoellerNoFilter);
    SELECT_SYMBOL_AVX_AVX2              (features,BVH4Triangle8Intersector4ChunkMoeller);
    SELECT_SYMBOL_DEFAULT2              (features,BVH4Triangle4Intersector4HybridMoeller,BVH4Triangle4Intersector4ChunkMoeller); // hybrid not supported below SSE4.2
    SELECT_SYMBOL_AVX_AVX2              (features,BVH4Triangle8Intersector4ChunkMoellerNoFilter);
    SELECT_SYMBOL_DEFAULT2              (features,BVH4Triangle4Intersector4HybridMoellerNoFilter,BVH4Triangle4Intersector4ChunkMoellerNoFilter); // hybrid not supported below SSE4.2
    SELECT_SYMBOL_SSE42_AVX_AVX2        (features,BVH4Triangle4Intersector4HybridMoeller);
    SELECT_SYMBOL_SSE42_AVX_AVX2        (features,BVH4Triangle4Intersector4HybridMoellerNoFilter);
    SELECT_SYMBOL_AVX_AVX2              (features,BVH4Triangle8Intersector4HybridMoeller);
    SELECT_SYMBOL_AVX_AVX2              (features,BVH4Triangle8Intersector4HybridMoellerNoFilter);
    SELECT_SYMBOL_DEFAULT_SSE41_AVX     (features,BVH4Triangle1vIntersector4ChunkPluecker);
    SELECT_SYMBOL_DEFAULT_SSE41_AVX     (features,BVH4Triangle4vIntersector4ChunkPluecker);
    SELECT_SYMBOL_DEFAULT2              (features,BVH4Triangle4vIntersector4HybridPluecker,BVH4Triangle4vIntersector4ChunkPluecker); // hybrid not supported below SSE4.2
    SELECT_SYMBOL_SSE42_AVX             (features,BVH4Triangle4vIntersector4HybridPluecker);
    SELECT_SYMBOL_DEFAULT_SSE41_AVX     (features,BVH4Triangle4iIntersector4ChunkPluecker);
    SELECT_SYMBOL_DEFAULT_SSE41_AVX_AVX2(features,BVH4VirtualIntersector4Chunk);

    /* select intersectors8 */
    SELECT_SYMBOL_AVX_AVX2(features,BVH4Bezier1Intersector8Chunk);
    SELECT_SYMBOL_AVX_AVX2(features,BVH4Bezier1iIntersector8Chunk);
    SELECT_SYMBOL_AVX_AVX2(features,BVH4Triangle1Intersector8ChunkMoeller);
    SELECT_SYMBOL_AVX_AVX2(features,BVH4Triangle4Intersector8ChunkMoeller);
    SELECT_SYMBOL_AVX_AVX2(features,BVH4Triangle4Intersector8ChunkMoellerNoFilter);
    SELECT_SYMBOL_AVX_AVX2(features,BVH4Triangle8Intersector8ChunkMoeller);
    SELECT_SYMBOL_AVX_AVX2(features,BVH4Triangle8Intersector8ChunkMoellerNoFilter);
    SELECT_SYMBOL_AVX_AVX2(features,BVH4Triangle4Intersector8HybridMoeller);
    SELECT_SYMBOL_AVX_AVX2(features,BVH4Triangle4Intersector8HybridMoellerNoFilter);
    SELECT_SYMBOL_AVX_AVX2(features,BVH4Triangle8Intersector8HybridMoeller);
    SELECT_SYMBOL_AVX_AVX2(features,BVH4Triangle8Intersector8HybridMoellerNoFilter);
    SELECT_SYMBOL_AVX     (features,BVH4Triangle1vIntersector8ChunkPluecker);
    SELECT_SYMBOL_AVX     (features,BVH4Triangle4vIntersector8ChunkPluecker);
    SELECT_SYMBOL_AVX     (features,BVH4Triangle4vIntersector8HybridPluecker);
    SELECT_SYMBOL_AVX     (features,BVH4Triangle4iIntersector8ChunkPluecker);
    SELECT_SYMBOL_AVX_AVX2(features,BVH4VirtualIntersector8Chunk);
  }

  BVH4::BVH4 (const PrimitiveType& primTy, void* geometry)
  : primTy(primTy), geometry(geometry), root(emptyNode),
    numPrimitives(0), numVertices(0) {}

  BVH4::~BVH4 () {
    for (size_t i=0; i<objects.size(); i++) 
      delete objects[i];
  }

  void BVH4::init(size_t numPrimitives, size_t numThreads)
  {
    /* allocate as much memory as likely needed and reserve conservative amounts of memory */
    size_t blockSize = LinearAllocatorPerThread::allocBlockSize;
    
    size_t numPrimBlocks = primTy.blocks(numPrimitives);
    size_t numAllocatedNodes = min(size_t(0.6*numPrimBlocks),numPrimitives);
    size_t numAllocatedPrimitives = min(size_t(1.2*numPrimBlocks),numPrimitives);
#if defined(__X86_64__)
    size_t numReservedNodes = 2*numPrimitives;
    size_t numReservedPrimitives = 2*numPrimitives;
#else
    size_t numReservedNodes = 1.5*numAllocatedNodes;
    size_t numReservedPrimitives = 1.5*numAllocatedPrimitives;
#endif
    
    size_t bytesAllocated = numAllocatedNodes * sizeof(BVH4::Node) + numAllocatedPrimitives * primTy.bytes; // required also for parallel split stage in BVH4BuilderFast
    size_t bytesReserved  = numReservedNodes  * sizeof(BVH4::Node) + numReservedPrimitives  * primTy.bytes;
    bytesReserved         = (bytesReserved+blockSize-1)/blockSize*blockSize + numThreads*blockSize*2;

    root = emptyNode;
    alloc.init(bytesAllocated,bytesReserved);
  }

  void BVH4::clearBarrier(NodeRef& node)
  {
    if (node.isBarrier())
      node.clearBarrier();
    else if (!node.isLeaf()) {
      Node* n = node.node();
      for (size_t c=0; c<N; c++)
        clearBarrier(n->child(c));
    }
  }

  Accel::Intersectors BVH4Bezier1Intersectors(BVH4* bvh)
  {
    Accel::Intersectors intersectors;
    intersectors.ptr = bvh;
    intersectors.intersector1 = BVH4Bezier1Intersector1;
    intersectors.intersector4 = BVH4Bezier1Intersector4Chunk;
    intersectors.intersector8 = BVH4Bezier1Intersector8Chunk;
    intersectors.intersector16 = NULL;
    return intersectors;
  }
  
  Accel::Intersectors BVH4Bezier1iIntersectors(BVH4* bvh)
  {
    Accel::Intersectors intersectors;
    intersectors.ptr = bvh;
    intersectors.intersector1 = BVH4Bezier1iIntersector1;
    intersectors.intersector4 = BVH4Bezier1iIntersector4Chunk;
    intersectors.intersector8 = BVH4Bezier1iIntersector8Chunk;
    intersectors.intersector16 = NULL;
    return intersectors;
  }
  
  Accel::Intersectors BVH4Triangle1Intersectors(BVH4* bvh)
  {
    Accel::Intersectors intersectors;
    intersectors.ptr = bvh;
    intersectors.intersector1 = BVH4Triangle1Intersector1Moeller;
    intersectors.intersector4 = BVH4Triangle1Intersector4ChunkMoeller;
    intersectors.intersector8 = BVH4Triangle1Intersector8ChunkMoeller;
    intersectors.intersector16 = NULL;
    return intersectors;
  }

  Accel::Intersectors BVH4Triangle4IntersectorsChunk(BVH4* bvh)
  {
    Accel::Intersectors intersectors;
    intersectors.ptr = bvh;
    intersectors.intersector1 = BVH4Triangle4Intersector1Moeller;
    intersectors.intersector4_filter   = BVH4Triangle4Intersector4ChunkMoeller;
    intersectors.intersector4_nofilter = BVH4Triangle4Intersector4ChunkMoellerNoFilter;
    intersectors.intersector8_filter   = BVH4Triangle4Intersector8ChunkMoeller;
    intersectors.intersector8_nofilter = BVH4Triangle4Intersector8ChunkMoellerNoFilter;
    intersectors.intersector16 = NULL;
    return intersectors;
  }

  Accel::Intersectors BVH4Triangle4IntersectorsHybrid(BVH4* bvh)
  {
    Accel::Intersectors intersectors;
    intersectors.ptr = bvh;
    intersectors.intersector1 = BVH4Triangle4Intersector1Moeller;
    intersectors.intersector4_filter   = BVH4Triangle4Intersector4HybridMoeller;
    intersectors.intersector4_nofilter = BVH4Triangle4Intersector4HybridMoellerNoFilter;
    intersectors.intersector8_filter = BVH4Triangle4Intersector8HybridMoeller;
    intersectors.intersector8_nofilter = BVH4Triangle4Intersector8HybridMoellerNoFilter;
    intersectors.intersector16 = NULL;
    return intersectors;
  }

  Accel::Intersectors BVH4Triangle8IntersectorsChunk(BVH4* bvh)
  {
    Accel::Intersectors intersectors;
    intersectors.ptr = bvh;
    intersectors.intersector1 = BVH4Triangle8Intersector1Moeller;
    intersectors.intersector4_filter   = BVH4Triangle8Intersector4ChunkMoeller;
    intersectors.intersector4_nofilter = BVH4Triangle8Intersector4ChunkMoellerNoFilter;
    intersectors.intersector8 = BVH4Triangle8Intersector8ChunkMoeller;
    intersectors.intersector8_filter   = BVH4Triangle8Intersector8ChunkMoeller;
    intersectors.intersector8_nofilter = BVH4Triangle8Intersector8ChunkMoellerNoFilter;
    intersectors.intersector16 = NULL;
    return intersectors;
  }

  Accel::Intersectors BVH4Triangle8IntersectorsHybrid(BVH4* bvh)
  {
    Accel::Intersectors intersectors;
    intersectors.ptr = bvh;
    intersectors.intersector1 = BVH4Triangle8Intersector1Moeller;
    intersectors.intersector4_filter   = BVH4Triangle8Intersector4HybridMoeller;
    intersectors.intersector4_nofilter = BVH4Triangle8Intersector4HybridMoellerNoFilter;
    intersectors.intersector8_filter   = BVH4Triangle8Intersector8HybridMoeller;
    intersectors.intersector8_nofilter = BVH4Triangle8Intersector8HybridMoellerNoFilter;
    intersectors.intersector16 = NULL;
    return intersectors;
  }

  Accel::Intersectors BVH4Triangle1vIntersectors(BVH4* bvh)
  {
    Accel::Intersectors intersectors;
    intersectors.ptr = bvh;
    intersectors.intersector1 = BVH4Triangle1vIntersector1Pluecker;
    intersectors.intersector4 = BVH4Triangle1vIntersector4ChunkPluecker;
    intersectors.intersector8 = BVH4Triangle1vIntersector8ChunkPluecker;
    intersectors.intersector16 = NULL;
    return intersectors;
  }

  Accel::Intersectors BVH4Triangle4vIntersectorsChunk(BVH4* bvh)
  {
    Accel::Intersectors intersectors;
    intersectors.ptr = bvh;
    intersectors.intersector1 = BVH4Triangle4vIntersector1Pluecker;
    intersectors.intersector4 = BVH4Triangle4vIntersector4ChunkPluecker;
    intersectors.intersector8 = BVH4Triangle4vIntersector8HybridPluecker;
    intersectors.intersector16 = NULL;
    return intersectors;
  }

  Accel::Intersectors BVH4Triangle4vIntersectorsHybrid(BVH4* bvh)
  {
    Accel::Intersectors intersectors;
    intersectors.ptr = bvh;
    intersectors.intersector1 = BVH4Triangle4vIntersector1Pluecker;
    intersectors.intersector4 = BVH4Triangle4vIntersector4HybridPluecker;
    intersectors.intersector8 = BVH4Triangle4vIntersector8HybridPluecker;
    intersectors.intersector16 = NULL;
    return intersectors;
  }

  Accel::Intersectors BVH4Triangle4iIntersectors(BVH4* bvh)
  {
    Accel::Intersectors intersectors;
    intersectors.ptr = bvh;
    intersectors.intersector1 = BVH4Triangle4iIntersector1Pluecker;
    intersectors.intersector4 = BVH4Triangle4iIntersector4ChunkPluecker;
    intersectors.intersector8 = BVH4Triangle4iIntersector8ChunkPluecker;
    intersectors.intersector16 = NULL;
    return intersectors;
  }

  Accel* BVH4::BVH4Bezier1(Scene* scene)
  { 
    BVH4* accel = new BVH4(Bezier1Type::type,scene);
    Accel::Intersectors intersectors = BVH4Bezier1Intersectors(accel);
    Builder* builder = BVH4Bezier1BuilderFast(accel,scene,0);
    return new AccelInstance(accel,builder,intersectors);
  }

  Accel* BVH4::BVH4Bezier1i(Scene* scene)
  { 
    BVH4* accel = new BVH4(SceneBezier1i::type,scene);
    Accel::Intersectors intersectors = BVH4Bezier1iIntersectors(accel);
    Builder* builder = BVH4Bezier1iBuilderFast(accel,scene,0);
    scene->needVertices = true;
    return new AccelInstance(accel,builder,intersectors);
  }

  Accel* BVH4::BVH4Triangle1(Scene* scene)
  { 
    BVH4* accel = new BVH4(SceneTriangle1::type,scene);
    Accel::Intersectors intersectors = BVH4Triangle1Intersectors(accel);
    
    Builder* builder = NULL;
    if      (g_tri_builder == "default"     ) builder = BVH4Triangle1Builder(accel,scene,0);
    else if (g_tri_builder == "spatialsplit") builder = BVH4Triangle1Builder(accel,scene,1);
    else if (g_tri_builder == "objectsplit" ) builder = BVH4Triangle1Builder(accel,scene,0);
    else if (g_tri_builder == "morton"      ) builder = BVH4Triangle1BuilderMorton(accel,scene,0);
    else if (g_tri_builder == "fast"        ) builder = BVH4Triangle1BuilderFast(accel,scene,0);
    else throw std::runtime_error("unknown builder "+g_tri_builder+" for BVH4<Triangle1>");

    return new AccelInstance(accel,builder,intersectors);
  }

  Accel* BVH4::BVH4Triangle4(Scene* scene)
  { 
    BVH4* accel = new BVH4(SceneTriangle4::type,scene);

    Accel::Intersectors intersectors;
    if      (g_tri_traverser == "default") intersectors = BVH4Triangle4IntersectorsHybrid(accel);
    else if (g_tri_traverser == "chunk"  ) intersectors = BVH4Triangle4IntersectorsChunk(accel);
    else if (g_tri_traverser == "hybrid" ) intersectors = BVH4Triangle4IntersectorsHybrid(accel);
    else throw std::runtime_error("unknown traverser "+g_tri_traverser+" for BVH4<Triangle4>");
   
    Builder* builder = NULL;
    if      (g_tri_builder == "default"     ) builder = BVH4Triangle4Builder(accel,scene,0);
    else if (g_tri_builder == "spatialsplit") builder = BVH4Triangle4Builder(accel,scene,1);
    else if (g_tri_builder == "objectsplit" ) builder = BVH4Triangle4Builder(accel,scene,0);
    else if (g_tri_builder == "morton"      ) builder = BVH4Triangle4BuilderMorton(accel,scene,0);
    else if (g_tri_builder == "fast"        ) builder = BVH4Triangle4BuilderFast(accel,scene,0);
    else throw std::runtime_error("unknown builder "+g_tri_builder+" for BVH4<Triangle4>");

    return new AccelInstance(accel,builder,intersectors);
  }

#if defined (__TARGET_AVX__)

  Accel* BVH4::BVH4Triangle8(Scene* scene)
  { 
    BVH4* accel = new BVH4(SceneTriangle8::type,scene);

    Accel::Intersectors intersectors;
    if      (g_tri_traverser == "default") intersectors = BVH4Triangle8IntersectorsHybrid(accel);
    else if (g_tri_traverser == "chunk"  ) intersectors = BVH4Triangle8IntersectorsChunk(accel);
    else if (g_tri_traverser == "hybrid" ) intersectors = BVH4Triangle8IntersectorsHybrid(accel);
    else throw std::runtime_error("unknown traverser "+g_tri_traverser+" for BVH4<Triangle8>");
   
    Builder* builder = NULL;
    if      (g_tri_builder == "default"     ) builder = BVH4Triangle8Builder(accel,scene,0);
    else if (g_tri_builder == "spatialsplit") builder = BVH4Triangle8Builder(accel,scene,1);
    else if (g_tri_builder == "objectsplit" ) builder = BVH4Triangle8Builder(accel,scene,0);
    else if (g_tri_builder == "morton"      ) builder = BVH4Triangle8BuilderMorton(accel,scene,0);
    else if (g_tri_builder == "fast"        ) builder = BVH4Triangle8BuilderFast(accel,scene,0);
    else throw std::runtime_error("unknown builder "+g_tri_builder+" for BVH4<Triangle8>");

    return new AccelInstance(accel,builder,intersectors);
  }
#endif

  Accel* BVH4::BVH4Triangle1v(Scene* scene)
  {
    BVH4* accel = new BVH4(SceneTriangle1v::type,scene);
    Accel::Intersectors intersectors = BVH4Triangle1vIntersectors(accel);

    Builder* builder = NULL;
    if      (g_tri_builder == "default"     ) builder = BVH4Triangle1vBuilder(accel,scene,0);
    else if (g_tri_builder == "spatialsplit") builder = BVH4Triangle1vBuilder(accel,scene,1);
    else if (g_tri_builder == "objectsplit" ) builder = BVH4Triangle1vBuilder(accel,scene,0);
    else if (g_tri_builder == "morton"      ) builder = BVH4Triangle1vBuilderMorton(accel,scene,0);
    else if (g_tri_builder == "fast"        ) builder = BVH4Triangle1vBuilderFast(accel,scene,0);
    else throw std::runtime_error("unknown builder "+g_tri_builder+" for BVH4<Triangle1v>");
        
    return new AccelInstance(accel,builder,intersectors);
  }

  Accel* BVH4::BVH4Triangle4v(Scene* scene)
  {
    BVH4* accel = new BVH4(SceneTriangle4v::type,scene);

    Accel::Intersectors intersectors;
    if      (g_tri_traverser == "default") intersectors = BVH4Triangle4vIntersectorsHybrid(accel);
    else if (g_tri_traverser == "chunk"  ) intersectors = BVH4Triangle4vIntersectorsChunk(accel);
    else if (g_tri_traverser == "hybrid" ) intersectors = BVH4Triangle4vIntersectorsHybrid(accel);
    else throw std::runtime_error("unknown traverser "+g_tri_traverser+" for BVH4<Triangle4>");

    Builder* builder = NULL;
    if      (g_tri_builder == "default"     ) builder = BVH4Triangle4vBuilder(accel,scene,0);
    else if (g_tri_builder == "spatialsplit") builder = BVH4Triangle4vBuilder(accel,scene,1);
    else if (g_tri_builder == "objectsplit" ) builder = BVH4Triangle4vBuilder(accel,scene,0);
    else if (g_tri_builder == "morton"      ) builder = BVH4Triangle4vBuilderMorton(accel,scene,0);
    else if (g_tri_builder == "fast"        ) builder = BVH4Triangle4vBuilderFast(accel,scene,0);
    else throw std::runtime_error("unknown builder "+g_tri_builder+" for BVH4<Triangle4v>");

    return new AccelInstance(accel,builder,intersectors);
  }

  Accel* BVH4::BVH4Triangle4i(Scene* scene)
  {
    BVH4* accel = new BVH4(SceneTriangle4i::type,scene);
    Accel::Intersectors intersectors = BVH4Triangle4iIntersectors(accel);

    Builder* builder = NULL;
    if      (g_tri_builder == "default"     ) builder = BVH4Triangle4iBuilder(accel,scene,0);
    else if (g_tri_builder == "spatialsplit") builder = BVH4Triangle4iBuilder(accel,scene,1);
    else if (g_tri_builder == "objectsplit" ) builder = BVH4Triangle4iBuilder(accel,scene,0);
    else if (g_tri_builder == "fast"        ) builder = BVH4Triangle4iBuilderFast(accel,scene,0);
    else if (g_tri_builder == "morton"      ) builder = BVH4Triangle4iBuilderMorton(accel,scene,0);
    else throw std::runtime_error("unknown builder "+g_tri_builder+" for BVH4<Triangle4i>");

    scene->needVertices = true;
    return new AccelInstance(accel,builder,intersectors);
  }

  void createTriangleMeshTriangle1Morton(TriangleMesh* mesh, BVH4*& accel, Builder*& builder)
  {
    if (mesh->numTimeSteps != 1) throw std::runtime_error("internal error");
    accel = new BVH4(TriangleMeshTriangle1::type,mesh->parent);
    builder = BVH4Triangle1MeshBuilderMorton(accel,mesh,0);
  } 

  void createTriangleMeshTriangle1(TriangleMesh* mesh, BVH4*& accel, Builder*& builder)
  {
    if (mesh->numTimeSteps != 1) throw std::runtime_error("internal error");
    accel = new BVH4(TriangleMeshTriangle1::type,mesh->parent);
    switch (mesh->flags) {
    case RTC_GEOMETRY_STATIC:     builder = BVH4Triangle1MeshBuilderFast(accel,mesh,0); break;
    case RTC_GEOMETRY_DEFORMABLE: builder = BVH4Triangle1MeshRefitFast(accel,mesh,0); break;
    case RTC_GEOMETRY_DYNAMIC:    builder = BVH4Triangle1MeshBuilderMorton(accel,mesh,0); break;
    default: throw std::runtime_error("internal error"); 
    }
  } 

  void createTriangleMeshTriangle4(TriangleMesh* mesh, BVH4*& accel, Builder*& builder)
  {
    if (mesh->numTimeSteps != 1) throw std::runtime_error("internal error");
    accel = new BVH4(TriangleMeshTriangle4::type,mesh->parent);
    switch (mesh->flags) {
    case RTC_GEOMETRY_STATIC:     builder = BVH4Triangle4MeshBuilderFast(accel,mesh,0); break;
    case RTC_GEOMETRY_DEFORMABLE: builder = BVH4Triangle4MeshRefitFast(accel,mesh,0); break;
    case RTC_GEOMETRY_DYNAMIC:    builder = BVH4Triangle4MeshBuilderMorton(accel,mesh,0); break;
    default: throw std::runtime_error("internal error"); 
    }
  } 

  void createTriangleMeshTriangle1v(TriangleMesh* mesh, BVH4*& accel, Builder*& builder)
  {
    if (mesh->numTimeSteps != 1) throw std::runtime_error("internal error");
    accel = new BVH4(TriangleMeshTriangle1v::type,mesh->parent);
    switch (mesh->flags) {
    case RTC_GEOMETRY_STATIC:     builder = BVH4Triangle1vMeshBuilderFast(accel,mesh,0); break;
    case RTC_GEOMETRY_DEFORMABLE: builder = BVH4Triangle1vMeshRefitFast  (accel,mesh,0); break;
    case RTC_GEOMETRY_DYNAMIC:    builder = BVH4Triangle1vMeshBuilderMorton(accel,mesh,0); break;
    default: throw std::runtime_error("internal error"); 
    }
  } 

  void createTriangleMeshTriangle4v(TriangleMesh* mesh, BVH4*& accel, Builder*& builder)
  {
    if (mesh->numTimeSteps != 1) throw std::runtime_error("internal error");
    accel = new BVH4(TriangleMeshTriangle4v::type,mesh->parent);
    switch (mesh->flags) {
    case RTC_GEOMETRY_STATIC:     builder = BVH4Triangle4vMeshBuilderFast(accel,mesh,0); break;
    case RTC_GEOMETRY_DEFORMABLE: builder = BVH4Triangle4vMeshRefitFast(accel,mesh,0); break;
    case RTC_GEOMETRY_DYNAMIC:    builder = BVH4Triangle4vMeshBuilderMorton(accel,mesh,0); break;
    default: throw std::runtime_error("internal error"); 
    }
  } 

  void createTriangleMeshTriangle4i(TriangleMesh* mesh, BVH4*& accel, Builder*& builder)
  {
    if (mesh->numTimeSteps != 1) throw std::runtime_error("internal error");
    accel = new BVH4(TriangleMeshTriangle4i::type,mesh->parent);
    switch (mesh->flags) {
    case RTC_GEOMETRY_STATIC:     builder = BVH4Triangle4iMeshBuilderFast(accel,mesh,0); break;
    case RTC_GEOMETRY_DEFORMABLE: builder = BVH4Triangle4iMeshRefitFast(accel,mesh,0); break;
    case RTC_GEOMETRY_DYNAMIC:    builder = BVH4Triangle4iMeshBuilderMorton(accel,mesh,0); break;
    default: throw std::runtime_error("internal error"); 
    }
  } 

  Accel* BVH4::BVH4BVH4Triangle1Morton(Scene* scene)
  {
    BVH4* accel = new BVH4(TriangleMeshTriangle1::type,scene);
    Accel::Intersectors intersectors = BVH4Triangle1Intersectors(accel);
    Builder* builder = BVH4BuilderTopLevelFast(accel,scene,&createTriangleMeshTriangle1Morton);
    return new AccelInstance(accel,builder,intersectors);
  }

  Accel* BVH4::BVH4BVH4Triangle1ObjectSplit(Scene* scene)
  {
    BVH4* accel = new BVH4(TriangleMeshTriangle1::type,scene);
    Accel::Intersectors intersectors = BVH4Triangle1Intersectors(accel);
    Builder* builder = BVH4BuilderTopLevelFast(accel,scene,&createTriangleMeshTriangle1);
    return new AccelInstance(accel,builder,intersectors);
  }

  Accel* BVH4::BVH4BVH4Triangle4ObjectSplit(Scene* scene)
  {
    BVH4* accel = new BVH4(TriangleMeshTriangle4::type,scene);
    Accel::Intersectors intersectors = BVH4Triangle4IntersectorsHybrid(accel);
    Builder* builder = BVH4BuilderTopLevelFast(accel,scene,&createTriangleMeshTriangle4);
    return new AccelInstance(accel,builder,intersectors);
  }

  Accel* BVH4::BVH4BVH4Triangle1vObjectSplit(Scene* scene)
  {
    BVH4* accel = new BVH4(TriangleMeshTriangle1v::type,scene);
    Accel::Intersectors intersectors = BVH4Triangle1vIntersectors(accel);
    Builder* builder = BVH4BuilderTopLevelFast(accel,scene,&createTriangleMeshTriangle1v);
    return new AccelInstance(accel,builder,intersectors);
  }

  Accel* BVH4::BVH4BVH4Triangle4vObjectSplit(Scene* scene)
  {
    BVH4* accel = new BVH4(TriangleMeshTriangle4v::type,scene);
    Accel::Intersectors intersectors = BVH4Triangle4vIntersectorsHybrid(accel);
    Builder* builder = BVH4BuilderTopLevelFast(accel,scene,&createTriangleMeshTriangle4v);
    return new AccelInstance(accel,builder,intersectors);
  }

  Accel* BVH4::BVH4BVH4Triangle4iObjectSplit(Scene* scene)
  {
    BVH4* accel = new BVH4(TriangleMeshTriangle4i::type,scene);
    Accel::Intersectors intersectors = BVH4Triangle4iIntersectors(accel);
    Builder* builder = BVH4BuilderTopLevelFast(accel,scene,&createTriangleMeshTriangle4i);
    return new AccelInstance(accel,builder,intersectors);
  }

  Accel* BVH4::BVH4Triangle1SpatialSplit(Scene* scene)
  {
    BVH4* accel = new BVH4(SceneTriangle1::type,scene);
    Builder* builder = BVH4Triangle1Builder(accel,scene,1);
    Accel::Intersectors intersectors = BVH4Triangle1Intersectors(accel);
    return new AccelInstance(accel,builder,intersectors);
  }
  
  Accel* BVH4::BVH4Triangle4SpatialSplit(Scene* scene)
  {
    BVH4* accel = new BVH4(SceneTriangle4::type,scene);
    Builder* builder = BVH4Triangle4Builder(accel,scene,1);
    Accel::Intersectors intersectors = BVH4Triangle4IntersectorsHybrid(accel);
    return new AccelInstance(accel,builder,intersectors);
  }

#if defined (__TARGET_AVX__)

  Accel* BVH4::BVH4Triangle8SpatialSplit(Scene* scene)
  {
    BVH4* accel = new BVH4(SceneTriangle8::type,scene);
    Builder* builder = BVH4Triangle8Builder(accel,scene,1);
    Accel::Intersectors intersectors = BVH4Triangle8IntersectorsHybrid(accel);
    return new AccelInstance(accel,builder,intersectors);
  }

#endif

  Accel* BVH4::BVH4Triangle1ObjectSplit(Scene* scene)
  {
    BVH4* accel = new BVH4(SceneTriangle1::type,scene);
    Builder* builder = BVH4Triangle1Builder(accel,scene,0);
    Accel::Intersectors intersectors = BVH4Triangle1Intersectors(accel);
    return new AccelInstance(accel,builder,intersectors);
  }
  
  Accel* BVH4::BVH4Triangle4ObjectSplit(Scene* scene)
  {
    BVH4* accel = new BVH4(SceneTriangle4::type,scene);
    Builder* builder = BVH4Triangle4Builder(accel,scene,0);
    Accel::Intersectors intersectors = BVH4Triangle4IntersectorsHybrid(accel);
    return new AccelInstance(accel,builder,intersectors);
  }

#if defined (__TARGET_AVX__)

  Accel* BVH4::BVH4Triangle8ObjectSplit(Scene* scene)
  {
    BVH4* accel = new BVH4(SceneTriangle8::type,scene);
    Builder* builder = BVH4Triangle8Builder(accel,scene,0);
    Accel::Intersectors intersectors = BVH4Triangle8IntersectorsHybrid(accel);
    return new AccelInstance(accel,builder,intersectors);
  }

#endif

  Accel* BVH4::BVH4Triangle1vObjectSplit(Scene* scene)
  {
    BVH4* accel = new BVH4(SceneTriangle1v::type,scene);
    Builder* builder = BVH4Triangle1vBuilder(accel,scene,0);
    Accel::Intersectors intersectors = BVH4Triangle1vIntersectors(accel);
    return new AccelInstance(accel,builder,intersectors);
  }

  Accel* BVH4::BVH4Triangle4vObjectSplit(Scene* scene)
  {
    BVH4* accel = new BVH4(SceneTriangle4v::type,scene);
    Builder* builder = BVH4Triangle4vBuilder(accel,scene,0);
    Accel::Intersectors intersectors = BVH4Triangle4vIntersectorsHybrid(accel);
    return new AccelInstance(accel,builder,intersectors);
  }

  Accel* BVH4::BVH4Triangle4iObjectSplit(Scene* scene)
  {
    BVH4* accel = new BVH4(SceneTriangle4i::type,scene);
    Builder* builder = BVH4Triangle4iBuilder(accel,scene,0);
    Accel::Intersectors intersectors = BVH4Triangle4iIntersectors(accel);
    scene->needVertices = true;
    return new AccelInstance(accel,builder,intersectors);
  }

  struct VirtualAccelObjectType : public PrimitiveType
  {
    VirtualAccelObjectType () 
      : PrimitiveType("object",sizeof(AccelSetItem),1,false,1) {} 
    
    size_t blocks(size_t x) const {
      return x;
    }
    
    size_t size(const char* This) const {
      return 1;
    }
    
    void pack(char* This, atomic_set<PrimRefBlock>::block_iterator_unsafe& prims, void* geom) const {}
  };

  static VirtualAccelObjectType virtual_accel_object_ty;

  Accel* BVH4::BVH4UserGeometry(Scene* scene)
  {
    BVH4* accel = new BVH4(virtual_accel_object_ty);
    Accel::Intersectors intersectors;
    intersectors.ptr = accel; 
    intersectors.intersector1 = BVH4VirtualIntersector1;
    intersectors.intersector4 = BVH4VirtualIntersector4Chunk;
    intersectors.intersector8 = BVH4VirtualIntersector8Chunk;
    intersectors.intersector16 = NULL;
    Builder* builder = BVH4UserGeometryBuilderFast(accel,scene,0);
    return new AccelInstance(accel,builder,intersectors);
  }

  Accel* BVH4::BVH4Triangle1ObjectSplit(TriangleMesh* mesh)
  {
    BVH4* accel = new BVH4(TriangleMeshTriangle1::type,mesh->parent);
    Builder* builder = BVH4Triangle1MeshBuilderFast(accel,mesh,0);
    Accel::Intersectors intersectors = BVH4Triangle1Intersectors(accel);
    return new AccelInstance(accel,builder,intersectors);
  }

  Accel* BVH4::BVH4Triangle4ObjectSplit(TriangleMesh* mesh)
  {
    BVH4* accel = new BVH4(TriangleMeshTriangle4::type,mesh->parent);
    Builder* builder = BVH4Triangle4MeshBuilderFast(accel,mesh,0);
    Accel::Intersectors intersectors = BVH4Triangle4IntersectorsHybrid(accel);
    return new AccelInstance(accel,builder,intersectors);
  }

  Accel* BVH4::BVH4Triangle1vObjectSplit(TriangleMesh* mesh)
  {
    BVH4* accel = new BVH4(TriangleMeshTriangle1v::type,mesh->parent);
    Builder* builder = BVH4Triangle1vMeshBuilderFast(accel,mesh,0);
    Accel::Intersectors intersectors = BVH4Triangle1vIntersectors(accel);
    return new AccelInstance(accel,builder,intersectors);
  }

  Accel* BVH4::BVH4Triangle4vObjectSplit(TriangleMesh* mesh)
  {
    BVH4* accel = new BVH4(TriangleMeshTriangle4v::type,mesh->parent);
    Builder* builder = BVH4Triangle4vMeshBuilderFast(accel,mesh,0);
    Accel::Intersectors intersectors = BVH4Triangle4vIntersectorsHybrid(accel);
    return new AccelInstance(accel,builder,intersectors);
  }

  Accel* BVH4::BVH4Triangle4Refit(TriangleMesh* mesh)
  {
    BVH4* accel = new BVH4(TriangleMeshTriangle4::type,mesh->parent);
    Builder* builder = BVH4Triangle4MeshRefitFast(accel,mesh,0);
    Accel::Intersectors intersectors = BVH4Triangle4IntersectorsHybrid(accel);
    return new AccelInstance(accel,builder,intersectors);
  }
}

