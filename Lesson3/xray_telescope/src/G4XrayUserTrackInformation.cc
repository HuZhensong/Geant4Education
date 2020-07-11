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
//
// ********************************************************************

#include <G4XrayUserTrackInformation.hh>

G4XrayUserTrackInformation::G4XrayUserTrackInformation()
 : G4VUserTrackInformation() {}

size_t G4XrayUserTrackInformation::NumberOfInteractions() const {
  return m_track_information.size();
}

void G4XrayUserTrackInformation::AddInformation(const interaction_data_t & info) {
  m_track_information.push_back(info);
}

const std::vector<G4XrayUserTrackInformation::interaction_data_t> & G4XrayUserTrackInformation::GetTrackInformation() const {
  return m_track_information;
}


void G4XrayUserTrackInformation::Print() const {

}

void G4XrayUserTrackInformation::interaction_data_t::print_on(std::ostream & os) const {

  os << position.x() << " " << position.y() << " " << position.z() << " "
     << incidentDirection.x() << " " << incidentDirection.y() << " " << incidentDirection.z() << " "
     << reflectedDirection.x() << " " << reflectedDirection.y() << " " << reflectedDirection.z() << " "
     << originalNormal.x() << " " << originalNormal.y() << " " << originalNormal.z() << " "
     << perturbedNormal.x() << " " << perturbedNormal.y() << " " << perturbedNormal.z() << " "
     << intensity << " " 
     << reflectivity << " "
     << static_cast<unsigned int>(status);
     
}


void G4XrayUserTrackInformation::interaction_data_t::read_from(std::istream & is) {
  G4double value;
  is >> value;
  position.setX(value);
  is >> value;
  position.setY(value);
  is >> value;
  position.setZ(value);
  
  is >> value;
  incidentDirection.setX(value);
  is >> value;
  incidentDirection.setY(value);
  is >> value;
  incidentDirection.setZ(value);
  
  is >> value;
  reflectedDirection.setX(value);
  is >> value;
  reflectedDirection.setY(value);
  is >> value;
  reflectedDirection.setZ(value);
  
  is >> value;
  originalNormal.setX(value);
  is >> value;
  originalNormal.setY(value);
  is >> value;
  originalNormal.setZ(value);
  
  is >> value;
  perturbedNormal.setX(value);
  is >> value;
  perturbedNormal.setY(value);
  is >> value;
  perturbedNormal.setZ(value);
  
  is >> intensity;
  is >> reflectivity;
  unsigned long ulvalue;
  is >> ulvalue;
  status = static_cast<G4XrayUserTrackInformation::status_t>(ulvalue);
}
