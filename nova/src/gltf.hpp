#pragma once

#include "math.hpp"
#include <vector>

namespace gltf
{

enum class ComponentType : unsigned long
{
  SignedByte = 5120,
  UnsignedByte = 5121,
  SignedShort = 5122,
  UnsignedShort = 5123,
  UnsignedInt = 5125,
  Float = 5126
};

enum class ElementType
{
  AttrPosition,
  AttrNormal,
  AttrTexCoord,
  AttrJointIndices,
  AttrJointWeights,

  VertexIndex,

  JointInvertedBindMatrices,
  JointTranslation,
  JointRotation,
  JointScale
};

inline bool isAttribute(ElementType type)
{
  switch (type) {
    case ElementType::AttrPosition:
    case ElementType::AttrNormal:
    case ElementType::AttrTexCoord:
    case ElementType::AttrJointIndices:
    case ElementType::AttrJointWeights:
      return true;
    default:
      return false;
  }
}

struct BufferDesc
{
  ElementType type;
  uint32_t dimensions;
  ComponentType componentType;
  size_t size;
  size_t byteLength;
  size_t offset;
  size_t index;
};

struct MaterialDesc
{
  std::string name;
  std::string baseColourTexture;
  std::string normalMap;
  Vec4f baseColourFactor = { 1, 1, 1, 1 };
  float metallicFactor = 0.f;
  float roughnessFactor = 0.f;
  bool isDoubleSided = false;
};

struct MeshDesc
{
  std::vector<BufferDesc> buffers;
  MaterialDesc material;
  Mat4x4f transform;
};

struct NodeDesc
{
  unsigned long nodeIndex;
  Mat4x4f transform;
  std::vector<NodeDesc> children;
};

enum class TransformationType
{
  Rotation,
  Translation,
  Scale
};

struct AnimationChannelDesc
{
  size_t joint;
  TransformationType type;
  std::vector<float> times;
  std::vector<Vec4f> values;
};

struct AnimationDesc
{
  std::vector<AnimationChannelDesc> channels;
};

struct SkinDesc
{
  std::vector<unsigned long> joints;
  BufferDesc inverseBindMatricesBuffer;
};

struct ArmatureDesc
{
  NodeDesc root;
  std::vector<AnimationDesc> animations;
  SkinDesc skin;
};

struct ModelDesc
{
  std::vector<MeshDesc> meshes;
  ArmatureDesc armature;
  std::vector<std::string> buffers;
};

inline size_t getSize(ComponentType type)
{
  switch (type) {
    case ComponentType::SignedByte: return 1;
    case ComponentType::UnsignedByte: return 1;
    case ComponentType::SignedShort: return 2;
    case ComponentType::UnsignedShort: return 2;
    case ComponentType::UnsignedInt: return 4;
    case ComponentType::Float: return 4;
    default: EXCEPTION("Cannot get size of unknown type");
  }
}

ModelDesc extractModel(const std::vector<char>& jsonData);

} // namespace gltf
