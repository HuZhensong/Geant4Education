//  Giuseppe Vacanti (cosine Science & Computing BV)
//  May 15, 2008
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


#ifndef G4XRAYSPECULARREFLECTINGSURFACE_HH
#define G4XRAYSPECULARREFLECTINGSURFACE_HH

#include <G4XraySurfaceProperty.hh>


// ======================================================================
//
// Specular reflection, possibly with reflectivity=1;
//

class G4XraySpecularReflectingSurface : public G4XraySurfaceProperty {
public:
  G4XraySpecularReflectingSurface(const G4String& name, G4double roughness = 0, G4bool force_unit_reflectivity = false);
  G4XraySpecularReflectingSurface(G4double roughness = 0, G4bool force_unit_reflectivity = false);

  virtual double roughness() const;

  virtual G4double reflect(G4double photonEnergy,
			   G4complex preRefractionIndex,
			   G4complex postRefractionIndex,
			   G4ThreeVector & surfaceNormal,
			   G4ThreeVector & photonDirection,
			   const G4ThreeVector & photonPosition) const;

  virtual ~G4XraySpecularReflectingSurface() {}
  
  static void specular_reflect(const G4ThreeVector & surfaceNormal,
			       G4ThreeVector & photonDirection);
    
private:
  const double m_roughness;
  const G4bool m_force_unit_reflectivity;
};

#endif
