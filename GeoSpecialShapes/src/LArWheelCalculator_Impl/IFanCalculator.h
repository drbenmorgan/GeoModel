/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __LArWheelCalculator_Impl_IFanCalculator_H__
#define __LArWheelCalculator_Impl_IFanCalculator_H__

#include "GeoModelKernel/GeoDefinitions.h"

namespace LArWheelCalculator_Impl
{

  /// @class IFanCalculator
  /// Abstract interface for fan calculator classes that handle distance
  /// calculation to parts of the LAr endcap.
  ///
  class IFanCalculator
  {

    public:

      /// Virtual destructor
      virtual ~IFanCalculator() {};

      /// @name Geometry methods
      /// @{

      virtual double DistanceToTheNearestFan(GeoTrf::Vector3D &p,
                                             int & out_fan_number) const = 0;

      virtual int PhiGapNumberForWheel(int i) const = 0;

      virtual std::pair<int, int> GetPhiGapAndSide(const GeoTrf::Vector3D &p) const = 0;

      /// @}

  };

}

#endif // __LArWheelCalculator_Impl_IFanCalculator_H__
