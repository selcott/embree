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

#pragma once

#include "sys/platform.h"
#include "sys/filename.h"
#include "sys/stl/vector.h"
#include "math/vec2.h"
#include "math/vec3.h"

#include <vector>
#include <memory>

namespace embree
{
  /*! Scene representing the OBJ file */
  struct OBJScene 
  {
    OBJScene () {}
    ~OBJScene () 
    {
      for (size_t i=0; i<meshes.size(); i++)
        delete meshes[i];

      for (size_t i=0; i<hairsets.size(); i++)
        delete hairsets[i];
    }

    /*! OBJ Triangle */
    struct Triangle 
    {
    public:
      Triangle (int v0, int v1, int v2, int materialID) 
      : v0(v0), v1(v1), v2(v2), materialID(materialID) {}

    public:
      int v0, v1, v2, materialID;
    };

    /*! Mesh. */
    struct Mesh 
    {
#if 1 // FIXME: have to enable this for Visual Studio compiler
      vector_t<Vec3fa> v;
      vector_t<Vec3fa> vn;
#else
      std::vector<Vec3fa> v;
      std::vector<Vec3fa> vn;

#endif
      std::vector<Vec2f> vt;
      std::vector<Triangle> triangles;
    };

    struct Hair
    {
    public:
      Hair () {}
      Hair (int vertex, int id)
      : vertex(vertex), id(id) {}
    public:
      int vertex,id;  //!< index of first control point and hair ID
    };

    /*! Hair Set. */
    struct HairSet
    {
#if 1 // FIXME: have to enable this for Visual Studio compiler
      vector_t<Vec3fa> v;       //!< hair control points (x,y,z,r)
#else
      std::vector<Vec3fa> v;       //!< hair control points (x,y,z,r)
#endif
      std::vector<Hair> hairs;  //!< list of hairs
    };
    
    enum MaterialTy { MATERIAL_OBJ, MATERIAL_THIN_GLASS, MATERIAL_METAL };

    /*! OBJ material */
    struct OBJMaterial
    {
    public:
      OBJMaterial ()
      : ty(MATERIAL_OBJ), illum(0), d(1.f), Ns(1.f), Ni(1.f), Ka(0.f), Kd(1.f), Ks(0.f), Tf(1.f) {};

      OBJMaterial (float d, const Vec3fa& Kd, const Vec3fa& Ks, const float Ns)
      : ty(MATERIAL_OBJ), illum(0), d(d), Ns(Ns), Ni(1.f), Ka(0.f), Kd(Kd), Ks(Ks), Tf(1.f) {};
      
    public:
      int ty;
      int align[3];

      int illum;             /*< illumination model */
      float d;               /*< dissolve factor, 1=opaque, 0=transparent */
      float Ns;              /*< specular exponent */
      float Ni;              /*< optical density for the surface (index of refraction) */
      
      Vec3fa Ka;              /*< ambient reflectivity */
      Vec3fa Kd;              /*< diffuse reflectivity */
      Vec3fa Ks;              /*< specular reflectivity */
      Vec3fa Tf;              /*< transmission filter */
    };

    struct MetalMaterial
    {
    public:
      MetalMaterial ()
      : ty(MATERIAL_METAL), reflectance(1.0f), eta(1.4f), k(0.0f), roughness(0.01f) {}

      MetalMaterial (const Vec3fa& reflectance, const Vec3fa& eta, const Vec3fa& k, const float roughness)
      : ty(MATERIAL_METAL), reflectance(1.0f), eta(1.4f), k(0.0f), roughness(0.01f) {}
      
    public:
      int ty;
      int align[3];
      
      Vec3fa reflectance;
      Vec3fa eta;
      Vec3fa k;
      float roughness;
    };

    /*! Material */
    struct Material
    {
    public:
      Material () { memset(this,0,sizeof(Material)); }
      Material (const OBJMaterial& in) { *((OBJMaterial*)this) = in; }
      OBJMaterial& obj() { return *(OBJMaterial*)this; }
      
    public:
      int ty;
      int align[3];
      Vec3fa v[7];
    };

    struct AmbientLight
    {
    public:
      AmbientLight () {}

      AmbientLight (const Vec3fa& L) : L(L) {}

    public:
      Vec3fa L;                  //!< radiance of ambient light
    };

    struct PointLight
    {
    public:
      PointLight () {}

      PointLight (const Vec3fa& P, const Vec3fa& I) : P(P), I(I) {}

    public:
      Vec3fa P;                  //!< position of point light
      Vec3fa I;                  //!< radiant intensity of point light
    };

    struct DirectionalLight
    {
    public:
      DirectionalLight () {}

      DirectionalLight (const Vec3fa& D, const Vec3fa& E) : D(D), E(E) {}

    public:
      Vec3fa D;                  //!< Light direction
      Vec3fa E;                  //!< Irradiance (W/m^2)
    };

    struct DistantLight
    {
    public:
      DistantLight() {}

      DistantLight (const Vec3fa& D, const Vec3fa& L, const float halfAngle) 
      : D(D), L(L), halfAngle(halfAngle), radHalfAngle(deg2rad(halfAngle)), cosHalfAngle(cos(deg2rad(halfAngle))) {}

    public:
      Vec3fa D;             //!< Light direction
      Vec3fa L;             //!< Radiance (W/(m^2*sr))
      float halfAngle;     //!< Half illumination angle
      float radHalfAngle;  //!< Half illumination angle in radians
      float cosHalfAngle;  //!< Cosine of half illumination angle
    };

  public:
    vector_t<Material> materials;                      //!< material list
    std::vector<Mesh*> meshes;                         //!< list of meshes
    std::vector<HairSet*> hairsets;                    //!< list of hair sets
    vector_t<AmbientLight> ambientLights;           //!< list of ambient lights
    vector_t<PointLight> pointLights;               //!< list of point lights
    vector_t<DirectionalLight> directionalLights;   //!< list of directional lights
    vector_t<DistantLight> distantLights;           //!< list of distant lights
  };
  
  /*! read from disk */
  void loadOBJ(const FileName& fileName, OBJScene& mesh, const Vec3fa& offset = zero);
}
