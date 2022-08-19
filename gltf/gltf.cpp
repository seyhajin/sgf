#include "gltf.h"

#include <scene/scene.hh>

#include <stb/stb.hh>

#define TINYGLTF_NO_STB_IMAGE_WRITE
#define TINYGLTF_NO_INCLUDE_STB_IMAGE
#define TINYGLTF_NOEXCEPTION
#define TINYGLTF_IMPLEMENTATION
#include "tinygltf/tiny_gltf.h"

#define CDEBUG(EXPR)                                                                                                   \
	if (true) debug() << "### GLTF:" << __FILE__ << EXPR;

namespace sgf {

namespace {

Mat4f localMatrix(const tinygltf::Node& gltfNode) {

	Mat4f matrix;

	if (gltfNode.matrix.size() == 16) {
		for (int i = 0; i < 16; ++i) { matrix.data()[i] = gltfNode.matrix[i]; }
		matrix.i.z = -matrix.i.z;
		matrix.j.z = -matrix.j.z;
		matrix.k.x = -matrix.k.x;
		matrix.k.y = -matrix.k.y;
		matrix.t.z = -matrix.t.z;
		return matrix;
	}

	if (gltfNode.translation.size() == 3) {
		Vec3f trans = Vec3f(gltfNode.translation[0], gltfNode.translation[1], gltfNode.translation[2]);
		CDEBUG("Tranlsation" << trans);
		matrix *= Mat4f::translation(trans);
	}

	if (gltfNode.rotation.size() == 4) {
		Quatf rotation =
			Quatf(Vec3f(-gltfNode.rotation[0], -gltfNode.rotation[1], gltfNode.rotation[2]), -gltfNode.rotation[3]);
		CDEBUG("Rotation" << rotation);
		matrix *= Mat3f(rotation);
	}

	if (gltfNode.scale.size() == 3) {
		Vec3f scale = Vec3f(gltfNode.scale[0], gltfNode.scale[1], gltfNode.scale[2]);
		CDEBUG("Scale" << scale);
		matrix *= Mat4f::scale(scale);
	}

	return matrix;
}

Texture* loadTexture(const tinygltf::Texture& gltfTex, const tinygltf::Model& gltfModel) {

	assert(gltfTex.source >= 0);

	const auto& gltfImg = gltfModel.images[gltfTex.source];

	TextureFormat format;

	switch (gltfImg.component) {
	case 1:
		format = TextureFormat::intensity8;
		break;
	case 2:
		format = TextureFormat::intensityAlpha16;
		break;
	case 3:
		format = TextureFormat::srgb24;
		break;
	case 4:
		format = TextureFormat::srgba32;
		break;
	default:
		panic("OOPS");
	}

	assert(gltfImg.bits == 8);

	TextureFlags flags = TextureFlags::mipmap;

	if (gltfTex.sampler >= 0) {
		const auto& gltfSampler = gltfModel.samplers[gltfTex.sampler];
		switch (gltfSampler.wrapS) {
		case TINYGLTF_TEXTURE_WRAP_CLAMP_TO_EDGE:
			flags |= TextureFlags::clampS;
			break;
		}
		switch (gltfSampler.wrapT) {
		case TINYGLTF_TEXTURE_WRAP_CLAMP_TO_EDGE:
			flags |= TextureFlags::clampT;
			break;
		}
	}

	return graphicsDevice()->createTexture(gltfImg.width, gltfImg.height, format, flags, gltfImg.image.data());
}

Material* loadMaterial(const tinygltf::Material& gltfMat, const tinygltf::Model& gltfModel) {

	CDEBUG("Loading material" << gltfMat.name);

	auto material = new Material;

	material->blendMode = (gltfMat.alphaMode == "BLEND") ? BlendMode::alpha : BlendMode::disable;
	material->cullMode = gltfMat.doubleSided ? CullMode::disable : CullMode::back;
	material->flatShaded = false;

	{
		const auto& gltfTexInfo = gltfMat.pbrMetallicRoughness.baseColorTexture;
		if (gltfTexInfo.index >= 0) {
			const auto& gltfTex = gltfModel.textures[gltfTexInfo.index];
			material->baseColorTexture = loadTexture(gltfTex, gltfModel);
		}
		const double* p = gltfMat.pbrMetallicRoughness.baseColorFactor.data();
		material->baseColorFactor = Vec4f(p[0], p[1], p[2], p[3]);
	}

	{
		const auto& gltfTexInfo = gltfMat.emissiveTexture;
		if (gltfTexInfo.index >= 0) {
			const auto& gltfTex = gltfModel.textures[gltfTexInfo.index];
			material->emissiveTexture = loadTexture(gltfTex, gltfModel);
		}
		const double* p = gltfMat.emissiveFactor.data();
		material->emissiveFactor = Vec3f(p[0], p[1], p[2]);
	}

	return material;
}

Mesh* loadPrimitive(const tinygltf::Primitive& gltfPrim, const tinygltf::Model& gltfModel) {

	Material* material = nullptr;

	if (gltfPrim.material >= 0) {
		const auto& gltfMat = gltfModel.materials[gltfPrim.material];
		material = loadMaterial(gltfMat, gltfModel);
	}

	auto mesh = new Mesh(material);

	auto& vertices = mesh->vertices();
	auto& triangles = mesh->triangles();

	for (const auto& attrib : gltfPrim.attributes) {

		const auto& accessor = gltfModel.accessors[attrib.second];

		size_t count = accessor.count;
		if (count > vertices.size()) vertices.resize(count);
		Vertex* v = vertices.data();

		const auto& bufferView = gltfModel.bufferViews[accessor.bufferView];
		const auto& buffer = gltfModel.buffers[bufferView.buffer];
		uint8_t* src = (uint8_t*)buffer.data.data() + bufferView.byteOffset + accessor.byteOffset;

		int stride = accessor.ByteStride(bufferView);

		int size = (accessor.type != TINYGLTF_TYPE_SCALAR) ? accessor.type : 1;

		// debug() << "--- Loading gltf attrib" << attrib.first;

		if (attrib.first == "POSITION") {
			assert(size == 3 && accessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT);
			for (size_t i = 0; i < count; ++i) {
				v->position = *(Vec3f*)src;
				v->position.z = -v->position.z;
				src += stride;
				++v;
			}
		} else if (attrib.first == "NORMAL") {
			assert(size == 3 && accessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT);
			for (size_t i = 0; i < count; ++i) {
				v->normal = *(Vec3f*)src;
				v->normal.z = -v->normal.z;
				src += stride;
				++v;
			}
		} else if (attrib.first == "TEXCOORD_0") {
			assert(size == 2 && accessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT);
			for (size_t i = 0; i < count; ++i) {
				v->texCoords0 = *(Vec2f*)src;
				src += stride;
				++v;
			}
		} else if (attrib.first == "TEXCOORD_1") {
			assert(size == 2 && accessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT);
			//			debug() << "### TODO";
		} else if (attrib.first == "TANGENT") {
			assert(size == 4 && accessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT);
			//			debug() << "### TODO";
		} else {
			debug() << "!!! Gltf loader unrecognized attribute:" << attrib.first << "size" << size << "componentType"
					<< accessor.componentType;
			panic("TODO");
		}
	}

	// Triangles...
	const auto& accessor = gltfModel.accessors[gltfPrim.indices];

	size_t count = accessor.count / 3;
	assert(count * 3 == accessor.count);
	triangles.resize(count);
	Triangle* tp = triangles.data();

	const auto& bufferView = gltfModel.bufferViews[accessor.bufferView];
	const auto& buffer = gltfModel.buffers[bufferView.buffer];
	uint8_t* data = (uint8_t*)buffer.data.data() + bufferView.byteOffset + accessor.byteOffset;

	int stride = accessor.ByteStride(bufferView);

	if (accessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT) {
		assert(stride == 4);
		auto src = (uint*)data;
		for (size_t i = 0; i < count; ++i) {
			tp->v0 = src[0];
			tp->v1 = src[2];
			tp->v2 = src[1];
			src += 3;
			++tp;
		}
	} else if (accessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT) {
		assert(stride == 2);
		auto src = (uint16_t*)data;
		for (size_t i = 0; i < count; ++i) {
			tp->v0 = src[0];
			tp->v1 = src[2];
			tp->v2 = src[1];
			src += 3;
			++tp;
		}
	} else if (accessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE) {
		assert(stride == 1);
		auto src = (uint8_t*)data;
		for (size_t i = 0; i < count; ++i) {
			tp->v0 = src[0];
			tp->v1 = src[2];
			tp->v2 = src[1];
			src += 3;
			++tp;
		}
	} else {
		panic("OOPS");
	}

	return mesh;
}

Mesh* loadMesh(const tinygltf::Mesh& gltfMesh, const tinygltf::Model& gltfModel) {

	CDEBUG("Loading mesh" << gltfMesh.name);

	auto mesh = new Mesh;

	for (const auto& gltfPrim : gltfMesh.primitives) {
		if (gltfPrim.mode != TINYGLTF_MODE_TRIANGLES) continue;
		auto primMesh = loadPrimitive(gltfPrim, gltfModel);
		addMesh(primMesh, mesh);
	}

	return mesh;
}

void loadNode(Mesh* modelMesh, const tinygltf::Node& gltfNode, Mat4f matrix, const tinygltf::Model& gltfModel) {

	CDEBUG("Loading node" << gltfNode.name);

	matrix *= localMatrix(gltfNode);

	if (gltfNode.mesh >= 0) {
		assert(gltfNode.mesh < gltfModel.meshes.size());
		auto mesh = loadMesh(gltfModel.meshes[gltfNode.mesh], gltfModel);
		transformMesh(mesh, matrix);
		addMesh(mesh, modelMesh);
	}

	for (int i : gltfNode.children) {
		assert(i < gltfModel.nodes.size());
		loadNode(modelMesh, gltfModel.nodes[i], matrix, gltfModel);
	}
}

bool strEndsWith(CString str, CString ext) {
	return str.size() >= ext.size() && str.substr(str.size() - ext.size()) == ext;
}

Mesh* loadGltfMesh(CString assetPath) {

	CDEBUG("Loading model" << assetPath);

	std::string err;
	std::string warn;
	tinygltf::Model gltfModel;
	tinygltf::TinyGLTF gltfLoader;

	auto path = resolveAssetPath(assetPath);

	bool res;
	if (strEndsWith(path, ".gltf")) {
		res = gltfLoader.LoadASCIIFromFile(&gltfModel, &err, &warn, path);
	} else {
		res = gltfLoader.LoadBinaryFromFile(&gltfModel, &err, &warn, path);
	}
	if (!err.empty()) { debug() << "Tiny gltf error: " << err; }
	if (!warn.empty()) { //
		debug() << "Tiny gltf warning: " << warn << "\n\n";
	}
	if (!res) {
		debug() << "Tiny gltf failed to load file: " << assetPath;
		abort();
	}

	assert(gltfModel.defaultScene >= 0);
	const auto& gltfScene = gltfModel.scenes[gltfModel.defaultScene];

	CDEBUG("Loading scene" << gltfScene.name);

	auto modelMesh = new Mesh;

	for (int i : gltfScene.nodes) {
		assert(i < gltfModel.nodes.size());
		loadNode(modelMesh, gltfModel.nodes[i], {}, gltfModel);
	}

	return modelMesh;
}

} // namespace

Mesh* loadMesh(CString assetPath) {

	return loadGltfMesh(assetPath);
}

Model* loadModel(CString assetPath) {

	auto mesh = loadMesh(assetPath);

	return createModel(mesh);
}

} // namespace sgf
