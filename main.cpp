#include "includes/args.h"
#include "includes/CompFab.h"
#include "includes/Mesh.h"
#include "includes/utils.h"

#include <tclap/CmdLine.h>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
// #include <cstdlib>
#include <cstdlib>

enum FileFormat { obj, binvox };

struct VoxelizerArgs : Args {
	// path to files
	std::string input, output;
	FileFormat format;
	// voxelization settings
	int size;
	// width, height, depth;
	int samples;
};

// construct the command line arguments
VoxelizerArgs * parseArgs(int argc, char *argv[]) {
	VoxelizerArgs * args = new VoxelizerArgs();

	// Define the command line object
	TCLAP::CmdLine cmd("A simple voxelization utility.", ' ', "0.0");

	TCLAP::UnlabeledValueArg<std::string> input( "input", "path to .obj mesh", true, "", "string");
	TCLAP::UnlabeledValueArg<std::string> output("output","path to save voxel grid", true, "", "string");

	TCLAP::ValueArg<std::string> format("f", "format","voxel grid save format - obj|binvox", false, "binvox", "string");
	TCLAP::ValueArg<int> size(  "r","resolution", "voxelization resolution",  false, 32, "int");
	TCLAP::ValueArg<int> samples( "s","samples", "number of sample rays per vertex",  false, -1, "int");

	TCLAP::MultiSwitchArg verbosity( "v", "verbose", "Verbosity level. Multiple flags for more verbosity.");


	// Add args to command line object and parse
	cmd.add(input); cmd.add(output);  // order matters for positional args
	cmd.add(size); cmd.add(format); 
	// cmd.add(width); cmd.add(height); cmd.add(depth); 
	cmd.add(verbosity); cmd.add(samples);
	cmd.parse( argc, argv );

	// store in wrapper struct
	args->input  = input.getValue();
	args->output = output.getValue();
	args->size   = size.getValue();
	// args->width  = width.getValue();
	// args->height = height.getValue();
	// args->depth  = depth.getValue();
	args->samples  = samples.getValue();
	args->verbosity  = verbosity.getValue();

	args->debug(1) << "input:     " << args->input  << std::endl;
	args->debug(1) << "output:    " << args->output << std::endl;

	char fl = format.getValue().at(0);
	if (fl == 'b' || fl == 'B') {
		args->format = binvox;
		args->debug(1) << "save format: binvox" << std::endl;
	} else if (fl == 'o' || fl == 'O') {
		args->format = obj;
		args->debug(1) << "save format: obj" << std::endl;
	} else {
		args->debug(0) << "Unknown file format specified, use one of: (o) obj, (b) binvox" << std::endl;
	}


	// args->debug(1) << "format:    " << args->format   << std::endl;
	args->debug(1) << "size:      " << args->size   << std::endl;
	// args->debug(1) << "width:     " << args->width  << std::endl;
	// args->debug(1) << "height:    " << args->height << std::endl;
	// args->debug(1) << "depth:     " << args->height << std::endl;
	args->debug(1) << "samples:   " << args->samples << std::endl;
	args->debug(1) << "verbosity: " << args->verbosity << std::endl;

	return args;
}

typedef std::vector<CompFab::Triangle> TriangleList;

TriangleList g_triangleList;
CompFab::VoxelGrid *g_voxelGrid;

bool loadMesh(const char *filename, unsigned int dim)
{
	g_triangleList.clear();
	
	Mesh *tempMesh = new Mesh(filename, true);
	
	CompFab::Vec3 v1, v2, v3;

	//copy triangles to global list
	for(unsigned int tri =0; tri<tempMesh->t.size(); ++tri)
	{
		v1 = tempMesh->v[tempMesh->t[tri][0]];
		v2 = tempMesh->v[tempMesh->t[tri][1]];
		v3 = tempMesh->v[tempMesh->t[tri][2]];
		g_triangleList.push_back(CompFab::Triangle(v1,v2,v3));
	}

	//Create Voxel Grid
	CompFab::Vec3 bbMax, bbMin;
	BBox(*tempMesh, bbMin, bbMax);
	
	//Build Voxel Grid
	double bbX = bbMax[0] - bbMin[0];
	double bbY = bbMax[1] - bbMin[1];
	double bbZ = bbMax[2] - bbMin[2];
	double spacing;
	
	if(bbX > bbY && bbX > bbZ)
	{
		spacing = bbX/(double)(dim-2);
	} else if(bbY > bbX && bbY > bbZ) {
		spacing = bbY/(double)(dim-2);
	} else {
		spacing = bbZ/(double)(dim-2);
	}
	
	CompFab::Vec3 hspacing(0.5*spacing, 0.5*spacing, 0.5*spacing);

	g_voxelGrid = new CompFab::VoxelGrid(bbMin-hspacing, dim, dim, dim, spacing);

	delete tempMesh;
	return true;
}



void saveVoxelsToObj(const char * outfile)
{
	Mesh box;
	Mesh mout;
	int nx = g_voxelGrid->m_dimX;
	int ny = g_voxelGrid->m_dimY;
	int nz = g_voxelGrid->m_dimZ;
	double spacing = g_voxelGrid->m_spacing;
	
	CompFab::Vec3 hspacing(0.5*spacing, 0.5*spacing, 0.5*spacing);
	
	for (int ii = 0; ii < nx; ii++) {
		for (int jj = 0; jj < ny; jj++) {
			for (int kk = 0; kk < nz; kk++) {
				if(!g_voxelGrid->isInside(ii,jj,kk)){
					continue;
				}
				CompFab::Vec3 coord(0.5f + ((double)ii)*spacing, 0.5f + ((double)jj)*spacing, 0.5f+((double)kk)*spacing);
				CompFab::Vec3 box0 = coord - hspacing;
				CompFab::Vec3 box1 = coord + hspacing;
				makeCube(box, box0, box1);
				mout.append(box);
			}
		}
	}

	mout.save_obj(outfile);
}

bool save(VoxelizerArgs *args) {
	switch (args->format) {
		case obj:
			saveVoxelsToObj((args->output + ".obj").c_str());
			break;
		case binvox:
			g_voxelGrid->save_binvox((args->output + ".binvox").c_str());
			break;
		default:
			args->debug(0) << "Failed to save - no file type specified." << std::endl;
			return false;
	}
	return true;
}

extern void kernel_wrapper(int samples, int w, int h, int d, CompFab::VoxelGrid *g_voxelGrid, std::vector<CompFab::Triangle> triangles);

int main(int argc, char *argv[])
{
	VoxelizerArgs *args = parseArgs(argc, argv);

	args->debug(0) << "\nLoading Mesh" << std::endl;
	loadMesh(args->input.c_str(), args->size);

	clock_t start = clock();
	args->debug(0) << "Voxelizing in the GPU, this might take a while." << std::endl;
	if (args->samples > -1) args->debug(0) << "Randomly choosing " << args->samples << " directions." << std::endl;
	kernel_wrapper(args->samples, args->size, args->size, args->size, g_voxelGrid, g_triangleList);

	// Summary: teapot.obj (9000 triangles) @ 512x512x512, 3 samples in: 15 seconds
	args->debug(0) << "Summary: "
		<< utils::split(args->input, '/').back() 
		<< " (" << g_triangleList.size() << " triangles)"
		<< " @ " << g_voxelGrid->m_dimX << "x" << g_voxelGrid->m_dimY << "x" << g_voxelGrid->m_dimZ;
	if (args->samples > 0) 
		args->debug(0) << ", " << args->samples << " samples" ;
	else args->debug(0) << ", 1 sample" ;
	args->debug(0) << " in: " << float( clock() - start ) /  CLOCKS_PER_SEC << " seconds" << std::endl;

	args->debug(0) << "Saving Results." << std::endl;
	if (!save(args)) {
		args->debug(0) << "Failed to save! Exiting." << std::endl;
	};

	return 0;
}