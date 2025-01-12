//---------------------------------------------------------------------------//
//! \file CelerRunAction.hh
//! \brief Minimal Geant4 RunAction for interfacing to Celeritas SimpleOffload 
//---------------------------------------------------------------------------//
#pragma once

#include <G4UserRunAction.hh>

#include "Celeritas.hh"

class CelerRunAction : public G4UserRunAction
{
  public:
    void BeginOfRunAction(G4Run const* run)
    {
        CelerSimpleOffload().BeginOfRunAction(run);
    }

    void EndOfRunAction(G4Run const* run)
    {
        CelerSimpleOffload().EndOfRunAction(run);
    }
};
