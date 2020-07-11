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
//  2018.03.26 胡祯崧 HU Zhensong
//  -  Line 95 10 148 152 162 173 189 , add ,"0",FatalException,"G4XrayRefractionIndex.ccLn100"  after excep.
//   It's said that after Geant4 9.4, in the file globals.hh, the definition of G4Exception was changed.
//
// ********************************************************************


#include "G4XrayRefractionIndex.hh"

#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"
#include <fstream>
#include <sstream>

using namespace std;

namespace {
  const G4double tolerance_check = 1e-5;
}

G4String G4XrayRefractionIndex::FindOpticalConstantsFile(const G4String & materialName) {

  // The logic here is as follows.
  //
  // - If G4NKDATA is not defined, look for refraction index data files in the current directory.
  // - If G4NKDATA is defined, look for refraction index data files in the directory it points to.
  // - The data files have names like <material name>.dat.
  // - If the material has been selected through the Nist manager, the name will be prefixed with G4_.
  // - Material names can be capitalized (this is the case if the names returned by the Nist manager).
  // - Try first <material name>.dat; the check if prefixed with G4_, remove prerfix and try again, finally convert to lower case and try with that.
  // - Fail if there is no file.

  
  G4String baseDir = ".";
  const char * envDir = ::getenv("G4NKDATA");
  if(!envDir)
    G4cerr << "G4XrayRefractionIndex: the environment variable G4NKDATA is not defined. Will look for refraction index data in the current directory." << G4endl;
  else
    baseDir.assign(envDir);

  G4String mn = materialName;
  G4String name = baseDir + "/" + mn + ".dat";
  std::ifstream is;
  is.open(name.c_str());
  if(is)
    return name;

  if(mn.substr(0,3) == "G4_")
    mn = mn.substr(3);

  name = baseDir + "/" + mn + ".dat";

  is.open(name.c_str());
  if(is)
    return name;

  mn.toLower();

  name = baseDir + "/" + mn + ".dat";
  is.open(name.c_str());
  if(is)
    return name;

  G4String msg("Cannot find refraction index file for material ");
  msg += materialName;
  G4Exception("G4XrayRefractionIndex::FindOpticalConstantsFile", "0", FatalException, msg.c_str());      
  // The return is never executed -- it is there only for the compiler
  return "";
}


G4XrayRefractionIndex::G4XrayRefractionIndex(const G4String& fileName)
  : m_file(fileName), m_e_min(0), m_e_max(0) {

  //  G4cout << "G4XrayRefractionIndex::G4XrayRefractionIndex(const G4String& fileName)" << G4endl;

  std::ifstream is(fileName.c_str());
  if(!is){
    const G4String excep("G4XrayRefractionIndex: opening of file " + fileName + " failed. Check that the file exists and that you can read it.");
    G4Exception(excep,"0",FatalException,"G4XrayRefractionIndex.ccLn95");
  }
  read(is);
  if(!is.eof()){
    const G4String excep("G4XrayRefractionIndex: reading of file " + fileName + " failed. Check that the file exists and that you can read it.");
    G4Exception(excep,"0",FatalException,"G4XrayRefractionIndex.ccLn100"); // Here is the error that in globals.hh of Geant4 9.4, the G4Exception code changed.
  }

}

G4XrayRefractionIndex::~G4XrayRefractionIndex() {
  //  G4cout << "G4XrayRefractionIndex::~G4XrayRefractionIndex()" << G4endl;
}


void G4XrayRefractionIndex::read(std::istream& is) {

  bool first = true;
  G4double previous_E =0;
  size_t lineno = 0;
  string line;
  bool cxro_file = false;

  while(getline(is, line)){
    ++lineno;
    if(line[0] == '#') // comment line
      continue;
    if(line.size() == 0 or line.find_first_not_of(' ') == string::npos) // blank line
      continue;
    istringstream lis(line);

    // refraction index files downloaded from CXRO start with two text
    // lines, like the following:
    //
    // Si3N4 Density=3.44
    // Energy(eV), Delta, Beta
    //
    // 
    if(lineno == 1 and line.find("Density") != string::npos){
      G4cout << "G4XrayRefractionIndex: " << m_file << " appears to be a CXRO file with the following header: " << line << G4endl;
      cxro_file = true;
      continue;
    }
    if(lineno == 2 and cxro_file)
      continue;

    G4double E, k, d;
    lis >> E >> k >> d;
  //  lis >> E >> d >> k; //This is the order of Henke website
    E *= eV; // REMARK: units in are eV
    std::ostringstream ln;
    ln << lineno;
    if(!lis){ 
      G4String excep("G4XrayRefractionIndex: IO error while reading " + m_file + " at line " + ln.str() + ". Not enough data items? (Expected three.) The line contains: " + line);
      G4Exception(excep,"0",FatalException,"G4XrayRefractionIndex.ccLn148");
    }
    if (E <= 0) {
      G4String excep("G4XrayRefractionIndex: negative energy in  " + m_file + " at line " + ln.str());
      G4Exception(excep,"0",FatalException,"G4XrayRefractionIndex.ccLn152");     
    }
    m_index[E] = G4complex(1-d,-k); 
    if(first){
      first = false;
      previous_E = E;
      m_e_min = m_e_max = E;
    } else {
      if(E <= previous_E){
	G4String excep("G4XrayRefractionIndex: energies do not increase monotonically in " + m_file + " at line " + ln.str());
	G4Exception(excep,"0",FatalException,"G4XrayRefractionIndex.ccLn162");     
      }
      if(E < m_e_min)
	m_e_min = E;
      else if(E > m_e_max)
	m_e_max = E;
    }
  }

  if (m_index.size() == 0){
    G4String excep("G4XrayRefractionIndex: no data found in " + m_file);
    G4Exception(excep,"0",FatalException,"G4XrayRefractionIndex.ccLn173");         
  }

}

pair<G4double, G4double> G4XrayRefractionIndex::GetEnergyRange() const {
  return make_pair(m_e_min, m_e_max);
}


G4complex G4XrayRefractionIndex::GetRefractionIndex(G4double E) const {

  if(E < m_e_min or E > m_e_max){
    ostringstream os;
    os << " energy " << E << " is not in [" << m_e_min << "," << m_e_max << "]";
    G4String excep(" G4XrayRefractionIndex: " + os.str());
    G4Exception(excep,"0",FatalException,"G4XrayRefractionIndex.ccLn189");         
  }
    
  G4complex index;

  map_t::const_iterator iter = m_index.lower_bound(E);
  
  // lower_bound returns an iterator pointing to the first element
  // with an energy >= E. So iter==m_index.begin() means that
  // E==m_e_min (because it cannot be E<m_e_min, see check above). The
  // case iter=m_index.end() may mean E==m_max_e, or E is in between
  // the last two points in the list. Algorithmically this case can be
  // treated as any other case.

  if(iter == m_index.begin()){
    // E == m_e_min
    index = (*iter).second;
  } else {
  
    const G4double next_E = (*iter).first;
    const G4complex next_index = (*iter).second;
    --iter;
    const G4double previous_E = (*iter).first;
    const G4complex previous_index = (*iter).second;

    // REMARK: check on dE
    const G4double dE = next_E - previous_E;
    const G4complex dindex = next_index - previous_index;
    const G4complex slope = dindex/dE;

    index = previous_index + slope*(E - previous_E);

  }
  return index;
}
