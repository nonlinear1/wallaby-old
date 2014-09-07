#pragma once

#include "Kangaroo/Mat.h"
#include "Kangaroo/Image.h"
#include "BoundedVolumeGrid.h"
#include "Kangaroo/ImageIntrinsics.h"
#include "Kangaroo/Sdf.h"

namespace roo
{

// --------------------------------------------------------------------------------------------------------------------------
void RaycastSdf(Image<float> depth, Image<float4> norm, Image<float> img,
                const BoundedVolumeGrid<SDF_t,roo::TargetDevice, roo::Manage> vol,
                const Mat<float,3,4> T_wc, ImageIntrinsics K, float near, float far,
                float trunc_dist, bool subpix = true);

// raycast grid sdf
void RaycastSdf(Image<float> depth, Image<float4> norm, Image<float> img,
                const BoundedVolumeGrid<SDF_t,roo::TargetDevice, roo::Manage> vol,
                const BoundedVolumeGrid<float,roo::TargetDevice, roo::Manage> grayVol,
                const Mat<float,3,4> T_wc, ImageIntrinsics K, float near, float far,
                float trunc_dist, bool subpix );
}