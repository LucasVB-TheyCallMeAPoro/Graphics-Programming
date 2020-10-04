#pragma once
#include "EMath.h"

struct MeshTriangleInfo
{
	unsigned int idxVertex0;
	unsigned int idxVertex1;
	unsigned int idxVertex2;

	Elite::FVector3 normal;
};

struct Vertex
{
	Elite::FPoint3 position;
};
