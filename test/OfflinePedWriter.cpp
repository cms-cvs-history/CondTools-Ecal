// Uses Zhen's DBWriter to write POOL-ORA objects to a DB

#include <iostream>
#include <string>
#include <vector>

#include "CondCore/DBCommon/interface/DBWriter.h"
#include "CondCore/IOVService/interface/IOV.h"
#include "CondCore/MetaDataService/interface/MetaData.h"
#include "FWCore/Framework/interface/IOVSyncValue.h"
#include "SealKernel/Service.h"
#include "POOLCore/POOLContext.h"
#include "SealKernel/Context.h"

#include "DataFormats/EcalDetId/interface/EBDetId.h"
#include "CondFormats/EcalObjects/interface/EcalPedestals.h"
#include "CondFormats/EcalObjects/interface/EcalWeightRecAlgoWeights.h"
#include "CondFormats/EcalObjects/interface/EcalWeight.h"

using namespace std;

class WriterApp {
public:
  WriterApp(string conStr)
  {
    pool::POOLContext::loadComponent( "SEAL/Services/MessageService" );
    pool::POOLContext::setMessageVerbosityLevel( seal::Msg::Nil );
    
    writer = new cond::DBWriter(conStr);
    metadataSvc = new cond::MetaData(conStr);
  }

  ~WriterApp()
  {
    delete writer;
    delete metadataSvc;
  }

  void buildDetIdVector()
  {
    EBDetId detid;
    for (int ieta = -EBDetId::MAX_IETA; ieta <= EBDetId::MAX_IETA; ieta++) {
      if (ieta == 0) { continue; }
      for (int iphi = EBDetId::MIN_IPHI; iphi <= EBDetId:: MAX_IPHI; iphi++) {
	detid = EBDetId(ieta, iphi);
	detVec.push_back(detid);
      }
    }
  }

  void writeEcalPedestals(int num)
  {
    pool::POOLContext::setMessageVerbosityLevel( seal::Msg::Nil );
    cout << "Building DetIDs..." << flush;
    this->buildDetIdVector();
    cout << "Done." << endl;
    
    cond::IOV* pedIOV = new cond::IOV; 
    string pedTok;
    string pedIOVTok;

    EcalPedestals* ped = new EcalPedestals();
    EcalPedestals::Item item;

    writer->startTransaction();
    for (int run=0; run<num; run++) {
      cout << "Run " << run << ": " << flush;
      cout << "Generate pedestals..." << flush;
      for (vector<EBDetId>::const_iterator detid_p = detVec.begin();
	   detid_p != detVec.end();
	   ++detid_p) 
	{
	  item.mean_x1  = rand();
	  item.rms_x1   = rand();
	  item.mean_x6  = rand();
	  item.rms_x6   = rand();
	  item.mean_x12 = rand();
	  item.rms_x12  = rand();

	  ped->m_pedestals.insert(std::make_pair(detid_p->rawId(),item));
	}
      
      cout << "Write pedestals..." << flush;
      pedTok = writer->write<EcalPedestals>(ped, "EcalPedestals");

      cout << "Write IOV..." << flush;
      pedIOV->iov.insert(std::make_pair(run, pedTok));
      pedIOVTok = writer->write<cond::IOV>(pedIOV, "IOV");

      cout << "Commit..." << flush;
      writer->commitTransaction();

      cout << "Add MetaData... " << flush;
      metadataSvc->addMapping("EcalPedestals_Nov_test", pedIOVTok);
      cout << "Done." << endl;
    }
  }

private:
  cond::DBWriter* writer;
  cond::MetaData* metadataSvc;
  vector<EBDetId> detVec;

};

int main(int argc, char* argv[])
{
  if (argc != 3) {
    cout << "Usage:" << endl;
    cout << "  " << argv[0] << " <contact string> <num>" << endl;
    exit(-1);
  }
  
  string conStr = argv[1];
  int num = atoi(argv[2]);

  try {
    WriterApp app(conStr);
    app.writeEcalPedestals(num);
  } catch (seal::Exception& e) {
    cout << "seal::Exception:  " << e.what() << endl;
  } catch (exception &e) {
    cout << "std::exception:  " << e.what() << endl;
  } catch (...) {
    cout << "Unknown error caught" << endl;
  }

  return 0;
}
