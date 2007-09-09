#include "FWCore/PluginManager/interface/ModuleDef.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/SourceFactory.h"
#include "CondTools/Ecal/interface/EcalDBCopy.h"
#include "CondTools/Ecal/interface/EcalTestDevDB.h"
#include "CondTools/Ecal/interface/EcalPedestalTransfer.h"
#include "CondTools/Ecal/interface/EcalTrendChecker.h"
#include "CondTools/Ecal/interface/EcalGetLaserData.h"


DEFINE_SEAL_MODULE();
DEFINE_ANOTHER_FWK_MODULE(EcalDBCopy);
DEFINE_ANOTHER_FWK_MODULE(EcalTestDevDB);
DEFINE_ANOTHER_FWK_MODULE(EcalPedestalTransfer);
DEFINE_ANOTHER_FWK_MODULE(EcalTrendChecker);
DEFINE_ANOTHER_FWK_MODULE(EcalGetLaserData);
