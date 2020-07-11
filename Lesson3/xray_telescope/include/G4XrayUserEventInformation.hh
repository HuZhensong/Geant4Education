// G4XrayUserEventInformation.hh
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

#ifndef G4XRAYUSEREVENTINFORMATION_HH
#define G4XRAYUSEREVENTINFORMATION_HH 1

#include <G4VUserEventInformation.hh>
#include <G4Types.hh>

#include <map>
#include <vector>

class G4VPhysicalVolume;

class G4XrayUserEventInformation : public G4VUserEventInformation {
public:
  G4XrayUserEventInformation();
  void Print() const {}
  void RegisterTrack(G4int trackID);
  void AddReflection(G4int trackID,  const G4VPhysicalVolume * physvolumeID);
  std::map<G4int, std::vector<const G4VPhysicalVolume *> > & GetReflections() const;
  virtual ~G4XrayUserEventInformation() {}
private:
  mutable std::map<G4int, std::vector<const G4VPhysicalVolume *> > m_reflections;
};


#endif // G4XRAYUSEREVENTINFORMATION_HH
