#include "Segmentation.h"
#include <random>

Segmentation::Segmentation(const double _ang_thres) : m_ang_thres(_ang_thres)
{
}

Segmentation::~Segmentation()
{
}

bool Segmentation::Run(MyMesh& mesh)
{
	mesh.request_face_colors();
	mesh.request_face_normals();
	mesh.update_face_normals();

	RegionGrow(mesh);

	WriteLabel(mesh);

	return true;
}

bool Segmentation::RegionGrow(MyMesh& mesh)
{
	auto f_chart = OM::getOrMakeProperty<OM::FaceHandle, size_t>(mesh, "f_chart");

	std::set<Face> faces;
	for (const auto& f_h : mesh.faces())
	{
		faces.insert(f_h);
		f_chart[f_h] = -1;
	}

	size_t seg_number = 0;
	size_t current_index = 0;
	std::map<size_t, Plane_3> plane_map;
	std::map<size_t, MyMesh::Normal> pnormal_map;
	MyMesh::Color current_color = GetRandomColor();

	while (faces.size() != 0)
	{
		// Locate vertex with highest planarity
		Face rnd_face = GetRandomFace(faces);

		// Initialize current region
		std::set<Face> current_region;
		std::set<Face> seeds;

		// Calculate initial plane
		Plane_3 plane = FittingPlane(mesh, rnd_face);
		MyMesh::Normal pNormal = GetPlaneNormal(mesh, plane, rnd_face);

		int idx1 = rnd_face.idx();
		
		// Update
		current_region.insert(rnd_face);
		f_chart[rnd_face] = current_index;
		mesh.set_color(rnd_face, current_color);
		faces.erase(rnd_face);
		seeds.insert(rnd_face);

		// Forward grow
		while (seeds.size() != 0)
		{
			//neighbors.clear();
			// Collect 1-ring faces for all seeds
			std::set<Face> neighbors;
			std::map<Face, Face> map_face_seed;
			for (const auto& seed : seeds)
			{
				// Collect faces
				std::set<Face> new_neighbors = GetOneRingFaces(mesh, seed);
				neighbors.insert(new_neighbors.begin(), new_neighbors.end());
				for (const auto& nn : new_neighbors)
				{
					map_face_seed[nn] = seed;
				}
			}
			seeds.clear();

			// Check neighboring faces
			for (auto& neighbor : neighbors)
			{
				int idx2 = neighbor.idx();
				// Check visited
				bool is_visited = f_chart[neighbor] != -1;
				if (is_visited)	continue;

				// Check distance and angle
				//bool is_angle_fitting = CheckAngle(mesh, neighbor, pNormal, m_ang_thres);
				bool is_angle_fitting = CheckAngle(mesh, neighbor, map_face_seed[neighbor], m_ang_thres);
				if (is_angle_fitting)
				{
					// Add face to current region
					current_region.insert(neighbor);

					// Update
					f_chart[neighbor] = current_index;
					mesh.set_color(neighbor, current_color);
					faces.erase(neighbor);
					seeds.insert(neighbor);
				}
			}

			// Calculate plane for current region
			if (!current_region.empty())
			{
				plane = FittingPlane(mesh, current_region);
				pNormal = GetPlaneNormal(mesh, plane, *current_region.begin());
			}
		}

		// Debug info print
		{
			int cSize = current_region.size();
			std::stringstream ss;
			ss << clear << "current_region size:" << " " << cSize;
			std::cout << ss.rdbuf() << std::flush;
		}

		// Store
		if (current_region.size() != 0)
		{
			plane_map[current_index] = plane;
			pnormal_map[current_index] = pNormal;
			m_segment_map[current_index] = current_region;
		}

		// Initialize search for next region
		seg_number += 1;
		current_index += 1;
		current_region.clear();
		current_color = GetRandomColor();
	}
	return true;
}

bool Segmentation::CheckAngle(const MyMesh& mesh, const Face& face, const MyMesh::Normal& pNormal, double theta)
{
	MyMesh::Normal fNormal = mesh.normal(face); // Already Normalize

	double angle = std::acos((fNormal.dot(pNormal))) * 180 / CGAL_PI;

	return angle < theta;
}

bool Segmentation::CheckAngle(const MyMesh& mesh, const Face& face1, const Face& face2, double theta)
{
	MyMesh::Normal fNormal1 = mesh.normal(face1); // Already Normalize
	MyMesh::Normal fNormal2 = mesh.normal(face2); // Already Normalize

	double angle = std::acos((fNormal1.dot(fNormal2))) * 180 / CGAL_PI;

	return angle < theta;
}

Face Segmentation::GetRandomFace(const std::set<Face>& faces)
{
	/*std::random_device rd;
	std::mt19937 mt(rd());
	std::uniform_int_distribution<> dist(0, faces.size());

	auto iter = faces.begin();
	std::advance(iter, dist(mt));

	return *iter;*/

	return *faces.begin();
}

MyMesh::Color Segmentation::GetRandomColor()
{
	std::random_device rd;
	std::mt19937 mt(rd());
	std::uniform_int_distribution<> dist(100, 256);
	
	return MyMesh::Color(dist(mt), dist(mt), dist(mt));
}

MyMesh::Normal Segmentation::GetPlaneNormal(const MyMesh& mesh, const Plane_3& plane, const Face& face)
{
	auto ortho_vector = plane.orthogonal_vector();
	MyMesh::Normal init_pnormal(ortho_vector[0], ortho_vector[1], ortho_vector[2]);
	init_pnormal.normalize();

	MyMesh::Normal face_normal = mesh.normal(face);	// Already Normalize
	double angle = std::acos((init_pnormal.dot(face_normal))) * 180 / CGAL_PI;

	if (angle < 90)
	{
		return init_pnormal;
	}
	else
	{
		return -init_pnormal;	
	}
}

Plane_3 Segmentation::FittingPlane(const MyMesh& mesh, const std::set<Face>& faces)
{
	std::set<Vertex> vertices;
	for (const auto& f_h : faces)
	{
		for (const auto& v_h : mesh.fv_range(f_h))
		{
			vertices.insert(v_h);
		}
	}

	std::vector<Point_3> points;
	points.reserve(vertices.size());
	for (const auto& v_h : vertices)
	{
		const auto& pt = mesh.point(v_h);
		points.emplace_back(Point_3(pt[0], pt[1], pt[2]));
	}

	Plane_3 plane;
	linear_least_squares_fitting_3(points.begin(), points.end(), plane, CGAL::Dimension_tag<0>());

	return plane;
}

Plane_3 Segmentation::FittingPlane(const MyMesh& mesh, const Face& face)
{
	std::set<Vertex> vertices;
	for (const auto& v_h : mesh.fv_range(face))
	{
		vertices.insert(v_h);
	}
	
	std::vector<Point_3> points;
	points.reserve(vertices.size());
	for (const auto& v_h : vertices)
	{
		const auto& pt = mesh.point(v_h);
		points.emplace_back(Point_3(pt[0], pt[1], pt[2]));
	}

	Plane_3 plane;
	linear_least_squares_fitting_3(points.begin(), points.end(), plane, CGAL::Dimension_tag<0>());

	return plane;
}


bool Segmentation::WriteLabel(MyMesh& mesh)
{
	// Write label file
	auto f_chart = OM::getProperty<OM::FaceHandle, size_t>(mesh, "f_chart");

	std::vector<size_t> labels(mesh.n_faces() + 1);
	labels[0] = size_t(m_segment_map.size());
	for (const auto& f_h : mesh.faces())
	{
		labels[f_h.idx() + 1] = f_chart[f_h];
	}

	std::ofstream f("label.dat", std::ios::binary);
	f.write((char*)labels.data(), (mesh.n_faces() + 1) * sizeof(size_t));
	f.close();

	return true;
}