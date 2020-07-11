//  Giuseppe Vacanti/Ernst-Jan Buis (cosine Science & Computing BV)
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

#ifndef G4XrayGrazingAngleScattering_h
#define G4XrayGrazingAngleScattering_h 1

#include <G4VDiscreteProcess.hh>
#include <G4ForceCondition.hh>
#include <globals.hh>

class G4Track;
class G4Step;
class G4ParticleDefinition;
class G4VPhysicalVolume;
class G4XrayRefractionIndex;


class G4XrayGrazingAngleScattering : public G4VDiscreteProcess {

public:
  enum photon_mode_t { particle, intensity };

  G4XrayGrazingAngleScattering(G4int verbosity = 0, G4double intensity_threshold = 1e-3, photon_mode_t mode=particle);
  ~G4XrayGrazingAngleScattering();

  G4double GetMeanFreePath(const G4Track& ,  G4double , G4ForceCondition* condition);
  
  G4VParticleChange* PostStepDoIt(const G4Track& aTrack,  const G4Step&  aStep);
  
  G4bool IsApplicable(const G4ParticleDefinition& aParticle);

  G4int GetVerbosity() const;
  void SetVerbosity(G4int verbosity);


  // Basic physics functionality that can be used from G4XraySurfaceProperty:
  //

  // Reflectivity from vacuum into a thick medium
  static void ReflectivityFromVacuum(G4double grazingAngle, G4complex postRefractionIndex,
				     G4double & sReflectivity, G4double & pReflectivity);
  static void ReflectivityFromVacuum(G4double energy, G4double grazingAngle, G4complex postRefractionIndex,
				     G4double & unpolarizedReflectivity);

  static void reflectance(G4double energy, G4double grazingAngle, G4complex preRefractionIndex, G4complex postRefractionIndex,G4double & sReflectivity, G4double & pReflectivity);

  static void Reflectance(G4double energy, G4double grazingAngle, G4complex preRefractionIndex, G4complex postRefractionIndex, G4double & unpolarizedReflectivity) ;

  // Specular fraction
  static G4double SpecularFraction(G4double energy, G4double grazing_angle, G4double roughness,G4complex postRefractionIndex);

  // This is the possible interface 
  //   static void reflectance(G4double energy, G4double grazingAngle, G4complex preRefractionIndex, G4complex postRefractionIndex,
  // 			  G4double & sReflectivity, G4double & pReflectivity);
  //   static void reflectance(G4double energy, G4double grazingAngle, G4complex preRefractionIndex, G4complex postRefractionIndex,
  // 			  G4double & unpolarizedReflectivity);
  
private:
  const mode_t theMode;
  std::map<size_t, G4XrayRefractionIndex *> theRefractionIndexMap;

  const G4double theIntensityThreshold;
   G4int theVerbosity;

};

#endif
