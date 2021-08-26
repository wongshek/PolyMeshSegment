#include "util.h"
#include "Segmentation.h"

int main(int argc, char** argv)
{
	std::string in_path = argv[1];
	std::string out_path = argv[2];
	double ang_thres = std::stod(argv[3]);
	
	MyMesh mesh;
	OpenMesh::IO::read_mesh(mesh, in_path);
	
	Segmentation SEG(ang_thres);
	SEG.Run(mesh);

	OpenMesh::IO::Options wopt;
	wopt += OpenMesh::IO::Options::FaceColor;
	OpenMesh::IO::write_mesh(mesh, out_path, wopt);
}