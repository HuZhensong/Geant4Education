//  Giuseppe Vacanti (cosine Science & Computing BV)
//  March 28, 2008
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


#ifndef G4XRAYSURFACEPROPERTY_HH
#define G4XRAYSURFACEPROPERTY_HH

#include <G4SurfaceProperty.hh>
#include <G4ThreeVector.hh>

class G4XraySurfaceProperty : public G4SurfaceProperty {
public:
  G4XraySurfaceProperty(const G4String& name)
    : G4SurfaceProperty(name, x_ray) {}

  virtual ~G4XraySurfaceProperty() {}

  // Returns the surface rms roughness
  virtual G4double roughness() const = 0;

  // Changes (reflects) the photon:
  //
  // + photonEnergy: the energy of the photon.
  //
  // + localSurfaceNormal is the normal pointing out of the
  // reflecting surface; the normal might be change locally due to the
  // details of the surface: the new normal is returned.
  //
  // + photonDirection is the direction of the photon; this gets
  // changed by the reflection process.
  //
  // + pre/postRefractionIndex: the refraction index (at photonEnergy)
  // of the material the photon is propagating through (pre) and might
  // reflect on (post).
  //
  // The method returns the reflectivity used in the reflection process.

  virtual G4double reflect(G4double photonEnergy,
			   G4complex preRefractionIndex,
			   G4complex postRefractionIndex,
			   G4ThreeVector & localSurfaceNormal,
			   G4ThreeVector & photonDirection,
			   const G4ThreeVector & localPhotonPosition) const = 0;

};


#endif
