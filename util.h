#pragma once
#include "types.h"

#include <map>
#include <vector>
#include <iostream>

//inline std::set<Face> GetKRingFaces(const MyMesh& mesh, const int k, const Face& f)
//{
//	std::set<Face> neighbors, new_neighbors;
//	std::set<Face> f_mark;
//
//	neighbors.insert(f);
//	for (size_t i = 0; i < k; i++)
//	{
//		for (const auto& neighbor : neighbors)
//		{
//			if (f_mark.count(neighbor)) continue;
//			for (const auto ff_h : mesh.ff_range(neighbor))
//			{
//				new_neighbors.insert(ff_h);
//			}
//			f_mark.insert(neighbor);
//		}
//		neighbors.insert(new_neighbors.begin(), new_neighbors.end());
//		new_neighbors.clear();
//	}
//  neighbors.erase(f);
//
//	return neighbors;
//}

inline std::set<Face> GetOneRingFaces(const MyMesh& mesh, const Face& f)
{
	std::set<Face> neighbors;

	for (const auto ff_h : mesh.ff_range(f))
	{
		neighbors.insert(ff_h);
	}

	return neighbors;
}