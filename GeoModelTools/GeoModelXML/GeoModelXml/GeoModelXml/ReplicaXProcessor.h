/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GEO_MODEL_XML_REPLICAX_PROCESSOR_H
#define GEO_MODEL_XML_REPLICAX_PROCESSOR_H
#include <xercesc/util/XercesDefs.hpp>
//
//   Processor for replicaX tags.
//
#include <string>
#include <map>

#include "GeoModelXml/ElementProcessor.h"
class GmxUtil;

class ReplicaXProcessor: public ElementProcessor {
public:
    void process(const xercesc::DOMElement *element, GmxUtil &gmxUtil, GeoNodeList &toAdd);
private:
    std::map <std::string, GeoNodeList> m_map; 
    void tokenize(std::string&,char,std::vector<std::string>&) const ;
};

#endif // REPLICAX_PROCESSOR_H
