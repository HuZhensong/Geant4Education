//  cosine Science & Computing BV
//  April  3, 2008
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


#ifndef G4XrayRefractionIndex_h
#define G4XrayRefractionIndex_h

#include "globals.hh"

#include <map>
#include <utility>

class G4XrayRefractionIndex {
public:
  G4XrayRefractionIndex(const G4String & fileName);
  ~G4XrayRefractionIndex();
  
  std::pair<G4double, G4double> GetEnergyRange() const;
  G4complex GetRefractionIndex(G4double energy) const;

  static G4String FindOpticalConstantsFile(const G4String & materialName);

private:

  G4String m_file;
  typedef  std::map< G4double, G4complex, std::less<G4double> > map_t;
  map_t m_index;
  G4double m_e_min, m_e_max;
  void read(std::istream& is);

};

#endif
