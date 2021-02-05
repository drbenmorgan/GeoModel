#include "GDMLInterface/GDMLController.h"
#include "ExpressionEvaluator/ExpressionEvaluator.h"
#include <iostream>
#include <string>

#include "GeoModelKernel/GeoShape.h"
#include "GeoModelKernel/GeoElement.h"
#include "GeoModelKernel/GeoMaterial.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoPhysVol.h"

#include "GDMLInterface/isotope.h"

std::string stripPointer(const std::string str)
{
	const std::string ptr="0x";
	size_t pos=str.find(ptr);
	if (pos!=std::string::npos)
	  return str.substr(0,pos);
	else
	  return str;
}


GDMLController::GDMLController(std::string name): controllerName(name) {
	theXMLStore=XMLHandlerStore::GetHandlerStore();
	theEvaluator=ExpressionEvaluator::GetEvaluator();

	registerHandlers();
}

ExpressionEvaluator* GDMLController::Evaluator()
{
	return theEvaluator;
}

XMLHandlerStore* GDMLController::XMLStore()
{
	return theXMLStore;
}

void GDMLController::saveSolid(std::string name, GeoShape*  sh)
{
	if (theSolids.find(name)!=theSolids.end())
	{
		 std::cout << " Warning! Shape "<< name << " already in store!"<<std::endl;
	}
	else
		theSolids[name]=sh;
}
GeoShape* GDMLController::retrieveSolid(std::string name)
{
	if (theSolids.find(name)!=theSolids.end())
	{
		return theSolids[name];
	}
	else
	{
		std::cout << " Warning! Shape "<< name << " not found in store! returning 0"<<std::endl;
		return 0;
	}
}
void GDMLController::saveIsotope(std::string name, isotope  i)
{
	if (theIsotopes.find(name)!=theIsotopes.end())
	{
		 std::cout << " Warning! isotope "<< name << " already in store!"<<std::endl;
	}
	else
		theIsotopes[name]=i;
}
isotope& GDMLController::retrieveIsotope(std::string name)
{
	if (theIsotopes.find(name)!=theIsotopes.end())
	{
		return theIsotopes[name];
	}
	else
	{
		static isotope empty=isotope();
		std::cout << " Warning! Isotope "<< name << " not found in store! returning empty"<<std::endl;
		return empty;
	}
}

void GDMLController::savePosition(std::string name, position p)
{
	if (thePositions.find(name)!=thePositions.end())
	{
		 std::cout << " Warning! position "<< name << " already in store!"<<std::endl;
	}
	else
		thePositions[name]=p;
}
position& GDMLController::retrievePosition(std::string name)
{
	if (thePositions.find(name)!=thePositions.end())
	{
		return thePositions[name];
	}
	else
	{
		static position empty=position();
		std::cout << " Warning! Position "<< name << " not found in store! returning empty"<<std::endl;
		return empty;
	}
}

void GDMLController::saveRotation(std::string name, rotation p)
{
	if (theRotations.find(name)!=theRotations.end())
	{
		 std::cout << " Warning! rotation "<< name << " already in store!"<<std::endl;
	}
	else
		theRotations[name]=p;
}
rotation& GDMLController::retrieveRotation(std::string name)
{
	if (theRotations.find(name)!=theRotations.end())
	{
		return theRotations[name];
	}
	else
	{
		static rotation empty=rotation();
		std::cout << " Warning! rotation "<< name << " not found in store! returning empty"<<std::endl;
		return empty;
	}
}

void GDMLController::saveElement(std::string name, GeoElement* e)
{
	if (theElements.find(name)!=theElements.end())
	{
		 std::cout << " Warning! element "<< name << " already in store!"<<std::endl;
	}
	else
		theElements[name]=e;
}
GeoElement* GDMLController::retrieveElement(std::string name)
{
	if (theElements.find(name)!=theElements.end())
	{
		return theElements[name];
	}
	else
	{
		static GeoElement* empty=nullptr;
		std::cout << " Warning! element "<< name << " not found in store! returning empty"<<std::endl;
		return empty;
	}
}

void GDMLController::saveMaterial(std::string name, GeoMaterial* m)
{
	if (theMaterials.find(name)!=theMaterials.end())
	{
		 std::cout << " Warning! material "<< name << " already in store!"<<std::endl;
	}
	else
		theMaterials[name]=m;
}
GeoMaterial* GDMLController::retrieveMaterial(std::string ref)
{
	//std::string name=stripPointer(ref);
	std::string name=ref;
	if (theMaterials.find(name)!=theMaterials.end())
	{
		return theMaterials[name];
	}
	else
	{
		static GeoMaterial* empty=0;
		std::cout << " Warning! material "<< name << " not found in store! returning empty"<<std::endl;
		return empty;
	}
}

void GDMLController::saveLogicalVolume(std::string name, GeoVolume lv)
{
	if (theLogVolumes.find(name)!=theLogVolumes.end())
	{
		 std::cout << " Warning! logical volume "<< name << " already in store!"<<std::endl;
	}
	else
		theLogVolumes[name]=lv;
}
GeoVolume GDMLController::retrieveLogicalVolume(std::string name)
{
	if (theLogVolumes.find(name)!=theLogVolumes.end())
	{
		return theLogVolumes[name];
	}
	else
	{
		std::cout << " Warning! logical volume "<< name << " not found in store! returning empty"<<std::endl;
		return std::make_pair(nullptr,nullptr);
	}
}

#include "GDMLInterface/defineHandler.h"
#include "GDMLInterface/boxHandler.h"
#include "GDMLInterface/tubeHandler.h"
#include "GDMLInterface/topTreeHandler.h"
#include "GDMLInterface/materialsHandler.h"
#include "GDMLInterface/solidsHandler.h"
#include "GDMLInterface/structureHandler.h"
#include "GDMLInterface/MEEHandler.h"
#include "GDMLInterface/atomHandler.h"
#include "GDMLInterface/fractionHandler.h"
#include "GDMLInterface/isotopeHandler.h"
#include "GDMLInterface/elementHandler.h"
#include "GDMLInterface/materialHandler.h"
#include "GDMLInterface/densityHandler.h"
#include "GDMLInterface/temperatureHandler.h"
#include "GDMLInterface/positionHandler.h"
#include "GDMLInterface/rotationHandler.h"
#include "GDMLInterface/materialrefHandler.h"
#include "GDMLInterface/solidrefHandler.h"
#include "GDMLInterface/volumerefHandler.h"
#include "GDMLInterface/volumeHandler.h"
#include "GDMLInterface/physvolHandler.h"
#include "GDMLInterface/setupHandler.h"
#include "GDMLInterface/worldHandler.h"
#include "GDMLInterface/physvolHandler.h"
#include "GDMLInterface/coneHandler.h"
#include "GDMLInterface/eltubeHandler.h"
#include "GDMLInterface/paraHandler.h"
#include "GDMLInterface/torusHandler.h"
#include "GDMLInterface/trdHandler.h"
#include "GDMLInterface/polyconeHandler.h"
#include "GDMLInterface/polyhedraHandler.h"
#include "GDMLInterface/zplaneHandler.h"
#include "GDMLInterface/unionHandler.h"
#include "GDMLInterface/subtractionHandler.h"
#include "GDMLInterface/intersectionHandler.h"
#include "GDMLInterface/booleanHandler.h"
#include "GDMLInterface/trapHandler.h"
#include "GDMLInterface/twistedTrapHandler.h"
#include "GDMLInterface/tessellatedHandler.h"
#include "GDMLInterface/triangularHandler.h"
#include "GDMLInterface/quadrangularHandler.h"
#include "GDMLInterface/xtruHandler.h"
#include "GDMLInterface/arb8Handler.h"
#include "GDMLInterface/sectionHandler.h"
#include "GDMLInterface/twoDimVertexHandler.h"
#include "GDMLInterface/unidentifiedShapeHandler.h"

void GDMLController::registerHandlers()
{
	//std::cout << "This is GDMLController::registerHandlers()" << std::endl;
	new defineHandler("define",this);
	new boxHandler("box",this);
	new tubeHandler("tube",this);
	new topTreeHandler("gdml",this);
	new materialsHandler("materials",this);
	new solidsHandler("solids",this);
	new structureHandler("structure",this);
	new MEEHandler("MEE",this);
	new atomHandler("atom",this);
	new fractionHandler("fraction",this);
	new isotopeHandler("isotope",this);
	new elementHandler("element",this);
	new materialHandler("material",this);
	new densityHandler("D",this);
	new temperatureHandler("T",this);
	new positionHandler("position",this);
	new positionHandler("positionref",this);
	new rotationHandler("rotation",this);
	new rotationHandler("rotationref",this);
	new positionHandler("firstposition",this);
	new positionHandler("firstpositionref",this);
	new rotationHandler("firstrotation",this);
	new rotationHandler("firstrotationref",this);
	new materialrefHandler("materialref",this);
	new solidrefHandler("solidref",this);
	new volumerefHandler("volumeref",this);
	new volumeHandler("volume",this);
	new physvolHandler("physvol",this);
	new setupHandler("setup",this);
	new worldHandler("world",this);
	new coneHandler("cone",this);
	new eltubeHandler("eltube",this);
	new paraHandler("para",this);
	new torusHandler("torus",this);
	new trdHandler("trd",this);
	new polyconeHandler("polycone",this);
	new polyhedraHandler("polyhedra",this);
	new zplaneHandler("zplane",this);
	new booleanHandler("first",this);
	new booleanHandler("second",this);
	new unionHandler("union",this);
	new subtractionHandler("subtraction",this);
	new intersectionHandler("intersection",this);
	new trapHandler("trap",this);
        new twistedTrapHandler("twistedtrap",this);
	new tessellatedHandler("tessellated",this);
	new triangularHandler("triangular",this);
	new quadrangularHandler("quadrangular",this);
	new xtruHandler("xtru",this);
	new arb8Handler("arb8",this);
	new twoDimVertexHandler("twoDimVertex",this);
	new sectionHandler("section",this);
        new unidentifiedShapeHandler("cutTube",this);
}
