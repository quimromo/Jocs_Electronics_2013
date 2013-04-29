#include "mesh.h"
#include <cassert>
#include "includes.h"
#include "text.h"
#include <fstream>
#include <string.h>
#include <stdio.h>

Mesh::Mesh()
{
	primitive = GL_TRIANGLES;
}

void Mesh::clear()
{
	vertices.clear();
	normals.clear();
	uvs.clear();
}

void Mesh::render()
{
	assert(vertices.size() && "No vertices in this mesh");

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, &vertices[0] );

	if (normals.size())
	{
		glEnableClientState(GL_NORMAL_ARRAY);
		glNormalPointer(GL_FLOAT, 0, &normals[0] );
	}

	if (uvs.size())
	{
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glTexCoordPointer(2,GL_FLOAT, 0, &uvs[0] );
	}

	glDrawArrays(primitive, 0, vertices.size() );
	glDisableClientState(GL_VERTEX_ARRAY);

	if (normals.size())
		glDisableClientState(GL_NORMAL_ARRAY);
	if (uvs.size())
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);

}

void Mesh::createPlane(float size)
{
	vertices.clear();
	normals.clear();
	uvs.clear();

	//create six vertices (3 for upperleft triangle and 3 for lowerright)

	vertices.push_back( Vector3(size,0,size) );
	vertices.push_back( Vector3(size,0,-size) );
	vertices.push_back( Vector3(-size,0,-size) );
	vertices.push_back( Vector3(-size,0,size) );
	vertices.push_back( Vector3(size,0,size) );
	vertices.push_back( Vector3(-size,0,-size) );

	//all of them have the same normal
	normals.push_back( Vector3(0,1,0) );
	normals.push_back( Vector3(0,1,0) );
	normals.push_back( Vector3(0,1,0) );
	normals.push_back( Vector3(0,1,0) );
	normals.push_back( Vector3(0,1,0) );
	normals.push_back( Vector3(0,1,0) );

	//texture coordinates
	uvs.push_back( Vector2(1,1) );
	uvs.push_back( Vector2(1,0) );
	uvs.push_back( Vector2(0,0) );
	uvs.push_back( Vector2(0,1) );
	uvs.push_back( Vector2(1,1) );
	uvs.push_back( Vector2(0,0) );
}

bool Mesh::loadASE(const char* fileName){

	char binfilename[100];
	strcpy(binfilename, fileName);
	strcat(binfilename, ".bin");
	
	if(loadASEbin(binfilename)){
		return true;
	}

	text file;
	if(!file.create(fileName)){
		std::cout << fileName << "not found." << std::endl;
		return false;
	}
	file.seek("*MESH_NUMVERTEX");
	int numVert = file.getint(); file.getword();
	
	int numFaces = file.getint();
	
	std::vector< Vector3 > vert_temp(numVert);
	
	for(int i = 0; i < numVert; ++i){
		float x,y,z;
		file.seek("*MESH_VERTEX"); file.getint();	
		x = file.getfloat();
		y = file.getfloat();
		z = file.getfloat();
		vert_temp[i] = Vector3(x,y,z);
	}

	vertices.resize(numFaces * 3);
	for(int f = 0; f < numFaces; ++f){
		file.seek("*MESH_FACE"); file.getword();
		for(int vf = 0; vf < 3; ++vf){
			file.getword(); int v = file.getint();
			vertices[f*3 + vf] = vert_temp[v];
		}
	}
	vert_temp.clear();

	file.seek("*MESH_NUMTVERTEX");
	int tex_vs = file.getint();
	std::vector< Vector2 > uv_temp(tex_vs);
	for(int i = 0; i < tex_vs; ++i){
		file.seek("*MESH_TVERT");file.getint();
		float u = file.getfloat();
		float v = file.getfloat();
		uv_temp[i] = Vector2(u, v);
	}

	uvs.resize(numFaces * 3);
	for(int f = 0; f < numFaces; ++f){
		file.seek("*MESH_TFACE"); file.getint();
		for(int vt = 0; vt < 3; ++vt){
			uvs[f*3 + vt] = uv_temp[file.getint()];
		}
	}

	normals.resize(numFaces * 3);
	for(int n = 0; n < numFaces * 3; ++n){
		file.seek("*MESH_VERTEXNORMAL"); file.getint();
		float x,y,z;
		x = file.getfloat();
		y = file.getfloat();
		z = file.getfloat();
		normals[n] = Vector3(x,y,z);
	}

	writeBinary(binfilename);
	return true;
	
}

bool Mesh::writeBinary(const char* filename){

	std::ofstream file;
	file.open(filename, std::ios::out | std::ios::binary);
	if(!file.is_open()){
		std::cout << "No se puede abrir el fichero " << filename << std::endl;
		return false;
	}
	int nvertices = vertices.size();
	file.write((const char*)&nvertices, sizeof(int));
	file.write((char*)&vertices[0], sizeof(Vector3)*nvertices);

	int nnormals = normals.size();
	file.write((char*)&nnormals, sizeof(int));
	file.write((char*)&normals[0], sizeof(Vector3)*nnormals);

	int nuvs = uvs.size();
	file.write((char*)&nuvs, sizeof(int));
	file.write((char*)&uvs[0], sizeof(Vector2)*nuvs);

	file.close();
	return true;

}

bool Mesh::loadASEbin(const char* filename){
	
	std::ifstream file;
	file.open(filename, std::ios::in | std::ios::binary);
	if(!file.is_open()){
		std::cout << "No se puede abrir el fichero " << filename << std::endl;
		return false;
	}
	int nvertices;
	file.read((char*)&nvertices, sizeof(int));
	vertices.resize(nvertices);
	file.read((char*)&vertices[0], sizeof(Vector3)*nvertices);

	int nnormals;
	file.read((char*)&nnormals, sizeof(int));
	normals.resize(nnormals);
	file.read((char*)&normals[0], sizeof(Vector3)*nnormals);

	int nuvs;
	file.read((char*)&nuvs, sizeof(int));
	uvs.resize(nuvs);
	file.read((char*)&uvs[0], sizeof(Vector2)*nuvs);

	file.close();
	return true;

}
