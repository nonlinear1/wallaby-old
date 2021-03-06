// by lu.ma@colorado.edu

#ifndef SAVEPPMGRID_H
#define SAVEPPMGRID_H

#include "kangaroo/extra/SavePPM.h"
#include "BoundedVolumeGrid.h"
#include "RollingGridSDF.h"

// P1	Portable bitmap	ASCII
// P2	Portable graymap	ASCII
// P3	Portable pixmap	ASCII
// P4	Portable bitmap	Binary
// P5	Portable graymap	Binary
// P6	Portable pixmap	Binary

/////////////////////////////////////////////////////////////////////////////
// Save Volume types
/////////////////////////////////////////////////////////////////////////////

KANGAROO_EXPORT
template<typename T, typename Manage>
void SavePXM(
    std::ofstream&                                         bFile,
    const roo::VolumeGrid<T,roo::TargetHost,Manage>&       vol,
    std::string                                            ppm_type = "P5",
    int                                                    num_colors = 255)
{
  bFile << ppm_type << std::endl;
  bFile << vol.w << " " << vol.h << " " << vol.d << '\n';
  bFile << num_colors << '\n';

  for(unsigned int d=0; d<vol.d; ++d) {
    for(unsigned int r=0; r<vol.h; ++r) {
      bFile.write( (const char*)vol.RowPtr(r,d), vol.w * sizeof(T) );
    }
  }
  bFile.close();
}

KANGAROO_EXPORT
template<typename T, typename Manage>
void SavePXM(
    const std::string                                      filename,
    const roo::VolumeGrid<T,roo::TargetHost,Manage>&       vol,
    std::string                                            ppm_type = "P5",
    int                                                    num_colors = 255)
{
  std::ofstream bFile( filename.c_str(), std::ios::out | std::ios::binary );
  SavePXM<T,Manage>(bFile, vol, ppm_type, num_colors);
}

KANGAROO_EXPORT
template<typename T, typename Manage>
void SavePXM(
    std::ofstream& bFile,
    const roo::VolumeGrid<T,roo::TargetDevice,Manage>&     vol,
    std::string                                            ppm_type = "P5",
    int                                                    num_colors = 255)
{
  roo::VolumeGrid<T,roo::TargetHost,roo::Manage> hvol;
  hvol.InitVolume(vol.w, vol.h, vol.d);
  hvol.CopyFrom(vol);
  SavePXM(bFile, hvol, ppm_type, num_colors);
}

KANGAROO_EXPORT
template<typename T, typename Manage>
void SavePXM(
    const std::string                                      filename,
    const roo::VolumeGrid<T,roo::TargetDevice,Manage>&     vol,
    std::string                                            ppm_type = "P5",
    int                                                    num_colors = 255)
{
  std::ofstream bFile( filename.c_str(), std::ios::out | std::ios::binary );
  SavePXM<T,Manage>(bFile,vol,ppm_type,num_colors);
}

KANGAROO_EXPORT
void SavePXMBoundingBox(
    const std::string                                      filename,
    roo::BoundingBox                                       BBox);

KANGAROO_EXPORT
template<typename T, typename Manage>
void SavePXM(
    const std::string                                      filename,
    roo::BoundedVolumeGrid<T,roo::TargetDevice, Manage>&   vol,
    std::string                                            ppm_type = "P5",
    int                                                    num_colors = 255)
{
  // load data from device to host
  roo::BoundedVolumeGrid<T,roo::TargetHost, Manage> hvol;
  hvol.Init(vol.m_w,vol.m_h,vol.m_d,vol.m_nVolumeGridRes,vol.m_bbox);
  hvol.CopyAndInitFrom(vol);

  // first save the bounding box
  std::string sBBFileName = filename+"-BB";
  SavePXMBoundingBox(sBBFileName, vol.m_bbox);

  // save each active volume in BoundedVolumeGrid to HardDisk
  for(int i=0; i!=vol.GetTotalGridNum(); i++)
  {
    if(hvol.CheckIfBasicSDFActive(i))
    {
      // save
      std::string sFileName = filename + "-" + std::to_string(i);
      std::ofstream bFile( sFileName.c_str(), std::ios::out | std::ios::binary );
      SavePXM<T,Manage>(bFile,hvol.m_GridVolumes[i],ppm_type,num_colors);
    }
  }

  std::cout<<"[SavePPMGrid.h] Fininsh save grid SDF. "<<std::endl;
}

///============================= Save Grid SDFs ================================
KANGAROO_EXPORT
bool CheckIfBBfileExist(std::string filename);

KANGAROO_EXPORT
template<typename T, typename Manage>
void CheckifSaveBB(
    const std::string                                      sFilename,
    int3                                                   GlobalIndex,
    roo::BoundedVolumeGrid<T,roo::TargetDevice, Manage>&   rDVol)
{
  std::string sBBFileName = sFilename +"-BB#"+ std::to_string(GlobalIndex.x) + "#"+
      std::to_string(GlobalIndex.y) + "#"+ std::to_string(GlobalIndex.z);

  if( CheckIfBBfileExist(sBBFileName) == false)
  {
    // noticed that the BB box here is the one after shift is applied
    SavePXMBoundingBox(sBBFileName, rDVol.GetDesireBB(GlobalIndex));
  }
}


// file name format ObjID-GlobalIndex-LocalIndex or ObjID-LocalIndex
KANGAROO_EXPORT
template<typename T, typename Manage>
void SavePXMGridDesire(
    const std::string                                      sPathName,
    int                                                    pGridNeedSave[],
    int                                                    pGlobalIndex_x[],
    int                                                    pGlobalIndex_y[],
    int                                                    pGlobalIndex_z[],
    roo::BoundedVolumeGrid<T,roo::TargetDevice, Manage>&   rDVol,
    bool                                                   bSaveBBox,
    std::string                                            ppm_type = "P5",
    int                                                    num_colors = 255)
{
  if(rDVol.GetActiveGridVolNum() == 0)
  {
    std::cerr<<"[Kangaroo/SavePXMGridDesire] Cannot save PXM for void volume."<<std::endl;
    exit(-1);
  }
  else
  {
    // ------------------------------------------------------------------------
    // load data from device to host
    roo::BoundedVolumeGrid<T,roo::TargetHost, Manage> HVol;
    HVol.Init(rDVol.m_w,rDVol.m_h,rDVol.m_d,rDVol.m_nVolumeGridRes,rDVol.m_bbox);
    HVol.CopyAndInitFrom(rDVol);
    HVol.m_global_shift = rDVol.m_global_shift;

    // ------------------------------------------------------------------------
    // save each active volume in BoundedVolumeGrid to HardDisk
    int nSavedGridNum =0;

    for(int i=0; i!=static_cast<int>(rDVol.m_nGridNum_w); i++)
    {
      for(int j=0; j!=static_cast<int>(rDVol.m_nGridNum_h); j++)
      {
        for(int k=0; k!=static_cast<int>(rDVol.m_nGridNum_d); k++)
        {
          // here we don't consider any shift as the grid for saving does not
          // had any shift applied
          int nGridIndex = i + rDVol.m_nGridNum_w* (j+ rDVol.m_nGridNum_h* k);

          // --- save vol if necessary
          if( pGridNeedSave[nGridIndex]==1 &&
              HVol.CheckIfBasicSDFActive(nGridIndex) )
          {
            int3 GlobalIndex = make_int3(pGlobalIndex_x[nGridIndex],
                                         pGlobalIndex_y[nGridIndex],
                                         pGlobalIndex_z[nGridIndex]);

            int3 LocalIndex  = make_int3(i,j,k);

            std::string sGridFileName = sPathName+"#"+
                std::to_string(GlobalIndex.x)+"#"+std::to_string(GlobalIndex.y)+"#"+
                std::to_string(GlobalIndex.z)+"#"+std::to_string(LocalIndex.x)+"#"+
                std::to_string(LocalIndex.y)+"#"+std::to_string(LocalIndex.z);

            std::ofstream bFile( sGridFileName.c_str(), std::ios::out | std::ios::binary );
            SavePXM<T,Manage>(bFile, HVol.m_GridVolumes[nGridIndex], ppm_type, num_colors);
            nSavedGridNum++;

            // scan the disk and see if we need to save the bb (in global pose)
            if(bSaveBBox)
            {
              CheckifSaveBB(sPathName, GlobalIndex, rDVol);
            }
          }
        }
      }
    }

    printf("\n[Kangaroo/SavePXMGridDesire] Save %d grid sdf in Global Pose.\n", nSavedGridNum);
  }

}


// file name format ObjID-GlobalIndex-LocalIndex or ObjID-LocalIndex
KANGAROO_EXPORT
template<typename T, typename Manage>
void SavePXMGridDesire(
    const std::string                                        sPathName,
    int                                                      pGridNeedSave[],
    int                                                      pGlobalIndex_x[],
    int                                                      pGlobalIndex_y[],
    int                                                      pGlobalIndex_z[],
    roo::BoundedVolumeGrid<T,roo::TargetDevice, Manage>&     rDVol,
    roo::BoundedVolumeGrid<float,roo::TargetDevice, Manage>& rDColorVol,
    bool                                                     bSaveBBox,
    std::string                                              ppm_type = "P5",
    int                                                      num_colors = 255)
{
  if(rDVol.GetActiveGridVolNum() == 0)
  {
    std::cerr<<"[Kangaroo/SavePXMGridDesire] Cannot save PXM for void volume."<<std::endl;
    exit(-1);
  }
  else
  {
    // ------------------------------------------------------------------------
    // load data from device to host
    roo::BoundedVolumeGrid<T,roo::TargetHost, Manage> HVol;
    HVol.Init(rDVol.m_w,rDVol.m_h,rDVol.m_d,rDVol.m_nVolumeGridRes,rDVol.m_bbox);
    HVol.CopyAndInitFrom(rDVol);
    HVol.m_global_shift = rDVol.m_global_shift;

    // init color sdf
    roo::BoundedVolumeGrid<float,roo::TargetHost, Manage> HColorVol;
    HColorVol.Init(rDColorVol.m_w, rDColorVol.m_h, rDColorVol.m_d,
                   rDColorVol.m_nVolumeGridRes, rDColorVol.m_bbox);
    HColorVol.CopyAndInitFrom(rDColorVol);
    HColorVol.m_global_shift = rDColorVol.m_global_shift;

    // ------------------------------------------------------------------------
    // save each active volume in BoundedVolumeGrid to HardDisk
    int nSavedGridNum =0;

    for(int i=0; i!=static_cast<int>(rDVol.m_nGridNum_w); i++)
    {
      for(int j=0; j!=static_cast<int>(rDVol.m_nGridNum_h); j++)
      {
        for(int k=0; k!=static_cast<int>(rDVol.m_nGridNum_d); k++)
        {
          // here we don't consider any shift as the grid for saving does not
          // had any shift applied
          int nGridIndex = i + rDVol.m_nGridNum_w* (j+ rDVol.m_nGridNum_h* k);

          // --- save vol if necessary
          if( pGridNeedSave[nGridIndex]==1 &&
              HVol.CheckIfBasicSDFActive(nGridIndex) )
          {
            int3 GlobalIndex = make_int3(pGlobalIndex_x[nGridIndex],
                                         pGlobalIndex_y[nGridIndex],
                                         pGlobalIndex_z[nGridIndex]);

            int3 LocalIndex  = make_int3(i,j,k);

            std::string sGridFileName = sPathName+"#"+
                std::to_string(GlobalIndex.x)+"#"+std::to_string(GlobalIndex.y)+"#"+
                std::to_string(GlobalIndex.z)+"#"+std::to_string(LocalIndex.x)+"#"+
                std::to_string(LocalIndex.y)+"#"+std::to_string(LocalIndex.z);

            std::string sColorGridFileName = sPathName+"-Color-"+"#"+
                std::to_string(GlobalIndex.x)+"#"+std::to_string(GlobalIndex.y)+"#"+
                std::to_string(GlobalIndex.z)+"#"+std::to_string(LocalIndex.x)+"#"+
                std::to_string(LocalIndex.y)+"#"+std::to_string(LocalIndex.z);

            std::ofstream bFile( sGridFileName.c_str(), std::ios::out | std::ios::binary );
            SavePXM<T,Manage>(bFile, HVol.m_GridVolumes[nGridIndex], ppm_type, num_colors);

            std::ofstream bColorFile( sColorGridFileName.c_str(), std::ios::out | std::ios::binary );
            SavePXM<float,Manage>(bColorFile, HColorVol.m_GridVolumes[nGridIndex], ppm_type, num_colors);

            nSavedGridNum++;

            // scan the disk and see if we need to save the bb (in global pose)
            if(bSaveBBox)
            {
              CheckifSaveBB(sPathName, GlobalIndex, rDVol);
            }
          }
        }
      }
    }

    printf("\n[Kangaroo/SavePXMGridDesire] Save %d grid sdf in Global Pose.\n", nSavedGridNum);
  }

}


#endif // SAVEPPMGRID_H
