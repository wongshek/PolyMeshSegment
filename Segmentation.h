#pragma once
#include "util.h"

class Segmentation
{
public:
	Segmentation(const double _ang_thres);
	~Segmentation();

	bool Run(MyMesh& mesh);

protected:
	bool RegionGrow(MyMesh& mesh);
	bool CheckAngle(const MyMesh& mesh, const Face& face, const MyMesh::Normal& pNormal, double theta);
	Face GetRandomFace(const std::set<Face>& faces);
	MyMesh::Color GetRandomColor();
	MyMesh::Normal GetPlaneNormal(const MyMesh& mesh, const Plane_3& plane, const Face& face);
	Plane_3 FittingPlane(const MyMesh& mesh, const std::set<Face>& faces);
	Plane_3 FittingPlane(const MyMesh& mesh, const Face& face);
	bool WriteLabel(MyMesh& mesh);
	

protected:
	double m_ang_thres;
	std::map<size_t, std::set<Face>> m_segment_map;
};
