// G4XrayUserEventInformation.cc
//
// Giuseppe Vacanti (cosine Science & Computing BV)
// March 19, 2009
//
// This file is part of the XRTG4 Geant4 extension.

//     The XRTG4 Geant4 extension is free software: you can
//     redistribute it and/or modify it under the terms of the GNU
//     General Public License as published by the Free Software
//     Foundation, either version 2 of the License, or (at your
//     option) any later version.

//     XRTG4 Geant4 extension is distributed in the hope that it will
//     be useful, but WITHOUT ANY WARRANTY; without even the implied
//     warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
//     PURPOSE.  See the GNU General Public License for more details.

//     You should have received a copy of the GNU General Public
//     License along with XRTG4 Geant4 extension.  If not, see
//     <http://www.gnu.org/licenses/>.
//
// ********************************************************************
//
//     XRTG4 was an extension for very early geant4.9 or before, which
//     doesn't work properly on newer version, such as geant4-10.3.3.
//     胡祯崧 HU Zhensong made a series of change on this toolkit to
//     make it adapt to the newer version.
//
// ********************************************************************
//
//  CHANGE HISTORY
//  --------------

// ********************************************************************
#include "G4XrayUserEventInformation.hh"

#include <G4VPhysicalVolume.hh>

G4XrayUserEventInformation::G4XrayUserEventInformation() : G4VUserEventInformation() {}

void G4XrayUserEventInformation::AddReflection(G4int trackID, const G4VPhysicalVolume * physvolumeID) {
  m_reflections[trackID].push_back(physvolumeID);
}

void G4XrayUserEventInformation::RegisterTrack(G4int trackID) {
  m_reflections[trackID];
}

std::map<G4int, std::vector<const G4VPhysicalVolume *> > & G4XrayUserEventInformation::GetReflections() const {
  return m_reflections;
}
