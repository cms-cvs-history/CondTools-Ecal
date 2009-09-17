#ifndef ECAL_TPG_FINEGRAINSTRIPEE_HANDLER_H
#define ECAL_TPG_FINEGRAINSTRIPEE_HANDLER_H

#include <vector>
#include <typeinfo>
#include <string>
#include <map>
#include <iostream>
#include <time.h>

#include "CondCore/PopCon/interface/PopConSourceHandler.h"
#include "FWCore/ParameterSet/interface/ParameterSetfwd.h"


#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CondCore/DBOutputService/interface/PoolDBOutputService.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "DataFormats/Common/interface/Handle.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/EventSetupRecordKey.h"



#include "CondFormats/EcalObjects/interface/EcalTPGFineGrainStripEE.h"
#include "CondFormats/DataRecord/interface/EcalTPGFineGrainStripEERcd.h"

#include "OnlineDB/EcalCondDB/interface/all_monitoring_types.h"
#include "OnlineDB/Oracle/interface/Oracle.h"
#include "OnlineDB/EcalCondDB/interface/EcalCondDBInterface.h"

#include "DataFormats/EcalDetId/interface/EEDetId.h"
#include "DataFormats/EcalDetId/interface/EBDetId.h"
#include "DataFormats/Provenance/interface/Timestamp.h"

//class EcalElectronicsMapping;

using namespace std;
using namespace oracle::occi;

namespace edm {
  class ParameterSet;
  class Event;
  class EventSetup;
}

namespace popcon
{


	class EcalTPGFineGrainStripEEHandler : public popcon::PopConSourceHandler<EcalTPGFineGrainStripEE>
	{

		public:
                        EcalTPGFineGrainStripEEHandler(edm::ParameterSet const & );
			~EcalTPGFineGrainStripEEHandler();
			
			std::map<std::string, int> makeStripIdEB();
			std::map<std::string, int> makeStripIdEE();
			void getNewObjects();
			std::string id() const { return m_name;}
			EcalCondDBInterface* econn;

		private:
			unsigned long m_firstRun ;
			unsigned long m_lastRun ;
			std::map<std::string, int> correspEBId;
			std::map<std::string, int> correspEEId;
			
			std::string m_location;
			std::string m_gentag;
			std::string m_sid;
			std::string m_user;
			std::string m_pass;
                        std::string m_locationsource;
                        std::string m_name;


	};
}
#endif
