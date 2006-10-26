#include <memory>

#include <FWCore/Framework/interface/Frameworkfwd.h>
#include <FWCore/Framework/interface/EDAnalyzer.h>
#include <FWCore/Framework/interface/Event.h>
#include <FWCore/Framework/interface/EventSetup.h>
#include <FWCore/Framework/interface/ESHandle.h>
#include <FWCore/Framework/interface/MakerMacros.h>
#include <FWCore/ParameterSet/interface/ParameterSet.h>

#include <DetectorDescription/Core/interface/DDCompactView.h>
#include <DetectorDescription/Core/interface/DDValue.h>
#include <DetectorDescription/Core/interface/DDsvalues.h>
#include <DetectorDescription/Core/interface/DDExpandedView.h>
#include <DetectorDescription/Core/interface/DDFilteredView.h>

#include <Geometry/Records/interface/IdealGeometryRecord.h>

#include <string>
#include <cmath>
#include <iomanip>
#include <vector>
#include <map>
#include <sstream>

class GeometryAnalyzer : public edm::EDAnalyzer {

   public:
 
     explicit GeometryAnalyzer( const edm::ParameterSet& );
      ~GeometryAnalyzer();

      virtual void analyze( const edm::Event&, const edm::EventSetup& );
 
      const std::string& myName() { return myName_;}

   private: 

      const int dashedLineWidth_;
      const std::string dashedLine_;
      const std::string myName_;
  std::string label_;
};

GeometryAnalyzer::GeometryAnalyzer( const edm::ParameterSet& iConfig )
 : dashedLineWidth_(194), dashedLine_( std::string(dashedLineWidth_, '-') ), 
  myName_( "GeometryAnalyzer" )
{
}


GeometryAnalyzer::~GeometryAnalyzer()
{
}

void
 GeometryAnalyzer::analyze( const edm::Event& iEvent, const edm::EventSetup& iSetup )
{
   const double dPi = 3.14159265358;
   const double radToDeg = 180. / dPi; //@@ Where to get pi from?

   std::cout << myName() << ": Analyzer..." << std::endl;
   std::cout << "start " << dashedLine_ << std::endl;

   edm::ESHandle<DDCompactView> pDD;
   iSetup.get<IdealGeometryRecord>().get(label_, pDD );
   //std::cout <<" got the geometry"<<std::endl;
   try {
//       DDExpandedView ex(*pDD);
//       std::cout << " Top node is "<< ex.logicalPart().name() << std::endl;
//       std::map<std::string, int> myGeoHists;
//       std::string tstring;
//       do {
// 	//	std::cout << ex.geoHistory() << std::endl;
// 	std::stringstream tstream;
// 	tstream << ex.geoHistory();
// 	tstream >> tstring;
// 	if ( myGeoHists.find(tstring) != myGeoHists.end() ) {
// 	  myGeoHists[tstring] = 1;
// 	} else {
// 	  myGeoHists[tstring] = myGeoHists[tstring] + 1;
// 	}
//       } while ( ex.next() );

    std::string attribute = "MuStructure"; 
    std::string value     = "MuonBarrelDT";
    DDValue val(attribute, value, 0.0);

    // Asking only for the Muon DTs
    DDSpecificsFilter filter;
    filter.setCriteria(val,  // name & value of a variable 
		       DDSpecificsFilter::matches,
		       DDSpecificsFilter::AND, 
		       true, // compare strings otherwise doubles
		       true  // use merged-specifics or simple-specifics
		       );
    DDFilteredView fv(*pDD);
    fv.addFilter(filter);

      std::cout << " Top node is "<< fv.logicalPart().name() << std::endl;
      std::map<std::string, int> myGeoHists;
      std::string tstring;

  bool doChamber = fv.firstChild();

  // Loop on chambers
  int ChamCounter=0;
  while (doChamber){
    ChamCounter++;
    DDValue val("Type");
    const DDsvalues_type params(fv.mergedSpecifics());
    std::string type;
    if (DDfetch(&params,val)) type = val.strings()[0];
    // FIXME
    val=DDValue("FEPos");
    std::string FEPos;
    if (DDfetch(&params,val)) FEPos = val.strings()[0];
    std::cout << "would have...    DTChamber* chamber = buildChamber(fv,type);" << std::endl;

    // Loop on SLs
    bool doSL = fv.firstChild();
    int SLCounter=0;
    while (doSL) {
      SLCounter++;
      std::cout << " would have ...   DTSuperLayer* sl = buildSuperLayer(fv, chamber, type);" << std::endl;
      //theGeometry->add(sl);
      bool doL = fv.firstChild();
      int LCounter=0;
      // Loop on SLs
      while (doL) {
        LCounter++;
	std::cout << "would have ...        DTLayer* layer = buildLayer(fv, sl, type);" << std::endl;
        //theGeometry->add(layer);
        // pass ownership to superlayer
        //sl->add(layer);

	bool doWire = fv.firstChild();
	int WCounter=0;
	int firstWire=fv.copyno();
	//cout << " first wire: " << fv.copyno();
	while (doWire) {
	  WCounter++;
	  doWire = fv.nextSibling(); // next wire
	}
	std::cout << "num wires = " << WCounter << std::endl;
	fv.parent(); // <---- new

        fv.parent();
        doL = fv.nextSibling(); // go to next layer
      } // layers

      // pass ownership to chamber
      //chamber->add(sl);

      fv.parent();
      doSL = fv.nextSibling(); // go to next SL
    } // sls
    //    theGeometry->add(chamber);

    fv.parent();
    doChamber = fv.nextSibling(); // go to next chamber
  } // chambers
  std::cout << "Built " << ChamCounter << " drift tube chambers." << std::endl;

//       do {
// 	//	std::cout << ex.geoHistory() << std::endl;
// 	std::stringstream tstream;
// 	tstream << fv.geoHistory();
// 	tstream >> tstring;
// 	if ( myGeoHists.find(tstring) != myGeoHists.end() ) {
// 	  myGeoHists[tstring] = 1;
// 	} else {
// 	  myGeoHists[tstring] = myGeoHists[tstring] + 1;
// 	}
//       } while ( fv.next() );

      for ( std::map<std::string, int>::const_iterator it = myGeoHists.begin(); 
	    it != myGeoHists.end(); ++it ) {
	std::cout << it->first << " = " << it->second << std::endl;
      }

//       std::cout << " Top node is a "<< ex.logicalPart() << std::endl;
//       std::string attribute = "MuStructure"; 
//       std::string value     = "MuonBarrelDT";
//       DDValue val(attribute, value, 0.0);
      
//       // Asking only for the Muon DTs
//       DDSpecificsFilter filter;
//       filter.setCriteria(val,  // name & value of a variable 
// 			 DDSpecificsFilter::matches,
// 			 DDSpecificsFilter::AND, 
// 			 true, // compare strings otherwise doubles
// 			 true  // use merged-specifics or simple-specifics
// 			 );
//       DDFilteredView fv(*pDD);
//       fv.addFilter(filter);
//       std::cout << " Top node is " << fv.logicalPart().name() << std::endl;
//       bool next = fv.firstChild();
//       do {
// 	std::cout << fv.geoHistory() << std::endl;
//       } while ( next = fv.next() );
   }catch(const DDLogicalPart& iException){
      throw cms::Exception("Geometry")
      <<"A DDLogicalPart was thrown \""<<iException<<"\"";
   }

   std::cout << dashedLine_ << " end" << std::endl;
}

//define this as a plug-in
DEFINE_FWK_MODULE(GeometryAnalyzer);
