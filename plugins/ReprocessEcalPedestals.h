#ifndef REPROCESSECALPEDESTALS_H
#define REPROCESSECALPEDESTALS_H

#include "FWCore/Framework/interface/EDAnalyzer.h"

#include <string>
#include <map>
#include <set>

#include "CondFormats/EcalObjects/interface/EcalPedestals.h"
#include "DataFormats/EcalDetId/interface/EBDetId.h"

namespace edm {
  class ParameterSet;
  class Event;
  class EventSetup;
}

using namespace std;

class  ReprocessEcalPedestals : public edm::EDAnalyzer {
 public:
  explicit  ReprocessEcalPedestals(const edm::ParameterSet& iConfig );
  ~ReprocessEcalPedestals();

  virtual void analyze( const edm::Event& evt, const edm::EventSetup& evtSetup);
  virtual void endJob();

 private:
  void loadCacheFromFile(std::string file);
  void dumpCacheToFile(std::string file);

  unsigned long long m_cacheID;
  bool m_isFirstRun;
  std::set<int> m_smSet;
  EcalPedestals m_pedCache;
  std::string m_startFile;
  std::string m_endFile;
  bool m_appendMode;
  std::string m_recordToProcess;
};
#endif
