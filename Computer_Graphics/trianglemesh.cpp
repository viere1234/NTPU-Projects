#include "trianglemesh.h"

// Constructor of a triangle mesh.
TriangleMesh::TriangleMesh() 
{
	numTriangles = 0;
	numVertices = 0;
	vboId = 0;
	objCenter = glm::vec3(0.0f);
	objExtent = glm::vec3(0.0f);
}

// Destructor of a triangle mesh.
TriangleMesh::~TriangleMesh()
{
	glDeleteBuffers(1, &vboId);
	vertices.clear();
	materials.clear();
	subMeshes.clear();
}

// Load Materials
bool TriangleMesh::LoadMaterials(const std::string& filePath, const std::string& fileDirectory, std::unordered_map<std::string, int>& materialIndex) {
	std::ifstream file;
	file.open(filePath);
	if (file.fail()) {
		std::cout << "Failed to open material file." << std::endl;
		return false;
	}
	std::string line, token;
	glm::vec3 rgb;
	float Ns;
	while (std::getline(file, line)) {
		std::istringstream ss(line);
		ss >> token;
		if (token == "newmtl") {
			ss >> token;
			materialIndex[token] = this->materials.size();
			this->materials.emplace_back();
			this->materials.back().SetName(token);
		} 
		else if (token == "Ka") {
			ss >> rgb.r >> rgb.g >> rgb.b;
			this->materials.back().SetKa(rgb);
		}
		else if (token == "Kd") {
			ss >> rgb.r >> rgb.g >> rgb.b;
			this->materials.back().SetKd(rgb);
		}
		else if (token == "Ks") {
			ss >> rgb.r >> rgb.g >> rgb.b;
			this->materials.back().SetKs(rgb);
		}
		else if (token == "Ns") {
			ss >> Ns;
			this->materials.back().SetNs(Ns);
		}
		else if (token == "map_Kd") {
			ss >> token;
			token.insert(0, "/");
			token.insert(0, fileDirectory);
			this->materials.back().SetMapKd(new ImageTexture(token));
		}
	}
	file.close();
	return true;
}

// Load the geometry and material data from an OBJ file.
bool TriangleMesh::LoadFromFile(const std::string& filePath, const std::string& fileDirectory, const bool normalized)
{	
	// Load model file
	std::ifstream file;
	file.open(filePath);
	if (file.fail()) {
		std::cout << "Failed to open model file." << std::endl;
		return false;
	}
	std::vector<glm::vec3> position;
	std::vector<glm::vec2> texture;
	std::vector<glm::vec3> normal;
	glm::vec3 minVertex(FLT_MAX), maxVertex(-FLT_MAX); // bounding box
	std::unordered_map<std::string, int> materialIndex;
	std::unordered_map<std::string, int> vertexTrack;
	std::string line;
	while (std::getline(file, line)) {
		if (line.substr(0, 6) == "mtllib") { // load materials
			std::istringstream ss(line.substr(6));
			std::string materialFilePath;
			ss >> materialFilePath;
			materialFilePath.insert(0, "/");
			materialFilePath.insert(0, fileDirectory);
			if (!this->LoadMaterials(materialFilePath, fileDirectory, materialIndex)) {
				std::cout << "Failed to load material file." << std::endl;
				return false;
			}
		}
		else if (line.substr(0, 6) == "usemtl") { // set new sub mesh with material name
			std::istringstream ss(line.substr(6));
			std::string name;
			ss >> name;
			this->subMeshes.emplace_back(&this->materials[materialIndex[name]]);
		}
		else if (line.substr(0, 2) == "vt") { // texture
			std::istringstream ss(line.substr(2));
			glm::vec2 vt;
			ss >> vt.x;
			ss >> vt.y;
			texture.push_back(vt);
		}
		else if (line.substr(0, 2) == "vn") { // normal
			std::istringstream ss(line.substr(2));
			glm::vec3 vn;
			ss >> vn.x;
			ss >> vn.y;
			ss >> vn.z;
			normal.push_back(vn);
		}
		else if (line.substr(0, 1) == "v") { // position
			std::istringstream ss(line.substr(1));
			glm::vec3 v;
			ss >> v.x;
			ss >> v.y;
			ss >> v.z;
			position.push_back(v);
			// caculate bounding box
			if (minVertex.x > v.x) minVertex.x = v.x;
			if (minVertex.y > v.y) minVertex.y = v.y;
			if (minVertex.z > v.z) minVertex.z = v.z;
			if (maxVertex.x < v.x) maxVertex.x = v.x;
			if (maxVertex.y < v.y) maxVertex.y = v.y;
			if (maxVertex.z < v.z) maxVertex.z = v.z;
		}
		else if (line.substr(0, 1) == "f") { // face
			std::istringstream ss(line.substr(1));
			std::string ptn;
			unsigned int p, t, n, vertexCount = 0;
			char slash;
			while (ss >> ptn) {
				if (vertexCount >= 3) { // subdivision
					this->subMeshes.back().vertexIndices.push_back(this->subMeshes.back().vertexIndices[this->subMeshes.back().vertexIndices.size() - vertexCount]);
					this->subMeshes.back().vertexIndices.push_back(this->subMeshes.back().vertexIndices[this->subMeshes.back().vertexIndices.size() - 2]);
					vertexCount += 2;
				}
				if (vertexTrack.count(ptn)) { // share vertex with same ptn
					this->subMeshes.back().vertexIndices.push_back(vertexTrack[ptn]);
				}
				else { // add new vertex
					std::istringstream ptnss(ptn);
					ptnss >> p >> slash >> t >> slash >> n;
					this->vertices.emplace_back(position[p - 1], texture[t - 1], normal[n - 1]);
					this->subMeshes.back().vertexIndices.push_back(this->numVertices);
					vertexTrack[ptn] = this->numVertices;
					this->numVertices += 1;
				}
				++vertexCount;
			}
			this->numTriangles += vertexCount / 3;
		}
	}
	file.close();

	// Calculate object center and extent
	this->objCenter = glm::mix(maxVertex, minVertex, 0.5f);
	this->objExtent = maxVertex - minVertex;

	// Normalize the geometry data.
	if (normalized) {
		glm::mat3 scaleMatrix(1.0f / std::max({ this->objExtent.x, this->objExtent.y, this->objExtent.z }));
		for (auto& vertex : this->vertices) vertex.position = scaleMatrix * (vertex.position - this->objCenter);
		maxVertex -= this->objCenter;
		minVertex -= this->objCenter;
		this->objCenter = glm::vec3(0.0f);
		this->objExtent = scaleMatrix * this->objExtent;
	}

	// Create Buffers
	glGenBuffers(1, &this->vboId);
	glBindBuffer(GL_ARRAY_BUFFER, this->vboId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(VertexPTN) * this->numVertices, &vertices[0], GL_STATIC_DRAW);
	for (auto& subMesh : this->subMeshes) {
		glGenBuffers(1, &subMesh.iboId);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, subMesh.iboId);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * subMesh.vertexIndices.size(), &subMesh.vertexIndices[0], GL_STATIC_DRAW);
	}

	return true;
}

void TriangleMesh::Draw(const PhongShadingDemoShaderProg* shader)
{
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, vboId);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexPTN), 0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(VertexPTN), (const GLvoid*)(sizeof(GLfloat) * 3));
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(VertexPTN), (const GLvoid*)(sizeof(GLfloat) * 5));

	for (const auto& subMesh : subMeshes) {
		// upload material data to shader
		const glm::vec3 Ka = subMesh.material->GetKa(), Kd = subMesh.material->GetKd(), Ks = subMesh.material->GetKs();
		glUniform3fv(shader->GetLocKa(), 1, glm::value_ptr(Ka));
		glUniform3fv(shader->GetLocKd(), 1, glm::value_ptr(Kd));
		glUniform3fv(shader->GetLocKs(), 1, glm::value_ptr(Ks));
		glUniform1f(shader->GetLocNs(), subMesh.material->GetNs());
		if (subMesh.material->GetMapKd() != nullptr) {
			subMesh.material->GetMapKd()->Bind(GL_TEXTURE0);
			glUniform1i(shader->GetLocMapKd(), 0);
			glUniform1i(shader->GetLocUseMapKd(), 1);
		}
		else { glUniform1i(shader->GetLocUseMapKd(), 0); }
		// draw
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, subMesh.iboId);
		glDrawElements(GL_TRIANGLES, (int)subMesh.vertexIndices.size(), GL_UNSIGNED_INT, 0);
	}

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
}

// Show model information.
void TriangleMesh::ShowInfo()
{
	std::cout << "# Vertices: " << numVertices << std::endl;
	std::cout << "# Triangles: " << numTriangles << std::endl;
	std::cout << "Total " << subMeshes.size() << " subMeshes loaded" << std::endl;
	for (unsigned int i = 0; i < subMeshes.size(); ++i) {
		const SubMesh& g = subMeshes[i];
		std::cout << "SubMesh " << i << " with material: " << g.material->GetName() << std::endl;
		std::cout << "Num. triangles in the subMesh: " << g.vertexIndices.size() / 3 << std::endl;
	}
	std::cout << "Model Center: " << objCenter.x << ", " << objCenter.y << ", " << objCenter.z << std::endl;
	std::cout << "Model Extent: " << objExtent.x << " x " << objExtent.y << " x " << objExtent.z << std::endl;
}

