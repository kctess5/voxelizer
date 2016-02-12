#include "includes/Mesh.h"
#include "includes/CompFab.h"
#include <fstream>
#include <iostream>
#include <algorithm>
#ifdef _WIN32
#define NOMINMAX //Stop errors with std::max
#include <windows.h>
#endif
#include <stdio.h>
#include <cstdlib>
#include <utility>
#include <map>
#include <sstream>
#include <string.h>
//#include "util.h"

// typedef double CompFab::precision_type;
// typedef float CompFab::precision_type;

///@brief is a point inside a box
bool ptInBox(const CompFab::Vec3 & mn,
    const CompFab::Vec3 mx, const CompFab::Vec3 & x)
{
  for(int dim = 0 ;dim<3;dim++){
    if(x[dim]<mn[dim] || x[dim] > mx[dim]){
        return false;
    }
  }
  return true;
}

void makeCube(Mesh & m, const CompFab::Vec3 & mn,
    const CompFab::Vec3 mx)
{
  CompFab::Vec3 ss = mx -mn;
  m=UNIT_CUBE;
  for(unsigned int ii = 0;ii<m.v.size();ii++){
    m.v[ii][0] = mn[0] + ss[0]*m.v[ii][0];
    m.v[ii][1] = mn[1] + ss[1]*m.v[ii][1];
    m.v[ii][2] = mn[2] + ss[2]*m.v[ii][2];

  }
}

void Mesh::append(const Mesh & m)
{
  unsigned int offset = v.size();
  unsigned int ot = t.size();
  v.insert(v.end(),m.v.begin(),m.v.end());
  t.insert(t.end(),m.t.begin(), m.t.end());
  for(unsigned int ii = ot;ii<t.size();ii++){
    for(int jj = 0 ;jj<3;jj++){
      t[ii][jj] += offset;
    }
  }
}

Mesh & Mesh::operator= (const Mesh& m)
{
  v = m.v;
  t = m.t;
  n = m.n;
  return *this;
}

///@brief cube [0,1]^3
CompFab::Vec3 CUBE_VERT[8]={
    CompFab::Vec3 (0, 0, 0),
    CompFab::Vec3 (1, 0, 0),
    CompFab::Vec3 (1, 1, 0),
    CompFab::Vec3 (0, 1, 0),
    CompFab::Vec3 (0, 0, 1),
    CompFab::Vec3 (1, 0, 1),
    CompFab::Vec3 (1, 1, 1),
    CompFab::Vec3 (0, 1, 1)
};

CompFab::Vec3i CUBE_TRIG[12]={CompFab::Vec3i(0,3,1),
CompFab::Vec3i(1, 3, 2),
CompFab::Vec3i(5, 4, 0),
CompFab::Vec3i(5, 0, 1),
CompFab::Vec3i(6, 5, 1),
CompFab:: Vec3i(1, 2, 6),
CompFab:: Vec3i(3, 6, 2),
CompFab:: Vec3i(3, 7, 6),
CompFab:: Vec3i(4, 3, 0),
CompFab:: Vec3i(4, 7, 3),
CompFab:: Vec3i(7, 4, 5),
CompFab:: Vec3i(7, 5, 6)};
Mesh UNIT_CUBE(CUBE_VERT,CUBE_TRIG);

Mesh::Mesh():v(0),t(0){}

Mesh::Mesh(const std::vector<CompFab::Vec3>&_v,
    const std::vector<CompFab::Vec3i>&_t):v(_v),t(_t)
{
  compute_norm();
}

Mesh::Mesh(const CompFab::Vec3 * _v,
  const CompFab::Vec3i * _t)
{
  v.assign(_v,_v+8);
  t.assign(_t,_t+12);
  
  compute_norm();
}

void Mesh::save(std::ostream & out, std::vector<CompFab::Vec3> * vert)
{
  std::string vTok("v");
  std::string fTok("f");
  std::string texTok("vt");
  char bslash='/';
  std::string tok;
  if(vert==0){
    vert = &v;
  }
  for(size_t ii=0;ii<vert->size();ii++){
    out<<vTok<<" "<<(*vert)[ii][0]<<" "<<(*vert)[ii][1]<<" "<<(*vert)[ii][2]<<"\n";
  }
  if(tex.size()>0){
    for(size_t ii=0;ii<tex.size();ii++){
      out<<texTok<<" "<<tex[ii][0]<<" "<<tex[ii][1]<<"\n";
    }
    for(size_t ii=0;ii<t.size();ii++){
      out<<fTok<<" "<<t[ii][0]+1<<bslash<<texId[ii][0]+1<<" "
      <<t[ii][1]+1<<bslash<<texId[ii][1]+1<<" "
      <<t[ii][2]+1<<bslash<<texId[ii][2]+1<<"\n";
    }
  }else{
    for(size_t ii=0;ii<t.size();ii++){
      out<<fTok<<" "<<t[ii][0]+1<<" "<<
          t[ii][1]+1<<" "<<t[ii][2]+1<<"\n";
    }
  }
  out<<"#end\n";
}

void Mesh::save(const char * filename)
{
  std::ofstream out;
  out.open(filename);
  save(out);
  out.close();
}


void Mesh::load(std::istream &in)
{
  read_obj(in);
}

void Mesh::read_obj(std::istream & f)
{
  std::string line;
  std::string vTok("v");
  std::string fTok("f");
  std::string texTok("vt");
  char bslash='/',space=' ';
  std::string tok;
  while(1) {
    std::getline(f,line);
    if(f.eof()) {
      break;
    }
    if(line == "#end"){
      break;
    }
    if(line.size()<3) {
      continue;
    }
    if(line.at(0)=='#') {
      continue;
    }
    std::stringstream ss(line);
    ss>>tok;
    if(tok==vTok) {
      CompFab::Vec3 vec;
      ss>>vec[0]>>vec[1]>>vec[2];
      v.push_back(vec);
    } else if(tok==fTok) {
      bool hasTexture = false;
      if (line.find(bslash) != std::string::npos) {
        std::replace(line.begin(), line.end(), bslash, space);
        hasTexture = true;
      }
      std::stringstream facess(line);
      facess>>tok;
      std::vector<int> vidx;
      std::vector<int> texIdx;
      int x;
      while(facess>>x){
        vidx.push_back(x);
        if(hasTexture){
          facess>>x;
          texIdx.push_back(x);
        }
      }
      texIdx.resize(vidx.size());
      for(int ii = 0;ii<vidx.size()-2;ii++){
        CompFab::Vec3i trig, textureId;
        trig[0] = vidx[0]-1;
        textureId[0] = texIdx[0]-1;
        for (int jj = 1; jj < 3; jj++) {
          trig[jj] = vidx[ii+jj]-1;
          textureId[jj] = texIdx[ii+jj]-1;
        }
        t.push_back(trig);
        texId.push_back(textureId);
      }
    } else if(tok==texTok) {
        CompFab::Vec2f texcoord;
        ss>>texcoord[0];
        ss>>texcoord[1];
        tex.push_back(texcoord);
    }
  }
  std::cout<<"Num Triangles: "<< t.size()<<"\n";
}

void Mesh::read_ply(std::istream & f)
{
  std::string line;
  std::string vertLine("element vertex");
  std::string faceLine("element face");
  std::string texLine("property float s");
  std::string endHeaderLine("end_header");
  while(true) {
    std::getline(f,line);
    if(std::string::npos!=line.find(vertLine)) {
      break;
    }
  }
  std::string token;
  std::stringstream ss(line);
  ss>>token>>token;
  int nvert;
  ss>>nvert;
  bool hasTex=false;
  while(true) {
    std::getline(f,line);
    if(std::string::npos!=line.find(faceLine)) {
      break;
    }
    if(std::string::npos!=line.find(texLine)) {
      hasTex=true;
    }
  }
  std::stringstream ss1(line);
  ss1>>token>>token;
  int nface;
  ss1>>nface;
  while(true) {
    std::getline(f,line);
    if(std::string::npos!=line.find(endHeaderLine)) {
      break;
    }
  }

  v.resize(nvert);
  t.resize(nface);
  if(hasTex) {
    tex.resize(nvert);
  }
  for (int ii =0; ii<nvert; ii++) {
    for (int jj=0; jj<3; jj++) {
      f>>v[ii][jj];
    }
    if(hasTex) {
      for (int jj=0; jj<2; jj++) {
        f>>tex[ii][jj];
      }
      tex[ii][1]=1-tex[ii][1];;
    }
  }
  for (int ii =0; ii<nface; ii++) {
    int nidx;
    f>>nidx;
    for (int jj=0; jj<3; jj++) {
      f>>t[ii][jj];
    }
  }
}

void Mesh::save_obj(const char * filename)
{
  std::ofstream out(filename);
  if(!out.good()){
    std::cout<<"cannot open output file"<<filename<<"\n";
    return;
  }
  save(out);
  out.close();
}

void Mesh::update()
{}

Mesh::Mesh(const char * filename,bool normalize)
{
  load_mesh(filename,normalize);
}


void Mesh::load_mesh(const char * filename, bool normalize)
{
  std::ifstream f ;
  f.open(filename);
  if(!f.good()) {
    std::cout<<"Error: cannot open mesh "<<filename<<"\n";
    return;
  }
  switch(filename[strlen(filename)-1]) {
  case 'y':
    read_ply(f);
    break;
  case 'j':
    read_obj(f);
    break;
  default:
    break;
  }
  if(normalize){
    rescale();
  }
  compute_norm();

  f.close();
}

void Mesh::rescale()
{
  if(v.size()==0){
    std::cout<<"empty mesh\n";
    return;
  }
  CompFab::Vec3 mn=v[0],mx=v[0];

  //scale and translate to [0 , 1]
  for (unsigned int dim = 0; dim<3; dim++) {
    for( size_t ii=0; ii<v.size(); ii++) {
      mn[dim]= std::min(v[ii][dim],mn[dim]);
      mx[dim] = std::max(v[ii][dim],mx[dim]);
    }
    CompFab::precision_type translate = -mn[dim];
    for(size_t ii=0; ii<v.size(); ii++) {
      v[ii][dim]=(v[ii][dim]+translate);
    }
  }

  CompFab::precision_type scale = 1/(mx[0]-mn[0]);
  for(unsigned int dim=1; dim<3; dim++) {
    scale=std::min(1/(mx[dim]-mn[dim]),scale);
  }

  for(size_t ii=0; ii<v.size(); ii++) {
    for (unsigned int dim = 0; dim<3; dim++) {
      v[ii][dim]=v[ii][dim]*scale;
    }
  }
}

void Mesh::compute_norm()
{
    CompFab::Vec3 ZERO;
    
    n.resize(v.size(), ZERO);
  for(unsigned int ii=0; ii<t.size(); ii++) {
    CompFab::Vec3 a = v[t[ii][1]] - v[t[ii][0]];
    CompFab::Vec3 b = v[t[ii][2]] - v[t[ii][0]];
    b=a%b;
    b.normalize();
    for(int jj=0; jj<3; jj++) {
      n[t[ii][jj]]+=b;
      if(t[ii][jj]>=(int)n.size() || t[ii][jj]<0){
        std::cout<<ii<<" "<<jj<<" "<<t[ii][jj]<<" normal computation error\n";
      }
    }
  }
  for(unsigned int ii=0; ii<v.size(); ii++) {
    n[ii].normalize();
  }
}

void BBox(const Mesh & m,
    CompFab::Vec3 & mn, CompFab::Vec3 & mx)
{
  BBox(m.v, mn, mx);
}

bool is_nbr(const CompFab::Vec3i & a, const CompFab::Vec3i&b, int vert)
{
  for (int ii=0; ii<3; ii++) {

    int va=a[ii];
    if(va<=vert) {
      continue;
    }

    for (unsigned int jj=0; jj<3; jj++) {
      int vb=b[jj];
      if(vb<=vert) {
        continue;
      }
      if(va==vb) {
        return true;
      }
    }
  }
  return false;
}


void adjlist(const Mesh & m, std::vector<std::vector<int> > & adjMat)
{
  if(adjMat.size()==m.t.size()) {
    return;
  }
  std::vector<std::vector<int> >trigList;
  trigList.resize(m.v.size());
  for (unsigned int ii=0; ii<m.t.size(); ii++) {
    for (unsigned int jj=0; jj<3; jj++) {
      int vidx=m.t[ii][jj];
      trigList[vidx].push_back(ii);
    }
  }
  adjMat.resize(m.t.size());
  for (unsigned int ii=0; ii<m.v.size(); ii++) {
    int n_nbr=trigList[ii].size();
    for (int jj=0; jj<n_nbr; jj++) {
      int tj=trigList[ii][jj];
      for (int kk=(jj+1); kk<n_nbr; kk++) {
        int tk=trigList[ii][kk];
        if(is_nbr(m.t[tj],m.t[tk],ii)) {
          adjMat[tj].push_back(tk);
          adjMat[tk].push_back(tj);
        }

      }
    }
  }
}


void BBox(const std::vector<CompFab::Vec3 >& v,
    CompFab::Vec3 & mn, CompFab::Vec3 & mx)
{
  mn = v[0];
  mx = v[0];
  for(unsigned int ii = 1 ;ii<v.size();ii++){
    for(int dim = 0 ; dim<3;dim++){
      if(v[ii][dim]<mn[dim]){
        mn[dim] = v[ii][dim];
      }
      if(v[ii][dim]>mx[dim]){
        mx[dim] = v[ii][dim];
      }
    }
  }
}

