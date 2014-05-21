#pragma once

#include <sophus/se3.hpp>
#include <kangaroo/Sdf.h>

#include "SaveGIL.h"
#include "../MarchingCubesGrid.h"

#include <boost/ptr_container/ptr_vector.hpp>

struct KinectKeyframe
{
  KinectKeyframe(int w, int h, Sophus::SE3d T_iw)
    : img(w,h), T_iw(T_iw)
  {
  }

  roo::Image<uchar3, roo::TargetDevice, roo::Manage> img;
  Sophus::SE3d T_iw;
};

inline void SaveMeshlabGrid(std::string sPath,
                            roo::BoundedVolumeGrid<roo::SDF_t, roo::TargetDevice, roo::Manage>& vol)
{
  Eigen::Matrix3d RDFvision;  RDFvision  << 1,0,0,  0,1,0,  0,0,1;
  Eigen::Matrix3d RDFmeshlab; RDFmeshlab << 1,0,0,  0,-1,0, 0,0,-1;
  Eigen::Matrix4d T_vis_ml = Eigen::Matrix4d::Identity();
  T_vis_ml.block<3,3>(0,0) = RDFvision.transpose() * RDFmeshlab;
  Eigen::Matrix4d T_ml_vis = Eigen::Matrix4d::Identity();
  T_ml_vis.block<3,3>(0,0) = RDFmeshlab.transpose() * RDFvision;

  std::string mesh_filename = sPath+ "-Mesh";
  std::string mlp_file =sPath +"project.mlp";
  std::ofstream of(mlp_file);

  of << "<!DOCTYPE MeshLabDocument>" << std::endl;
  of << "<MeshLabProject>" << std::endl;

  of << " <MeshGroup>" << std::endl;

  roo::SaveMeshGrid(mesh_filename, vol);

  of << "  <MLMesh label=\"mesh.ply\" filename=\"" << mesh_filename << ".ply\">" << std::endl;
  of << "   <MLMatrix44>" << std::endl;
  of << "1 0 0 0 " << std::endl;
  of << "0 1 0 0 " << std::endl;
  of << "0 0 1 0 " << std::endl;
  of << "0 0 0 1 " << std::endl;
  of << "</MLMatrix44>" << std::endl;
  of << "  </MLMesh>" << std::endl;

  of << " </MeshGroup>" << std::endl;

  of << " <RasterGroup>" << std::endl;

  of << " </RasterGroup>" << std::endl;

  of << "</MeshLabProject> " << std::endl;

  of.close();
}


inline void SaveMeshlabGrid(std::string sPath,
                            roo::BoundedVolumeGrid<roo::SDF_t, roo::TargetDevice, roo::Manage>& vol,
                            roo::BoundedVolumeGrid<float, roo::TargetDevice, roo::Manage>& GreyVol)
{
  Eigen::Matrix3d RDFvision;  RDFvision  << 1,0,0,  0,1,0,  0,0,1;
  Eigen::Matrix3d RDFmeshlab; RDFmeshlab << 1,0,0,  0,-1,0, 0,0,-1;
  Eigen::Matrix4d T_vis_ml = Eigen::Matrix4d::Identity();
  T_vis_ml.block<3,3>(0,0) = RDFvision.transpose() * RDFmeshlab;
  Eigen::Matrix4d T_ml_vis = Eigen::Matrix4d::Identity();
  T_ml_vis.block<3,3>(0,0) = RDFmeshlab.transpose() * RDFvision;

  std::string mesh_filename = sPath+ "-Mesh";
  std::string mlp_file =sPath +"project.mlp";
  std::ofstream of(mlp_file);

  of << "<!DOCTYPE MeshLabDocument>" << std::endl;
  of << "<MeshLabProject>" << std::endl;

  of << " <MeshGroup>" << std::endl;

  roo::SaveMeshGrid(mesh_filename, vol, GreyVol);

  of << "  <MLMesh label=\"mesh.ply\" filename=\"" << mesh_filename << ".ply\">" << std::endl;
  of << "   <MLMatrix44>" << std::endl;
  of << "1 0 0 0 " << std::endl;
  of << "0 1 0 0 " << std::endl;
  of << "0 0 1 0 " << std::endl;
  of << "0 0 0 1 " << std::endl;
  of << "</MLMatrix44>" << std::endl;
  of << "  </MLMesh>" << std::endl;

  of << " </MeshGroup>" << std::endl;

  of << " <RasterGroup>" << std::endl;

  of << " </RasterGroup>" << std::endl;

  of << "</MeshLabProject> " << std::endl;

  of.close();
}

// generate a single mesh from several PPMs

inline void GenMeshlabFromPPM(std::string              sDirName,
                              std::string              sBBFileName,
                              int3                     VolRes,
                              int                      nGridRes,
                              std::vector<std::string> vfilename,
                              std::string              sMeshFileName)
{
  Eigen::Matrix3d RDFvision;  RDFvision  << 1,0,0,  0,1,0,  0,0,1;
  Eigen::Matrix3d RDFmeshlab; RDFmeshlab << 1,0,0,  0,-1,0, 0,0,-1;
  Eigen::Matrix4d T_vis_ml = Eigen::Matrix4d::Identity();
  T_vis_ml.block<3,3>(0,0) = RDFvision.transpose() * RDFmeshlab;
  Eigen::Matrix4d T_ml_vis = Eigen::Matrix4d::Identity();
  T_ml_vis.block<3,3>(0,0) = RDFmeshlab.transpose() * RDFvision;

  std::string mesh_filename = "mesh";
  std::ofstream of("project.mlp");

  of << "<!DOCTYPE MeshLabDocument>" << std::endl;
  of << "<MeshLabProject>" << std::endl;

  of << " <MeshGroup>" << std::endl;

  roo::GenMeshFromPPM(sDirName, sBBFileName, VolRes, nGridRes, vfilename, sMeshFileName);

  of << "  <MLMesh label=\"mesh.ply\" filename=\"" << mesh_filename << ".ply\">" << std::endl;
  of << "   <MLMatrix44>" << std::endl;
  of << "1 0 0 0 " << std::endl;
  of << "0 1 0 0 " << std::endl;
  of << "0 0 1 0 " << std::endl;
  of << "0 0 0 1 " << std::endl;
  of << "</MLMatrix44>" << std::endl;
  of << "  </MLMesh>" << std::endl;

  of << " </MeshGroup>" << std::endl;

  of << " <RasterGroup>" << std::endl;

  of << " </RasterGroup>" << std::endl;

  of << "</MeshLabProject> " << std::endl;

  of.close();
}

