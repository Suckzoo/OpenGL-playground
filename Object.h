#ifndef OBJECT
#define OBJECT
#include <vector>
#include <map>

#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/transform.hpp>

#include <common/objloader.hpp>
#include <common/vboindexer.hpp>

#include <btBulletDynamicsCommon.h>
class Object {
private:
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals;
	std::vector<unsigned short> indices;
	std::vector<glm::vec3> indexed_vertices;
	std::vector<glm::vec2> indexed_uvs;
	std::vector<glm::vec3> indexed_normals;
	GLuint programID;
	GLuint vertexbuffer, uvbuffer, normalbuffer, elementbuffer;
	glm::vec3 position;
	glm::quat rotation;
	btCollisionShape* collisionShape;
	btMotionState* motionState;
	btRigidBody* rigidBody;
public:
	static std::map<int, Object*> meshMap;
	Object(const char* filename,int mesh_number=0) {
		bool res = loadOBJ(filename, vertices, uvs, normals);
		indexVBO(vertices, uvs, normals, indices, indexed_vertices, indexed_uvs, indexed_normals);
		glGenBuffers(1, &vertexbuffer);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glBufferData(GL_ARRAY_BUFFER, indexed_vertices.size() * sizeof(glm::vec3), &indexed_vertices[0], GL_STATIC_DRAW);
		glGenBuffers(1, &uvbuffer);
		glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
		glBufferData(GL_ARRAY_BUFFER, indexed_uvs.size() * sizeof(glm::vec2), &indexed_uvs[0], GL_STATIC_DRAW);
		glGenBuffers(1, &normalbuffer);
		glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
		glBufferData(GL_ARRAY_BUFFER, indexed_normals.size() * sizeof(glm::vec3), &indexed_normals[0], GL_STATIC_DRAW);
		glGenBuffers(1, &elementbuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned short), &indices[0], GL_STATIC_DRAW);
		position = glm::vec3(rand()%20-10,rand()%20-10,rand()%20-10);
		rotation = glm::normalize(glm::quat(glm::vec3(rand()%360,rand()%360,rand()%360)));
		collisionShape = new btBoxShape(btVector3(1.f,1.f,1.f));
		motionState = new btDefaultMotionState(btTransform(btQuaternion(rotation.x,rotation.y,rotation.z,rotation.w),
														   btVector3(position.x,position.y,position.z)));
		btRigidBody::btRigidBodyConstructionInfo rigidBodyCI(0, motionState, collisionShape, btVector3(0,0,0));
		rigidBody = new btRigidBody(rigidBodyCI);
		rigidBody->setUserPointer((void*)mesh_number);
		meshMap[mesh_number]=this;
	}
	btRigidBody* getRigidBody() {
		return rigidBody;
	}
	void setProgram(GLuint _programID) {
		programID = _programID;
	}
	void setPosition(glm::vec3 _position) {
		position = _position;
		rigidBody->getMotionState()->setWorldTransform(btTransform(btQuaternion(rotation.x,rotation.y,rotation.z,rotation.w),btVector3(position.x,position.y,position.z)));
	}
	glm::vec3 getPosition() {
		return position;
	}
	void setRotation(glm::quat _rotation) {
		rotation = _rotation;
		rigidBody->getMotionState()->setWorldTransform(btTransform(btQuaternion(rotation.x,rotation.y,rotation.z,rotation.w),btVector3(position.x,position.y,position.z)));
	}
	glm::quat getRotation() {
		return rotation;
	}
	void draw() {
		//glUseProgram(programID);
		GLuint ModelMatrix = glGetUniformLocation(programID, "M");
		glm::mat4 RotationMatrix = glm::toMat4(rotation);
		glm::mat4 TranslationMatrix = glm::translate(position);
		glm::mat4 M = TranslationMatrix * RotationMatrix;
		glUniformMatrix4fv(ModelMatrix, 1, GL_FALSE, &M[0][0]);

		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glVertexAttribPointer(
			0,
			3,
			GL_FLOAT,
			GL_FALSE,
			0,
			(void*)0
			);
		glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
		glVertexAttribPointer(
			1,
			2,
			GL_FLOAT,
			GL_FALSE,
			0,
			(void*)0
			);
		glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
		glVertexAttribPointer(
			2,
			3,
			GL_FLOAT,
			GL_FALSE,
			0,
			(void*)0
			);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
		glDrawElements(
			GL_TRIANGLES,
			indices.size(),
			GL_UNSIGNED_SHORT,
			(void*)0
			);
	}
};
std::map<int, Object*> Object::meshMap;
#endif