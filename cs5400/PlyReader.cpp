#include "PlyReader.hpp"

void PlyOpener::openFile(const std::string &filePath)
{
	
	file.open(filePath, std::ios::in);

	//if(!file.is_open())
	//	throw std::runtime_error("Could not open file " + filePath);
}

void PlyOpener::readData()
{

	std::getline(file, line);

	

	//Read header
	while(!file.eof())
	{
		std::getline(file, line);

		file >> chunk;

		if(chunk == "element")
		{
			file >> chunk;
			if(chunk == "vertex")
				file >> numVertices;
			else if(chunk == "face")
				file >> numTriangles;
		}

		if(chunk == "end_header")
			break;

	}

	//Store Points
	for(int i = 0; i < numVertices; i++)
	{
		file >> x;
		file >> y;
		file >> z;
		file >> confidence;
		file >> intensity;

		points.push_back(x);
		points.push_back(y);
		points.push_back(z);
	}

	//Store faces
	for(int i = 0; i < numTriangles; i++)
	{

		file >> chunk;
		file >> p1;
		file >> p2;
		file >> p3;

		faces.push_back(p1);
		faces.push_back(p2);
		faces.push_back(p3);
	}

	
}


std::vector<GLfloat> PlyOpener::getPoints()
{
	return points;
}

std::vector<GLushort> PlyOpener::getFaces()
{
	return faces;
}