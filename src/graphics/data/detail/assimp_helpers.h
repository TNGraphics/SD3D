//
// Created by Tobias on 10/27/2020.
//

#ifndef SD3D_ASSIMP_HELPERS_H
#define SD3D_ASSIMP_HELPERS_H

#include <assimp/scene.h>

#include "../Texture.h"

enum aiTextureType;

namespace sd3d::assimp {

constexpr Texture::Type from_assimp_type(aiTextureType type) {
	switch (type) {
	case aiTextureType_DIFFUSE:
		return Texture::Type::DIFFUSE;
	case aiTextureType_SPECULAR:
		return Texture::Type::SPECULAR;
		// TODO handle more types
	case aiTextureType_AMBIENT:
	case aiTextureType_EMISSIVE:
	case aiTextureType_HEIGHT:
	case aiTextureType_NORMALS:
	case aiTextureType_SHININESS:
	case aiTextureType_OPACITY:
	case aiTextureType_DISPLACEMENT:
	case aiTextureType_LIGHTMAP:
	case aiTextureType_REFLECTION:
	case aiTextureType_BASE_COLOR:
	case aiTextureType_NORMAL_CAMERA:
	case aiTextureType_EMISSION_COLOR:
	case aiTextureType_METALNESS:
	case aiTextureType_DIFFUSE_ROUGHNESS:
	case aiTextureType_AMBIENT_OCCLUSION:
	case aiTextureType_UNKNOWN:
	case _aiTextureType_Force32Bit:
	case aiTextureType_NONE:
	default:
		return Texture::Type::DIFFUSE;
	}
}

}

#endif // SD3D_ASSIMP_HELPERS_H
