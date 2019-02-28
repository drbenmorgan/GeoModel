/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GEOMATERIAL2G4_Geo2G4MaterialFactory_h
#define GEOMATERIAL2G4_Geo2G4MaterialFactory_h

#include "G4Material.hh"
#include "GeoModelKernel/GeoMaterial.h"

#include <map>
#include <string>

typedef std::map<const GeoMaterial* , G4Material*, std::less<const GeoMaterial*> > matList;
typedef std::map<std::string, const GeoMaterial*, std::less<std::string> > matNames;

class Geo2G4MaterialFactory {
public:
  Geo2G4MaterialFactory();
  G4Material* Build(const GeoMaterial*);

private:
  matList m_definedMaterials;
  matNames m_definedMatNames;

};

#endif
