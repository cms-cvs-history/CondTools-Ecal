// Uses Zhen's DBWriter to write POOL-ORA objects to a DB

#include "CondCore/DBCommon/interface/DBWriter.h"
#include "CondCore/DBCommon/interface/DBSession.h"
#include "CondCore/DBCommon/interface/Exception.h"
#include "CondCore/DBCommon/interface/ServiceLoader.h"
#include "CondCore/DBCommon/interface/ConnectMode.h"
#include "CondCore/DBCommon/interface/MessageLevel.h"
#include "CondFormats/Calibration/interface/Pedestals.h"
#include "CondCore/IOVService/interface/IOV.h"
#include "CondCore/MetaDataService/interface/MetaData.h"

#include "DataFormats/EcalDetId/interface/EBDetId.h"
#include "CondFormats/EcalObjects/interface/EcalPedestals.h"
#include "CondFormats/EcalObjects/interface/EcalWeightRecAlgoWeights.h"
#include "CondFormats/EcalObjects/interface/EcalWeight.h"

#include <iostream>
#include <string>
#include <vector>


using namespace std;

class WriterApp {
public:
  WriterApp(string conStr)
  {
    loader = new cond::ServiceLoader;

    session = new cond::DBSession(conStr);
    session->connect(cond::ReadWriteCreate);

    metadataSvc = new cond::MetaData(conStr, *loader);
    metadataSvc->connect();

    pedWriter = new cond::DBWriter(*session, "EcalPedestals");
    iovWriter = new cond::DBWriter(*session, "IOV");
  }

  ~WriterApp()
  {
    delete pedWriter;
    delete iovWriter;
    session->disconnect();
    delete session;
    metadataSvc->disconnect();
    delete metadataSvc;
    delete loader;
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

  void writeEcalPedestals(int num, string tag)
  {
    cout << "Building DetIDs..." << flush;
    this->buildDetIdVector();
    cout << "Done." << endl;
    
    string pedTok;
    string pedIOVTok;

    EcalPedestals* ped;
    EcalPedestals::Item item;
    cond::IOV* pedIOV = new cond::IOV; 
    
    for (int run=1; run<=num; run++) {
      cout << "Run " << run << ": " << flush;
      cout << "Generate pedestals..." << flush;

      ped = new EcalPedestals();
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

      session->startUpdateTransaction();
      cout << "Write pedestals..." << flush;
      pedTok = pedWriter->markWrite<EcalPedestals>(ped);
      cout << "Commit..." << flush;
      session->commit();

      cout << "Insert IOV..." << flush;
      pedIOV->iov.insert(std::make_pair(run, pedTok));

      cout << "Done." << endl;
    }
    session->startUpdateTransaction();
    cout << "Write IOV..." << flush;
    pedIOVTok = iovWriter->markWrite<cond::IOV>(pedIOV);
    cout << "Commit..." << flush;
    session->commit();
    cout << "Add MetaData... " << flush;
    metadataSvc->addMapping(tag, pedIOVTok);
    cout << "Done." << endl;
    cout << endl;
  }

private:
  cond::ServiceLoader* loader;
  cond::DBSession* session;
  cond::DBWriter* pedWriter;
  cond::DBWriter* iovWriter;
  cond::MetaData* metadataSvc;
  vector<EBDetId> detVec;

};

int main(int argc, char* argv[])
{
  if (argc != 4) {
    cout << "Usage:" << endl;
    cout << "  " << argv[0] << " <contact string> <num> <tag>" << endl;
    exit(-1);
  }
  string conStr = argv[1];
  int num = atoi(argv[2]);
  string tag = argv[3];
  try {
    WriterApp app(conStr);
    app.writeEcalPedestals(num, tag);
  } catch (seal::Exception& e) {
    cout << "seal::Exception:  " << e.what() << endl;
  } catch (exception &e) {
    cout << "std::exception:  " << e.what() << endl;
  } catch (...) {
    cout << "Unknown error caught" << endl;
  }

  return 0;
}
