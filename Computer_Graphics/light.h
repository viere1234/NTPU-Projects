#ifndef LIGHT_H
#define LIGHT_H

#include "headers.h"


// VertexP Declarations.
struct VertexP
{
	VertexP() { position = glm::vec3(0.0f, 0.0f, 0.0f); }
	VertexP(glm::vec3 p) { position = p; }
	glm::vec3 position;
};

// PointLight Declarations.
class PointLight
{
public:
	// PointLight Public Methods.
	PointLight() {
		position = glm::vec3(1.5f, 1.5f, 1.5f);
		intensity = glm::vec3(1.0f, 1.0f, 1.0f);
		CreateVisGeometry();
	}
	PointLight(const glm::vec3 p, const glm::vec3 I) {
		position = p;
		intensity = I;
		CreateVisGeometry();
	}
	~PointLight() {
		glDeleteBuffers(1, &vboId);
	}

	glm::vec3 GetPosition()  const { return position;  }
	glm::vec3 GetIntensity() const { return intensity; }
	
	void Draw() {
		glPointSize(16.0f);
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vboId);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexP), 0);
		glDrawArrays(GL_POINTS, 0, 1);
		glDisableVertexAttribArray(0);
		glPointSize(1.0f);
	}

	void TranslationXY(const float* translate) {
		position.x += translate[0] * 0.0001f;
		position.y += translate[1] * 0.0001f;
	}

	void MoveLeft (const float moveSpeed) { position += moveSpeed * glm::vec3(-0.1f,  0.0f, 0.0f); }
	void MoveRight(const float moveSpeed) { position += moveSpeed * glm::vec3( 0.1f,  0.0f, 0.0f); }
	void MoveUp   (const float moveSpeed) { position += moveSpeed * glm::vec3( 0.0f,  0.1f, 0.0f); }
	void MoveDown (const float moveSpeed) { position += moveSpeed * glm::vec3( 0.0f, -0.1f, 0.0f); }

protected:
	// PointLight Protected Methods.
	void CreateVisGeometry() {
		VertexP lightVtx = glm::vec3(0, 0, 0);
		const int numVertex = 1;
		glGenBuffers(1, &vboId);
		glBindBuffer(GL_ARRAY_BUFFER, vboId);
		glBufferData(GL_ARRAY_BUFFER, sizeof(VertexP) * numVertex, &lightVtx, GL_STATIC_DRAW);
	}

	// PointLight Private Data.
	GLuint vboId;
	glm::vec3 position;
	glm::vec3 intensity;
};



// SpotLight Declarations.
class SpotLight : public PointLight
{
public:
	// SpotLight Public Methods.
	SpotLight() {
		position = glm::vec3(0.0f, 2.0f, 0.0f);
		intensity = glm::vec3(1.0f, 1.0f, 1.0f);
		direction = glm::vec3(1.5f, 1.5f, 1.5f);
		totalWidthCos = glm::cos(glm::radians(45.0f));
		epsilon = glm::cos(glm::radians(30.0f)) - totalWidthCos;
		CreateVisGeometry();
	}
	SpotLight(const glm::vec3 p, const glm::vec3 I, const glm::vec3 D, const float cutoffDeg, const float totalWidthDeg) {
		position = p;
		intensity = I;
		direction = glm::normalize(D);
		totalWidthCos = glm::cos(glm::radians(totalWidthDeg));
		epsilon = glm::cos(glm::radians(cutoffDeg)) - totalWidthCos;
		CreateVisGeometry();
	}

	glm::vec3 GetDirection() const { return direction; }
	float GetTotalWidthCos() const { return totalWidthCos; }
	float GetEpsilon() const { return epsilon; }

private:
	// SpotLight Private Data.
	glm::vec3 direction;
	float totalWidthCos;
	float epsilon;
};



// DirectionalLight Declarations.
class DirectionalLight
{
public:
	// DirectionalLight Public Methods.
	DirectionalLight() {
		direction = glm::vec3(1.5f, 1.5f, 1.5f);
		radiance = glm::vec3(1.0f, 1.0f, 1.0f);
		CreateVisGeometry();
	};
	DirectionalLight(const glm::vec3 dir, const glm::vec3 L) {
		direction = glm::normalize(dir);
		radiance = L;
		CreateVisGeometry();
	}
	~DirectionalLight() {
		glDeleteBuffers(1, &vboId);
		glDeleteBuffers(1, &iboId);
		glDeleteBuffers(1, &dirVboId);
		glDeleteBuffers(1, &dirIboId);
	}

	void Draw(const FillColorShaderProg* shader, const glm::mat4x4 MVP) {
		glLineWidth(2.0f);
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vboId);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexP), 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboId);
		// x
		glUniformMatrix4fv(shader->GetLocMVP(), 1, GL_FALSE, glm::value_ptr(MVP));
		glUniform3fv(shader->GetLocFillColor(), 1, glm::value_ptr(xVisColor));
		glDrawElements(GL_LINES, 6, GL_UNSIGNED_INT, 0);
		// y
		glm::mat4x4 yMVP = MVP * glm::rotate(glm::mat4x4(1.0f), glm::radians(90.0f), glm::vec3(0, 0, 1));
		glUniformMatrix4fv(shader->GetLocMVP(), 1, GL_FALSE, glm::value_ptr(yMVP));
		glUniform3fv(shader->GetLocFillColor(), 1, glm::value_ptr(yVisColor));
		glDrawElements(GL_LINES, 6, GL_UNSIGNED_INT, 0);
		// z
		glm::mat4x4 zMVP = MVP * glm::rotate(glm::mat4x4(1.0f), glm::radians(90.0f), glm::vec3(0, -1, 0)) * glm::rotate(glm::mat4x4(1.0f), glm::radians(90.0f), glm::vec3(1, 0, 0));
		glUniformMatrix4fv(shader->GetLocMVP(), 1, GL_FALSE, glm::value_ptr(zMVP));
		glUniform3fv(shader->GetLocFillColor(), 1, glm::value_ptr(zVisColor));
		glDrawElements(GL_LINES, 6, GL_UNSIGNED_INT, 0);
		// direction
		glBindBuffer(GL_ARRAY_BUFFER, dirVboId);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexP), 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, dirIboId);
		glm::mat4x4 dirMVP = MVP * glm::rotate(glm::mat4x4(1.0f), glm::acos(glm::dot(-direction, glm::vec3(1.0f, 0.0f, 0.0f))), glm::cross(direction, glm::vec3(1.0f, 0.0f, 0.0f)));
		glUniformMatrix4fv(shader->GetLocMVP(), 1, GL_FALSE, glm::value_ptr(dirMVP));
		glUniform3fv(shader->GetLocFillColor(), 1, glm::value_ptr(dirVisColor));
		glDrawElements(GL_LINES, 10, GL_UNSIGNED_INT, 0);
		glDisableVertexAttribArray(0);
	}

	void ChangeDirection(const float* rotationMatrixPtr)
	{
		direction = (glm::make_mat4(rotationMatrixPtr) * glm::vec4(0.0f, 0.0f, -1.0f, 0.0f)).xyz;
	}

	glm::vec3 GetDirection() const { return direction; }
	glm::vec3 GetRadiance()  const { return radiance; }

private:
	void CreateVisGeometry() {
		// Coordinate Visualized
		VertexP arrowVertices[4] = { glm::vec3(0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.9f, 0.1f, 0.0f), glm::vec3(0.9f, -0.1f, 0.0f) };
		unsigned int arrowIndices[6] = { 0, 1, 1, 2, 1, 3 };
		int numVertices = 4;
		glGenBuffers(1, &vboId);
		glBindBuffer(GL_ARRAY_BUFFER, vboId);
		glBufferData(GL_ARRAY_BUFFER, sizeof(VertexP) * numVertices, &arrowVertices, GL_STATIC_DRAW);
		glGenBuffers(1, &iboId);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboId);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * 6, &arrowIndices, GL_STATIC_DRAW);
		// Direction Visualized
		VertexP dirArrowVertices[6] = { glm::vec3(0.3f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.4f, 0.1f, 0.0f), glm::vec3(0.4f, -0.1f, 0.0f), glm::vec3(0.4f, 0.0f, 0.1f), glm::vec3(0.4f, 0.0f, -0.1f) };
		unsigned int dirArrowIndices[10] = { 0, 1, 0, 2, 0, 3, 0, 4, 0, 5 };
		numVertices = 6;
		glGenBuffers(1, &dirVboId);
		glBindBuffer(GL_ARRAY_BUFFER, dirVboId);
		glBufferData(GL_ARRAY_BUFFER, sizeof(VertexP) * numVertices, &dirArrowVertices, GL_STATIC_DRAW);
		glGenBuffers(1, &dirIboId);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, dirIboId);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * 10, &dirArrowIndices, GL_STATIC_DRAW);
	}

	// DirectionalLight Private Data.
	GLuint vboId, iboId, dirVboId, dirIboId;
	const glm::vec3 xVisColor = glm::vec3(1.0f, 0.0f, 0.0f), yVisColor = glm::vec3(0.0f, 1.0f, 0.0f), zVisColor = glm::vec3(0.0f, 0.0f, 1.0f), dirVisColor = glm::vec3(0.0f);
	glm::vec3 direction;
	glm::vec3 radiance;
};

#endif
