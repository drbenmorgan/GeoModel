/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// BarrelConstruction
// Insert the LAr Barrel into a pre-defined mother volume.
// Author: Gaston Parrour  translated to GeoModel by G.Unal

#ifndef LARGEOBARREL_BARRELCONSTRUCTION_H
#define LARGEOBARREL_BARRELCONSTRUCTION_H

#include "GeoModelKernel/GeoFullPhysVol.h"
#include "GeoGenericFunctions/FunctionNoop.h"

class LArGeoMaterialManager;
class GeoPublisher;

namespace LArGeo {

  class BarrelConstruction
  {
  public:
    BarrelConstruction(bool fullGeo
		       , LArGeoMaterialManager* matman
		       , GeoPublisher* publisher);
    virtual ~BarrelConstruction();

    // Get the envelope containing this detector.
    GeoFullPhysVol* GetPositiveEnvelope();
    GeoFullPhysVol* GetNegativeEnvelope();

    // Set parameters for the barrel.
    void setBarrelSagging(bool flag)        {m_A_SAGGING  = flag;}
    void setBarrelCellVisLimit(int maxCell) {m_NVISLIM    = maxCell;}

  private: 
    void MakeEnvelope();

    // It is illegal to copy a BarrelConstruction:
    BarrelConstruction (const BarrelConstruction &);

    // It is illegal to assign a BarrelConstruction:
    BarrelConstruction & operator= (const BarrelConstruction &);

    // Three auxiliary functions:------------------------------------//
    //                                                               //
    GeoGenfun::FunctionNoop Fx( double r,                            //
				GeoGenfun::GENFUNCTION G,            //
				const double Cenx[],                 //
				const double Ceny[] ) const;         //
    //                                                               //
    GeoGenfun::FunctionNoop Fy( double r,                            //
				GeoGenfun::GENFUNCTION G,            //
				const double Cenx[],                 //
				const double Ceny[] ) const;         //
    //                                                               //
    GeoGenfun::FunctionNoop Dely(GeoGenfun::GENFUNCTION G ) const;   //
    GeoGenfun::FunctionNoop Del1(GeoGenfun::GENFUNCTION G ) const;   //
    GeoGenfun::FunctionNoop Del2(GeoGenfun::GENFUNCTION G ) const;   //
    //---------------------------------------------------------------//

    // Atan2 for Generic Functions..:--------------------------------//
    GeoGenfun::FunctionNoop ATan2(GeoGenfun::GENFUNCTION y,          //
				  GeoGenfun::GENFUNCTION x) const;   //
    //---------------------------------------------------------------//

    bool                    m_A_SAGGING;
    int                     m_NVISLIM;

    // volumes that are private member variables:
    GeoFullPhysVol*  m_ecamPhysicalPos;
    GeoFullPhysVol*  m_ecamPhysicalNeg;

    bool             m_fullGeo;  // true->FULL, false->RECO
    LArGeoMaterialManager* m_matman;
    GeoPublisher*          m_publisher;
  };
}  // namespace LArGeo

#endif // LARGEOBARREL_BARRELCONSTRUCTION_H
