// ======================================================================== //
// Copyright 2009-2013 Intel Corporation                                    //
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

#include "heuristic_spatial_split.h"

namespace embree
{
  const SpatialSplit SpatialSplit::find(size_t threadIndex, size_t depth, size_t size, BezierRefList& prims, const LinearSpace3fa& space)
  {
    /* calculate geometry and centroid bounds */
    BBox3fa geomBounds = empty;
    for (BezierRefList::block_iterator_unsafe i = prims; i; i++)
      geomBounds.extend(i->bounds(space));

    /* calculate binning function */
    const ssef ofs  = (ssef) geomBounds.lower;
    const ssef diag = (ssef) geomBounds.size();
    const ssef scale = select(diag != 0.0f,rcp(diag) * ssef(BINS * 0.99f),ssef(0.0f));

    /* initialize bins */
    BBox3fa bounds[BINS][4];
    ssei    numBegin[BINS];
    ssei    numEnd[BINS];
    for (size_t i=0; i<BINS; i++) {
      bounds[i][0] = bounds[i][1] = bounds[i][2] = bounds[i][3] = empty;
      numBegin[i] = numEnd[i] = 0;
    }
 
    /* perform binning of curves */
    for (BezierRefList::block_iterator_unsafe i = prims; i; i++)
    {
      const Vec3fa v0 = xfmPoint(space,i->p0);
      const ssei bin0 = clamp(floori((ssef(v0)-ofs)*scale),ssei(0),ssei(BINS-1));
      //const ssei bin0 = floori((ssef(v0)-ofs)*scale);
      assert(bin0[0] >=0 && bin0[0] < BINS);
      assert(bin0[1] >=0 && bin0[1] < BINS);
      assert(bin0[2] >=0 && bin0[2] < BINS);
      const Vec3fa v1 = xfmPoint(space,i->p3);
      const ssei bin1 = clamp(floori((ssef(v1)-ofs)*scale),ssei(0),ssei(BINS-1));
      //const ssei bin1 = floori((ssef(v1)-ofs)*scale);
      assert(bin1[0] >=0 && bin1[0] < BINS);
      assert(bin1[1] >=0 && bin1[1] < BINS);
      assert(bin1[2] >=0 && bin1[2] < BINS);
      const ssei startbin = min(bin0,bin1);
      const ssei endbin   = max(bin0,bin1);

      for (size_t dim=0; dim<3; dim++) 
      {
        size_t bin;
        Bezier1 curve = *i;
        for (bin=startbin[dim]; bin<endbin[dim]; bin++) // FIXME: one can prevent many transformations in this loop here !!!
        {
          const float pos = float(bin+1)/scale[dim]+ofs[dim];
          const Vec3fa plane(space.vx[dim],space.vy[dim],space.vz[dim],-pos);
          Bezier1 bincurve,restcurve; 
          if (curve.split(plane,bincurve,restcurve)) {
            const BBox3fa cbounds = bincurve.bounds(space);
            bounds[bin][dim].extend(cbounds);
            curve = restcurve;
          }
        }
        numBegin[startbin[dim]][dim]++;
        numEnd  [endbin  [dim]][dim]++;
        const BBox3fa cbounds = curve.bounds(space);
        bounds[bin][dim].extend(cbounds);
      }
    }
    
    /* sweep from right to left and compute parallel prefix of merged bounds */
    ssef rAreas[BINS];
    ssei rCounts[BINS];
    ssei count = 0; BBox3fa bx = empty; BBox3fa by = empty; BBox3fa bz = empty;
    for (size_t i=BINS-1; i>0; i--)
    {
      count += numEnd[i];
      rCounts[i] = count;
      bx.extend(bounds[i][0]); rAreas[i][0] = halfArea(bx);
      by.extend(bounds[i][1]); rAreas[i][1] = halfArea(by);
      bz.extend(bounds[i][2]); rAreas[i][2] = halfArea(bz);
    }
    
    /* sweep from left to right and compute SAH */
    ssei ii = 1; ssef bestSAH = pos_inf; ssei bestPos = 0; ssei bestLeft = 0; ssei bestRight = 0;
    count = 0; bx = empty; by = empty; bz = empty;
    for (size_t i=1; i<BINS; i++, ii+=1)
    {
      count += numBegin[i-1];
      bx.extend(bounds[i-1][0]); float Ax = halfArea(bx);
      by.extend(bounds[i-1][1]); float Ay = halfArea(by);
      bz.extend(bounds[i-1][2]); float Az = halfArea(bz);
      const ssef lArea = ssef(Ax,Ay,Az,Az);
      const ssef rArea = rAreas[i];
      const ssei lCount = blocks(count);
      const ssei rCount = blocks(rCounts[i]);
      const ssef sah = lArea*ssef(lCount) + rArea*ssef(rCount);
      bestPos  = select(sah < bestSAH,ii ,bestPos);
      bestLeft = select(sah < bestSAH,count,bestLeft);
      bestRight= select(sah < bestSAH,rCounts[i],bestRight);
      bestSAH  = select(sah < bestSAH,sah,bestSAH);
    }
    
    /* find best dimension */
    SpatialSplit split;
    split.space = space;
    split.ofs = ofs;
    split.scale = scale;
    split.cost = inf;
    split.dim = -1;
    split.pos = 0.0f;
    split.num0 = split.num1 = 1;
    split.bounds0 = split.bounds1 = BBox3fa(inf);

    float bestCost = inf;
    for (size_t dim=0; dim<3; dim++) 
    {
      /* ignore zero sized dimensions */
      if (unlikely(scale[dim] == 0.0f)) 
        continue;
      
      /* test if this is a better dimension */
      if (bestSAH[dim] < bestCost && bestPos[dim] != 0) {
        split.dim = dim;
        split.pos = bestPos[dim]/scale[dim]+ofs[dim];
        split.cost = bestSAH[dim];
        split.num0 = bestLeft[dim];
        split.num1 = bestRight[dim];
        bestCost = bestSAH[dim];
      }
    }

    /* compute bounds of left and right side */
    if (split.dim == -1)
      return split;

    /* calculate bounding box of left and right side */
    BBox3fa lbounds = empty, rbounds = empty;
    float   larea   = 0.0f,  rarea   = 0.0f;
    size_t lnum = 0, rnum = 0;

    for (BezierRefList::block_iterator_unsafe i = prims; i; i++)
    {
      const Vec3fa plane(space.vx[split.dim],space.vy[split.dim],space.vz[split.dim],-split.pos);
      
      const float p0p = dot(i->p0,plane)+plane.w;
      const float p3p = dot(i->p3,plane)+plane.w;

      /* sort to the left side */
      if (p0p <= 0.0f && p3p <= 0.0f) {
        const BBox3fa bounds = i->bounds(space);
        lbounds.extend(bounds);
        larea += halfArea(bounds);
        lnum++;
        continue;
      }
      
      /* sort to the right side */
      if (p0p >= 0.0f && p3p >= 0.0f) {
        const BBox3fa bounds = i->bounds(space);
        rbounds.extend(bounds);
        rarea += halfArea(bounds);
        rnum++;
        continue;
      }

      Bezier1 left,right; 
      if (i->split(plane,left,right)) {
        const BBox3fa lcbounds = left.bounds(space);
        const BBox3fa rcbounds = right.bounds(space);
        lbounds.extend(lcbounds); larea += halfArea(lcbounds); lnum++;
        rbounds.extend(rcbounds); rarea += halfArea(rcbounds); rnum++;
        continue;
      }
      
      const BBox3fa bounds = i->bounds(space);
      lbounds.extend(bounds); larea += halfArea(bounds); lnum++;
    }
    split.bounds0 = NAABBox3fa(space,lbounds);
    split.bounds1 = NAABBox3fa(space,rbounds);
    //assert(lnum == split.num0);
    //assert(rnum == split.num1);
    split.num0 = lnum;
    split.num1 = rnum;
    split.numReplications = split.num0 + split.num1 - size;
    assert(split.numReplications >= 0);
    //assert(lnum > 0);
    //assert(rnum > 0);

    if (split.num0 == 0 || split.num1 == 0) {
      split.cost = inf;
      split.num0 = split.num1 = 1;
      split.bounds0 = split.bounds1 = BBox3fa(inf);
    }

    return split;
  }
      
  void SpatialSplit::split(size_t threadIndex, PrimRefBlockAlloc<Bezier1>& alloc, BezierRefList& prims, BezierRefList& lprims_o, BezierRefList& rprims_o) const
  {
    /* calculate splitting plane */
    const Vec3fa plane(space.vx[dim],space.vy[dim],space.vz[dim],-pos);
    
    /* sort each curve to left, right, or left and right */
    BezierRefList::item* lblock = lprims_o.insert(alloc.malloc(threadIndex));
    BezierRefList::item* rblock = rprims_o.insert(alloc.malloc(threadIndex));
    
    while (BezierRefList::item* block = prims.take()) 
    {
      for (size_t i=0; i<block->size(); i++) 
      {
        const Bezier1& prim = block->at(i); 
        const float p0p = dot(prim.p0,plane)+plane.w;
        const float p3p = dot(prim.p3,plane)+plane.w;

        /* sort to the left side */
        if (p0p <= 0.0f && p3p <= 0.0f)
        {
          if (likely(lblock->insert(prim))) continue; 
          lblock = lprims_o.insert(alloc.malloc(threadIndex));
          lblock->insert(prim);
          continue;
        }

        /* sort to the right side */
        if (p0p >= 0.0f && p3p >= 0.0f)
        {
          if (likely(rblock->insert(prim))) continue;
          rblock = rprims_o.insert(alloc.malloc(threadIndex));
          rblock->insert(prim);
          continue;
        }

        /* split and sort to left and right */
        Bezier1 left,right;
        if (prim.split(plane,left,right)) 
        {
          if (!lblock->insert(left)) {
            lblock = lprims_o.insert(alloc.malloc(threadIndex));
            lblock->insert(left);
          }
          if (!rblock->insert(right)) {
            rblock = rprims_o.insert(alloc.malloc(threadIndex));
            rblock->insert(right);
          }
          continue;
        }

        /* insert to left side as fallback */
        if (!lblock->insert(prim)) {
          lblock = lprims_o.insert(alloc.malloc(threadIndex));
          lblock->insert(prim);
        }
      }
      alloc.free(threadIndex,block);
    }
    //assert(BezierRefList::block_iterator_unsafe(lprims).size() == num0);
    //assert(BezierRefList::block_iterator_unsafe(rprims).size() == num1);
  }
}
