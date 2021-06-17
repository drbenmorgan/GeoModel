/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GEO_MODEL_XML_MAKE_ELLIPTICAL_TUBE_H
#define GEO_MODEL_XML_MAKE_ELLIPTICAL_TUBE_H
#include <xercesc/util/XercesDefs.hpp>

#include "GeoModelXml/Element2GeoItem.h"

class MakeEllipticalTube: public Element2GeoItem {
public:
    MakeEllipticalTube();
    const RCBase * make(const xercesc::DOMElement *element, GmxUtil &gmxUtil) const;
};

#endif // GEO_MODEL_XML_MAKE_ELLIPTICAL_TUBE_H
