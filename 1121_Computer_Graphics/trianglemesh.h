#ifndef TRIANGLEMESH_H
#define TRIANGLEMESH_H

#include "headers.h"
#include "material.h"

// VertexPTN Declarations.
struct VertexPTN
{
	VertexPTN() {
		position = glm::vec3(0.0f, 0.0f, 0.0f);
		texcoord = glm::vec2(0.0f, 0.0f);
		normal = glm::vec3(0.0f, 1.0f, 0.0f);
	}
	VertexPTN(glm::vec3 p, glm::vec2 uv, glm::vec3 n) {
		position = p;
		texcoord = uv;
		normal = n;
	}
	glm::vec3 position;
	glm::vec2 texcoord;
	glm::vec3 normal;
};

// SubMesh Declarations.
struct SubMesh
{
	SubMesh() {
		material = nullptr;
		iboId = 0;
	}
	SubMesh(PhongMaterial* material) : material(material), iboId(0) {}
	~SubMesh() {
		glDeleteBuffers(1, &iboId);
	}
	PhongMaterial* material;
	GLuint iboId;
	std::vector<unsigned int> vertexIndices;
};

// TriangleMesh Declarations.
class TriangleMesh
{
public:
	// TriangleMesh Public Methods.
	TriangleMesh();
	~TriangleMesh();
	
	// Load the model from an *.OBJ file.
	bool LoadFromFile(const std::string& filePath, const std::string& fileDirectory, const bool normalized = true);
	
	// Show model information.
	void ShowInfo();

	// Draw
	void Draw(const PhongShadingDemoShaderProg* shader);

	int GetNumVertices() const { return numVertices; }
	int GetNumTriangles() const { return numTriangles; }
	int GetNumSubMeshes() const { return (int)subMeshes.size(); }

	glm::vec3 GetObjCenter() const { return objCenter; }
	glm::vec3 GetObjExtent() const { return objExtent; }

private:
	bool LoadMaterials(const std::string& filePath, const std::string& fileDirectory, std::unordered_map<std::string, int>& materialIndex);

	// TriangleMesh Private Data.
	GLuint vboId;
	
	std::vector<VertexPTN> vertices;
	std::vector<PhongMaterial> materials;
	std::vector<SubMesh> subMeshes;

	int numVertices;
	int numTriangles;
	glm::vec3 objCenter;
	glm::vec3 objExtent;
};

#endif
