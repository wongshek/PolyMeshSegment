#pragma once

// OPENMESH //
#include <OpenMesh/Core/IO/MeshIO.hh>
#include <OpenMesh/Core/Utils/PropertyManager.hh>
#include <OpenMesh/Core/Mesh/PolyMesh_ArrayKernelT.hh>

namespace OM = OpenMesh;
struct MyTraits : public OM::DefaultTraits
{
	typedef OM::Vec3d Point; // use double-values points
};
typedef OM::PolyMesh_ArrayKernelT<MyTraits>  MyMesh;

using Face = OM::SmartFaceHandle;
using Vertex = OM::SmartVertexHandle;
// OPENMESH //

// CGAL //
#include <CGAL/Simple_cartesian.h>
#include <CGAL/linear_least_squares_fitting_3.h>

typedef CGAL::Simple_cartesian<double>                      Kernel;
typedef Kernel::Point_3                                     Point_3;
typedef Kernel::Plane_3                                     Plane_3;
// CGAL //

// PROGRESS //
#include <string>
static const std::string clear = "\r" + std::string(80, ' ') + "\r";
// PROGRESS //