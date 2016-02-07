#ifndef MESH_H
#define MESH_H

#include "includes/CompFab.h"

#include <map>
#include <vector>
#include <fstream>

class Mesh{
public:
  std::vector<CompFab::Vec3>v;
  std::vector<CompFab::Vec3>n;
  std::vector<CompFab::Vec2f>tex;
  std::vector<CompFab::Vec3i>texId;
  ///@brief triangles
  std::vector<CompFab::Vec3i>t;

  Mesh();
  Mesh(const std::vector<CompFab::Vec3>&_v,
      const std::vector<CompFab::Vec3i>&_t);
  Mesh(const CompFab::Vec3 * _v, const CompFab::Vec3i * _t);
  
  Mesh(const char * filename,bool normalize);
  void load_mesh(const char * filename, bool normalize=true);
  void save(const char * filename);
  void save(std::ostream &out, std::vector<CompFab::Vec3> *vert=0);
  void load(std::istream &in);
  void read_ply(std::istream & f);
  void read_obj(std::istream &f);

  void save_obj(const char * filename);
  void load_tex(const char * filename);
  
  void compute_norm();
  void rescale();
  void append(const Mesh & m);
  Mesh & operator= (const Mesh& m);
  virtual void update();
};
void makeCube(Mesh & m, const CompFab::Vec3 & mn,
    const CompFab::Vec3 mx);
///@brief cube [0,1]^3
extern Mesh UNIT_CUBE;
void BBox(const Mesh & m, CompFab::Vec3 & mn,
    CompFab::Vec3 & mx);

void BBox(const std::vector<CompFab::Vec3> & v, CompFab::Vec3 & mn,
    CompFab::Vec3 & mx);

bool ptInBox(const CompFab::Vec3 & mn,
    const CompFab::Vec3 mx, const CompFab::Vec3 & x);
void adjlist(const Mesh & m, std::vector<std::vector<int> > & adjMat);

#endif
