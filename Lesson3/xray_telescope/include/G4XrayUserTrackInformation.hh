//  Giuseppe Vacanti (cosine Science & Computing BV)
//  July  3, 2008
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


#ifndef G4XRAYUSERTRACKINFORMATION_HH 
#define G4XRAYUSERTRACKINFORMATION_HH 

#include <vector>
#include <iosfwd>

#include <G4ThreeVector.hh>
#include <G4VTouchable.hh>
#include <G4VPhysicalVolume.hh>

#include <G4VUserTrackInformation.hh>

#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"


class G4XrayUserTrackInformation : public G4VUserTrackInformation {

public:
  G4XrayUserTrackInformation();

  enum status_t { alive, killed_by_chance, below_intensity_threshold, no_xray_interaction, process_anomaly, geometry_anomaly, refraction_index_range, just_started }; 

  struct interaction_data_t {
    void print_on(std::ostream & os) const;
    void read_from(std::istream & is);
    interaction_data_t() : reflectivity(-1), intensity(-1), status(no_xray_interaction), volume(0), touchable(0) {}
    G4double reflectivity;
    G4ThreeVector position; // where the interaction has taken place
    G4ThreeVector incidentDirection;
    G4ThreeVector reflectedDirection;
    G4ThreeVector originalNormal;
    G4ThreeVector perturbedNormal;
    G4double intensity; // before interaction
    status_t status;
    const G4VPhysicalVolume * volume; // where the interaction has taken place
  private:
    const G4VTouchable * touchable; // where the interaction has taken place
  } ;
      
  size_t NumberOfInteractions() const;
  
  void AddInformation(const interaction_data_t & info);
  
  const std::vector<interaction_data_t> & GetTrackInformation() const;

  void Print() const;

private:

  std::vector<interaction_data_t> m_track_information;
};


#endif
