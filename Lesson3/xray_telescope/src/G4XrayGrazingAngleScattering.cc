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
//  2018.03.?? 胡祯崧 HU Zhensong
//  - Add #include "G4SystemOfUnits.hh"
//        #include "G4PhysicalConstants.hh"
//    Because the newer version of XRTG writes all of the units in these two files.
//
//  2018.04.09 胡祯崧 HU Zhensong
//  - Add NumOfPhotonsEnterMirror in Ln 101 217 228 to show how many photons entering the mirror, thus by dividing it with number of photons entering the detecotr, we can know that it's true.
//
//  2018.05.?? 胡祯崧 HU Zhensong
//  - Changed the reflectivity which derived from X-ray refraction index.
//
// ********************************************************************

#include "G4XrayGrazingAngleScattering.hh"
//#include "CLHEP/Units/SystemOfUnits.h"

#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"
#include <G4XrayRefractionIndex.hh>
#include <G4XraySurfaceProperty.hh>

#include <G4Gamma.hh>
#include <G4StepPoint.hh>
#include <G4LogicalSurface.hh>
#include <G4Navigator.hh>
#include <G4TransportationManager.hh>
#include <G4LogicalBorderSurface.hh>
#include <G4LogicalSkinSurface.hh>
#include <G4RunManager.hh>
#include <G4EventManager.hh>

#include <G4XrayUserTrackInformation.hh>
#include <G4XrayUserEventInformation.hh>

#include <cmath>
using namespace std;
namespace {
  // internal use

  // verbosity threshold for the physical process
  enum { silent = 0, physics_details = 2, surface_details = 5, track_details = 8, all_details = 10};

  const G4String myname("G4XrayGrazingAngleScattering");

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4XrayGrazingAngleScattering::G4XrayGrazingAngleScattering(G4int verbosity, G4double intensity_threshold, photon_mode_t mode)
  : G4VDiscreteProcess("G4XrayGrazingAngleScattering"), theMode(mode), theIntensityThreshold(intensity_threshold), theVerbosity(verbosity)
{
  if(theVerbosity)
    G4cout << myname << " initialized." << G4endl;

  if(theIntensityThreshold < 0)
  {
    G4Exception(myname.c_str(), "G4XRAY001", FatalErrorInArgument, "Intensity threshold must be non negative.");
  } 
  else if(theIntensityThreshold > 1)
   {
    G4Exception(myname.c_str(), "G4XRAY001", JustWarning, "Intensity threshold is larger than 1, which is probably not what was intended.");
  }    

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4int G4XrayGrazingAngleScattering::GetVerbosity() const {return theVerbosity;}


void G4XrayGrazingAngleScattering::SetVerbosity(G4int verbosity) { theVerbosity = verbosity; }


G4XrayGrazingAngleScattering::~G4XrayGrazingAngleScattering() {}
 

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

 
G4double G4XrayGrazingAngleScattering::GetMeanFreePath(const G4Track& ,  G4double , G4ForceCondition* condition) 
{    
  *condition = Forced;
  return DBL_MAX;
}
  

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......


G4VParticleChange* G4XrayGrazingAngleScattering::PostStepDoIt(const G4Track& aTrack,  const G4Step&  aStep) {
  
 // static int NumOfPhotonsEnterMirror = 0;  Here it's not the right place to add that.

  G4RunManager * runman = G4RunManager::GetRunManager();
  if(theVerbosity >= all_details){
    G4cout << "======================================================================" << G4endl;
    G4cout << myname << " (PostStepDoIt start): eventID=" << runman->GetCurrentEvent()->GetEventID()
	   << " trackID=" << aTrack.GetTrackID() << " energy=" << aTrack.GetVertexKineticEnergy()
	   << " weight=" << aTrack.GetWeight() << G4endl;
  }

  aParticleChange.Initialize(aTrack);

  // Track info data, if available, must be properly initialized as
  // soon as possible, and certainly before the first occurrence of
  // return G4VDiscreteProcess::PostStepDoIt, as tracks can drop out
  // of the process for a variety of reasons and never actually reach
  // the X-ray specific part of the process.

  G4XrayUserTrackInformation * track_info = dynamic_cast<G4XrayUserTrackInformation *>(aTrack.GetUserInformation());
  G4XrayUserTrackInformation::interaction_data_t track_info_data; // this has a default initalization

  G4XrayUserEventInformation * event_info = dynamic_cast<G4XrayUserEventInformation *>(runman->GetCurrentEvent()->GetUserInformation());
  if(event_info)  
  {
    event_info->RegisterTrack(aTrack.GetTrackID());
  }

  G4StepPoint* pPostStepPoint = aStep.GetPostStepPoint();
  if(track_info)
  {
    //    track_info_data.touchable = pPostStepPoint->GetTouchable();
    track_info_data.volume = pPostStepPoint->GetPhysicalVolume();
    track_info_data.intensity = aTrack.GetWeight();
    if(theVerbosity >= all_details)
    {
      //      G4cout << "touchable=" << track_info_data.touchable << " intensity=" << track_info_data.intensity << G4endl;
      G4cout << "physical volume=" << track_info_data.volume << "(" << track_info_data.volume->GetName()
	     << ") intensity=" << track_info_data.intensity << G4endl;
    }
  }

  const G4DynamicParticle * photon = aTrack.GetDynamicParticle();
  G4ThreeVector theGlobalPhotonDirection = photon->GetMomentumDirection();
  
  const G4ThreeVector theGlobalPoint = pPostStepPoint->GetPosition();
  G4Navigator * theNavigator = G4TransportationManager::GetTransportationManager()->GetNavigatorForTracking();

  const G4AffineTransform & glob2loc = theNavigator->GetGlobalToLocalTransform();
  const G4AffineTransform & loc2glob = theNavigator->GetLocalToGlobalTransform();

  const G4ThreeVector theLocalPoint = glob2loc.TransformPoint(theGlobalPoint);
    
  if(track_info)
  {
    track_info_data.incidentDirection = theGlobalPhotonDirection;
    track_info_data.position = theGlobalPoint;
    if(theVerbosity >= all_details)
    {
      G4cout << "incident direction=" << track_info_data.incidentDirection << " position=" << track_info_data.position << G4endl;
    }
  }

  // presumably this can never happen, as we are a boundary process?
  if (pPostStepPoint->GetStepStatus() != fGeomBoundary)
  {
    if(track_info)
    {
      track_info_data.status = G4XrayUserTrackInformation::process_anomaly;
      track_info->AddInformation(track_info_data);
    }
    G4cerr << myname << ": eventID=" << runman->GetCurrentEvent()->GetEventID()
	   << "\n trackID=" << aTrack.GetTrackID() << " called when post step point step status is not fGeomBoundary."
    // << "\n volName=" << aTrack.GetVolume()->GetName()
    // << "\n next volName=" << aTrack.GetNextVolume()->GetName() 
    // << "\n pre step pos=" << aStep.GetPreStepPoint()->GetPosition()
 	  // << "\n post step pos=" << aStep.GetPostStepPoint()->GetPosition()
    //  << "\n now pos="<< aTrack.GetPosition()
      << "\nstatus=" << static_cast<unsigned int>(G4XrayUserTrackInformation::process_anomaly) << G4endl;
    return G4VDiscreteProcess::PostStepDoIt(aTrack, aStep);
  }

  // Below threshold, we are immediately gone.
  if(aTrack.GetWeight() < theIntensityThreshold){
    if(track_info){
      track_info_data.status = G4XrayUserTrackInformation::below_intensity_threshold;
      track_info->AddInformation(track_info_data);
    }
    if(theVerbosity >= all_details){
      G4cout << "status=" << static_cast<unsigned long>(G4XrayUserTrackInformation::below_intensity_threshold) << G4endl;
    }
    aParticleChange.ProposeTrackStatus(fStopAndKill);
    return G4VDiscreteProcess::PostStepDoIt(aTrack, aStep);
  }

  G4StepPoint* pPreStepPoint  = aStep.GetPreStepPoint();

  const G4VPhysicalVolume * prePhysicalVolume = pPreStepPoint->GetPhysicalVolume();
  const G4VPhysicalVolume * postPhysicalVolume = pPostStepPoint->GetPhysicalVolume();

  G4Material * pre_mat =  pPreStepPoint->GetMaterial();
  G4Material * post_mat =  pPostStepPoint->GetMaterial();
  const size_t pre_mat_idx = pre_mat->GetIndex();
  const size_t post_mat_idx = post_mat->GetIndex();

  
  if(theRefractionIndexMap.find(pre_mat_idx) == theRefractionIndexMap.end()){
    theRefractionIndexMap[pre_mat_idx] = new G4XrayRefractionIndex(G4XrayRefractionIndex::FindOpticalConstantsFile(pre_mat->GetName()));
  }
  if(theRefractionIndexMap.find(post_mat_idx) == theRefractionIndexMap.end()){
    theRefractionIndexMap[post_mat_idx] = new G4XrayRefractionIndex(G4XrayRefractionIndex::FindOpticalConstantsFile(post_mat->GetName()));
  }

  const G4double photonEnergy = photon->GetKineticEnergy();

  const G4XrayRefractionIndex * pre_ref_idx = theRefractionIndexMap[pre_mat_idx];
  const std::pair<G4double, G4double> pre_range = pre_ref_idx->GetEnergyRange();

  const G4XrayRefractionIndex * post_ref_idx = theRefractionIndexMap[post_mat_idx];
  const std::pair<G4double, G4double> post_range = post_ref_idx->GetEnergyRange();

  if(theVerbosity >= all_details){
  //  NumOfPhotonsEnterMirror += 1;
    G4cout << "preStep information: position=" << pPreStepPoint->GetPosition()
	   << " direction=" << pPreStepPoint->GetMomentumDirection()
	   << " material=" << pre_mat->GetName()
	   << " refraction index range=[" << pre_range.first << "," << pre_range.second << "]"
	   << " physical volume=" << prePhysicalVolume->GetName() << G4endl;
    G4cout << "postStep information: position=" << pPostStepPoint->GetPosition()
	   << " direction=" << pPostStepPoint->GetMomentumDirection()
	   << " material=" << post_mat->GetName()
	   << " refraction index range=[" << post_range.first << "," << post_range.second << "]"
	   << " physical volume=" << postPhysicalVolume->GetName() 
      //     << " Number of photons entering the mirror=" << NumOfPhotonsEnterMirror 
           << G4endl;
    
  }
  
  if(pre_range.first > photonEnergy){
    aParticleChange.ProposeTrackStatus(fStopAndKill);
    aParticleChange.ProposeEnergy(0);
    aParticleChange.ProposeLocalEnergyDeposit(photonEnergy);
    if(track_info){
      track_info_data.status = G4XrayUserTrackInformation::refraction_index_range;
      track_info->AddInformation(track_info_data);
    }

    if(theVerbosity >= physics_details){
      G4cout << myname << ": photon with energy[keV]=" << photonEnergy/keV
	     << " cannot be handled by material " << pre_mat->GetName()
	     << " because its energy is too low: track killed.\nstatus="
	     <<  static_cast<unsigned long>(G4XrayUserTrackInformation::refraction_index_range) << G4endl;
    }

    return G4VDiscreteProcess::PostStepDoIt(aTrack,aStep);
  }

  if(pre_range.second < photonEnergy){
    if(track_info){
      track_info_data.status = G4XrayUserTrackInformation::refraction_index_range;      
      track_info->AddInformation(track_info_data);
    }
    if(theVerbosity >= physics_details){
      G4cout << myname << ": photon with energy[keV]=" << photonEnergy/keV
	     << " cannot be handled by material " << pre_mat->GetName()
	     << " because its energy is too high: track passed on unchanged.\nstatus="
	     << static_cast<unsigned long>(G4XrayUserTrackInformation::refraction_index_range) << G4endl;
    }
    return G4VDiscreteProcess::PostStepDoIt(aTrack,aStep);
  }

  if(post_range.first > photonEnergy){
    aParticleChange.ProposeTrackStatus(fStopAndKill);
    aParticleChange.ProposeEnergy(0);
    aParticleChange.ProposeLocalEnergyDeposit(photonEnergy);
    if(track_info){
      track_info_data.status = G4XrayUserTrackInformation::refraction_index_range;      
      track_info->AddInformation(track_info_data);
    }
    if(theVerbosity >= physics_details){
      G4cout << myname << ": photon with energy[keV]=" << photonEnergy/keV
	     << " cannot be handled by material " << post_mat->GetName()
	     << " because its energy is too low: track killed.\nstatus="
	     << static_cast<unsigned long>(G4XrayUserTrackInformation::refraction_index_range) << G4endl;
    }
    return G4VDiscreteProcess::PostStepDoIt(aTrack,aStep);
  }
  if(post_range.second < photonEnergy){
    if(track_info){
      track_info_data.status = G4XrayUserTrackInformation::refraction_index_range;      
      track_info->AddInformation(track_info_data);
    }
    if(theVerbosity >= physics_details){
      G4cout << myname << ": photon with energy[keV]=" << photonEnergy/keV
	     << " cannot be handled by material " << post_mat->GetName()
	     << " because its energy is too high: track passed on unchanged.\nstatus="
	     << static_cast<unsigned long>(G4XrayUserTrackInformation::refraction_index_range) << G4endl;
    }
    return G4VDiscreteProcess::PostStepDoIt(aTrack,aStep);
  }

  const G4complex pre_n = pre_ref_idx->GetRefractionIndex(photonEnergy);
  const G4complex post_n = post_ref_idx->GetRefractionIndex(photonEnergy);

  G4LogicalSurface* surface = 0;

  // Can have a Skin or a Border Surface

  surface = G4LogicalBorderSurface::GetSurface(prePhysicalVolume, postPhysicalVolume);

  if(!surface){
    if(theVerbosity >= surface_details){
      G4cout << myname << ": no border surface between physical volumes "
	     << prePhysicalVolume->GetName() << " and " <<  postPhysicalVolume->GetName()
	     << " . Considering a skin surface around " << postPhysicalVolume->GetName() << G4endl;
    }
    surface = G4LogicalSkinSurface::GetSurface(postPhysicalVolume->GetLogicalVolume());
  }
  
  if(!surface){
    if(track_info){
      track_info_data.status = G4XrayUserTrackInformation::no_xray_interaction;      
      track_info->AddInformation(track_info_data);
    }
    if(theVerbosity >= surface_details){
      G4cout << myname << ": no skin surface around logical volume "
	     << postPhysicalVolume->GetLogicalVolume()->GetName() << ": track passed on unchanged.\nstatus="
	     << static_cast<unsigned long>(G4XrayUserTrackInformation::no_xray_interaction) << G4endl;
    }
    return G4VDiscreteProcess::PostStepDoIt(aTrack, aStep);
  } else {
    if(theVerbosity >= surface_details){
      G4cout << myname << ": skin surface found with name=" << surface->GetName() << G4endl;
    }
  }

  const G4SurfaceProperty * property = surface->GetSurfaceProperty();

  if(!property){
    if(track_info){
      track_info_data.status = G4XrayUserTrackInformation::no_xray_interaction;
      track_info->AddInformation(track_info_data);
    }
    if(theVerbosity >= surface_details){
      G4cout << myname << ": there is no surface property, track passed on unchanged.\nstatus="
	     << static_cast<unsigned long>(G4XrayUserTrackInformation::no_xray_interaction) << G4endl;
    }
    return G4VDiscreteProcess::PostStepDoIt(aTrack, aStep);
  } else {
    if(theVerbosity >= surface_details){
      G4cout << myname << ": surface property found with name=" << property->GetName() << G4endl;
    }
  }
  
  // FIXME: a future version of Geant will make the dynamic_cast possible. GV submitted a bug report.
  //  const G4XraySurfaceProperty * xsurface = dynamic_cast<const G4XraySurfaceProperty *>(property);
  const G4XraySurfaceProperty * xsurface = (const G4XraySurfaceProperty *)(property);
  
  if(!xsurface){
    if(track_info){
      track_info_data.status = G4XrayUserTrackInformation::no_xray_interaction;
      track_info->AddInformation(track_info_data);
    }
    if(theVerbosity >= surface_details){
      G4cout << myname << ": it is not an X-ray surface property: track passed on unchanged."
	     << "\nstatus=" << static_cast<unsigned long>(G4XrayUserTrackInformation::no_xray_interaction) << G4endl;
    }
    return G4VDiscreteProcess::PostStepDoIt(aTrack, aStep);
  } else {
    if(theVerbosity >= surface_details){
      G4cout << myname << ": it is an X-ray surface property: going for the interaction."  << G4endl;
    }
  }

  G4bool valid = false;
  G4ThreeVector theLocalSurfaceNormal = -theNavigator->GetLocalExitNormal(&valid);
  if(not valid){
    if(track_info){
      track_info_data.status = G4XrayUserTrackInformation::geometry_anomaly;
      track_info->AddInformation(track_info_data);
    }
    G4cerr << myname << ": the navigator reports an invalid local normal between the physical volumes "
      	   << pPreStepPoint ->GetPhysicalVolume()->GetName() << " and " << pPostStepPoint->GetPhysicalVolume()->GetName()
	   << "\nstatus=" << static_cast<unsigned long>(G4XrayUserTrackInformation::geometry_anomaly) << G4endl;    
    return G4VDiscreteProcess::PostStepDoIt(aTrack, aStep);
  }
  
  if(track_info){
    track_info_data.originalNormal = loc2glob.TransformAxis(theLocalSurfaceNormal);
    if(theVerbosity >= all_details){
      G4cout << "original normal=" << track_info_data.originalNormal << G4endl;
    }
  }
  
  G4ThreeVector theLocalPhotonDirection =  glob2loc.TransformAxis(theGlobalPhotonDirection);
  
  const G4double reflectivity = xsurface->reflect(photonEnergy, pre_n, post_n,//Here is reflectivity!! what is n,reflectivity ? YeS, IT's the n = 1- delta - i*beta
						  theLocalSurfaceNormal,
						  theLocalPhotonDirection,
						  theLocalPoint);
  if(event_info){
    event_info->AddReflection(aTrack.GetTrackID(), postPhysicalVolume);
  }

  theGlobalPhotonDirection = loc2glob.TransformAxis(theLocalPhotonDirection);
  
  if(track_info){
    track_info_data.reflectivity = reflectivity;
    track_info_data.perturbedNormal = loc2glob.TransformAxis(theLocalSurfaceNormal);
    track_info_data.reflectedDirection = theGlobalPhotonDirection;
  }
  if(theVerbosity >= all_details){
    const G4ThreeVector perturbedNormal =  loc2glob.TransformAxis(theLocalSurfaceNormal);
    G4cout << "perturbed normal=" << perturbedNormal <<  " incidence angle="
	   << 0.5*M_PI - perturbedNormal.angle(theLocalPhotonDirection)
	   << " reflected direction=" << theGlobalPhotonDirection
	   << " scattering angle=" <<  0.5*M_PI - perturbedNormal.angle(theGlobalPhotonDirection)
	   << " reflectivity=" << reflectivity << G4endl;
  }


  G4bool kill_track = false;

  if(track_info){
    track_info_data.status = G4XrayUserTrackInformation::alive;
  }
  if(theVerbosity >= all_details){
    G4cout << "status=" << static_cast<unsigned long>(G4XrayUserTrackInformation::alive) << G4endl;
  }

  switch(theMode) {
  case particle:
    if(G4UniformRand() >= reflectivity)
      kill_track = true;
    if(track_info){
      if(kill_track){
	track_info_data.status = G4XrayUserTrackInformation::killed_by_chance;
	if(theVerbosity >= all_details){
	  G4cout << "status=" << static_cast<unsigned long>(G4XrayUserTrackInformation::killed_by_chance) << G4endl;
	}
      }
    }
    break;
  case intensity:
    aParticleChange.ProposeWeight(aTrack.GetWeight()*reflectivity);
    break;
  default:
    // FIXME: introduce proper handling --> stop
    ;
  }
  aParticleChange.SetNumberOfSecondaries(0);
  if(kill_track){
    aParticleChange.ProposeTrackStatus(fStopAndKill);
  }

  aParticleChange.ProposeMomentumDirection(theGlobalPhotonDirection);
  aParticleChange.ProposeEnergy(photonEnergy);

  if(track_info){
    track_info->AddInformation(track_info_data);
  }
  return G4VDiscreteProcess::PostStepDoIt(aTrack, aStep);

}
  

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......


G4bool G4XrayGrazingAngleScattering::IsApplicable(const G4ParticleDefinition& aParticle) { return ( &aParticle == G4Gamma::Gamma());}


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......


// Reflectivity from vacuum onto a thick medium


//void G4XrayGrazingAngleScattering::ReflectivityFromVacuum(G4double grazingAngle, G4complex postRefractionIndex,
//							  G4double & sReflectivity, G4double & pReflectivity) 
//{
//  if(grazingAngle < 0 and grazingAngle > M_PI_2) 
//  {
//    ostringstream msg;
//    msg << "grazingAngle=" << grazingAngle << " is not in [0,PI].";
//    G4Exception("G4XrayGrazingAngleScattering::ReflectivityFromVacuum","0",EventMustBeAborted,	msg.str().c_str());
//  }

//  const G4double theta = M_PI_2 - grazingAngle;
  
//  const G4double n = postRefractionIndex.real();
//  const G4double k = -postRefractionIndex.imag();
//  const G4double costheta = cos(theta);
//  const G4double sintheta = sin(theta);
//  const G4double tantheta = sintheta/costheta;
//  const G4double c = sqr(n) - sqr(k) - sqr(sintheta);

//  const G4double gg = sqrt(sqr(c) + sqr(2*n*k));
//  const G4double a2 = 0.5*(gg+c);
//  const G4double a = sqrt(a2);
//  const G4double b2 = 0.5*(gg-c);

//  const G4double f = a2 + b2;
//  const G4double v = f + sqr(costheta);
//  const G4double w = 2*a*costheta;
//  sReflectivity = (v - w) / (v + w);

//  const G4double vv = f + sqr(sintheta)*sqr(tantheta);
//  const G4double ww = 2*a*sintheta*tantheta;
//  pReflectivity = sReflectivity * (vv - ww) / (vv + ww);
    
//}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......



void G4XrayGrazingAngleScattering::ReflectivityFromVacuum(G4double energy, G4double grazingAngle, G4complex postRefractionIndex, G4double & unpolarizedReflectivity) 
{
  
  const G4double delta = 1-postRefractionIndex.real();
  const G4double beta = -postRefractionIndex.imag();
//  const G4double thetac = 81*sqrt(delta) * (1*deg);
  //const G4double angle = (M_PI_2 - grazingAngle)* (1*deg);//grazingAngle;//pi/2-(grazingAngle+M_PI_2);
  const G4double angle = grazingAngle;

if(angle < 0 or angle > M_PI_2) 
  {
    ostringstream msg;
    msg << "grazingAngle=" << angle << " is not in [0,PI].";
    G4Exception("G4XrayGrazingAngleScattering::ReflectivityFromVacuum","0",EventMustBeAborted,	msg.str().c_str());
  }

  const G4complex post = postRefractionIndex;
  const G4complex i = (post-post.real())/abs(post-post.real());
  const G4complex n = 1 - delta - i * beta;
  const G4double sinangle = sin(angle);
  const G4double cosangle = cos(angle);
  const G4complex qs = sqrt(n*n-cosangle*cosangle);

  const G4complex Rs = sqr(abs(sinangle - qs)/abs(sinangle + qs));
  const G4complex Rp = sqr(abs(n*n*sinangle-qs)/abs(n*n*sinangle+qs));
 
  const G4double Rsreal = Rs.real();
  const G4double Rpreal = Rp.real();
  unpolarizedReflectivity = 0.5*(Rsreal+Rpreal);


  
//  G4cout << "energy = " << energy/eV << "\n" 
//         << "deg angle = " << angle*180/3.1415926 << "\n"
//         << "grazingangle  = " << grazingAngle<< "\n"
//         << "sin angle = " << sinangle << "\n"
//         << "reflectivity = " << unpolarizedReflectivity << "\n"<< G4endl;


}




// void G4XrayGrazingAngleScattering::reflectance(G4double energy, G4double grazingAngle, G4complex preRefractionIndex, G4complex postRefractionIndex,
// 		 G4double & sReflectivity, G4double & pReflectivity) {

//   // REMARK: This implementation is copied from xmbtracer, but I do
//   // not understand all of it. It implements Fresnel's formulae, as
//   // per equation 20 (section 1.5) of Born & Wolf, where the
//   // amplitudes of the transmitted and reflected waves are calculated
//   // for two polarizations.

//   // FIXME: better assert would be useful
//   assert(grazingAngle > 0);
//   assert(grazingAngle <= M_PI_2);
//   assert(energy > 0);

//   // The photon travels from the pre side (medium 1) to the post side
//   // (medium 2). The angle is the grazing angle, and we change it
//   // immediately to the angle measured from the normal.


//   const G4complex & n1 = preRefractionIndex;
//   const G4complex & n2 = postRefractionIndex;
//   const G4double angle = M_PI_2 - grazingAngle;
//   const G4complex s1 = sin(angle)/n1;
//   const G4complex c1 = sqrt(1. - s1*s1);
//   const G4complex n12 = n1/n2;
//   const G4complex s2 = s1*n12;
//   const G4complex c2 = sqrt(1. - s2*s2);

//   const G4complex c1n12 = c1*n12;
//   const G4complex s_denom = c1n12+c2;
//   const G4bool s_total_reflection = abs(s_denom) != 0.;
//   const G4complex s_amplitude_r = (s_total_reflection ? (c1n12 - c2)/s_denom : 1.0);
 //  const G4complex s_amplitude_t = (s_total_reflection ? 2.0*c1n12/s_denom : 0.);


 //  const G4complex c2n12 = c2*n12;
 //  const G4complex p_denom = c2n12+c1;
 //  const G4bool p_total_reflection = abs(p_denom) != 0;
 //  const G4complex p_amplitude_r = (p_total_reflection ? (c2n12 - c1)/p_denom : 1.0);
 //  const G4complex p_amplitude_t = (p_total_reflection ? 2.0*c1n12/p_denom : 0.);
   
 //  const G4double srreal = s_amplitude_r.real();
 //  const G4double streal = s_amplitude_t.real();
 //  const G4double srimag = s_amplitude_r.imag();
 //  const G4double stimag = s_amplitude_t.imag();

 //  const G4double prreal = p_amplitude_r.real();
 //  const G4double ptreal = p_amplitude_t.real();
 //  const G4double primag = p_amplitude_r.imag();
 //  const G4double ptimag = p_amplitude_t.imag();

//   const G4double sreal = srreal+streal;
 //  const G4double simag = srimag+stimag;
  // const G4double preal = prreal+ptreal;
 //  const G4double pimag = primag+ptimag;
   
 //  sReflectivity = sqrt(sreal*sreal+simag*simag);
 //  pReflectivity = sqrt(preal*preal+pimag*pimag);

 //  sReflectivity = sqrt(s_amplitude_r*s_amplitude_r+s_amplitude_t*s_amplitude_t);
 //  pReflectivity = sqrt(p_amplitude_r*p_amplitude_r+p_amplitude_t*p_amplitude_t);

// }




//void G4XrayGrazingAngleScattering::Reflectance(G4double energy, G4double grazingAngle, G4complex //preRefractionIndex, G4complex postRefractionIndex,
// 		 G4double & unpolarizedReflectivity) {

//   G4double sReflectivity = 0, pReflectivity = 0;
//   reflectance(energy, grazingAngle, preRefractionIndex, postRefractionIndex, sReflectivity, pReflectivity);

//   G4double angle = pi/2-grazingAngle;
//   G4double lambda = h_Planck * c_light/ energy;
//   G4double qz = 2*pi/lambda*(2*sin(angle));
//   G4
  
  // unpolarizedReflectivity =  0.5*(sReflectivity+pReflectivity);
    
 //}






G4double G4XrayGrazingAngleScattering::SpecularFraction(G4double energy, G4double grazingAngle, G4double rmsRoughness,G4complex postRefractionIndex) {

  static const char * origin = "G4XrayGrazingAngleScattering::SpecularFraction";
  
  if(energy <= 0){
    ostringstream msg;
    msg << "energy=" << energy << " must be > 0.";
    G4Exception(origin,
		"0",
		EventMustBeAborted,
		msg.str().c_str());
  }

  if(rmsRoughness < 0){
    ostringstream msg;
    msg << "roughness=" << rmsRoughness << " must be > 0.";
    G4Exception(origin,
		"0",
		RunMustBeAborted,
		msg.str().c_str());
  }

  if(grazingAngle < 0) {
    ostringstream msg;
    msg << "grazingAngle=" << grazingAngle << " must be > 0.";
    G4Exception(origin,
		"0",
		EventMustBeAborted,
		msg.str().c_str());
  }

  const G4double wavelength = h_Planck*c_light/energy;
  //const G4double exponent = std::pow(4*pi*rmsRoughness*std::sin(grazingAngle)/wavelength, 2.); //Here is to calculate the reflectivity  
  //const G4double specular_fraction = std::exp(-exponent);
  const G4complex n = postRefractionIndex;
  const G4double sinangle = sin(grazingAngle);
  const G4double cosangle = cos(grazingAngle);
  const G4double kiz = 2*M_PI /wavelength * sinangle;
  const G4complex ktz = 2*M_PI /wavelength * sqrt(n*n - cosangle*cosangle);
  const G4double ktz_real = ktz.real();
  const G4double frac = std::exp(-2*kiz*ktz_real*rmsRoughness*rmsRoughness);
  const G4double specular_fraction = frac*frac;


/*
  G4cout  << "\n rmsRoughness=" << rmsRoughness
          << "\n grazing ang="  << grazingAngle * 180/M_PI 
          << "\n wavelength="   << wavelength
          << "\n energy="       << energy 
          << "\n kiz="          << kiz
          << "\n ktz_real="     << ktz_real
          << "\n M_PI="         << M_PI
          << "\n specular fra="  << specular_fraction << G4endl;*/

  return specular_fraction;  
}


