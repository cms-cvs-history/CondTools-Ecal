#include "CondTools/Ecal/interface/EcalTPGFineGrainTowerEEHandler.h"
#include "OnlineDB/EcalCondDB/interface/EcalLogicID.h"
#include "OnlineDB/EcalCondDB/interface/RunTPGConfigDat.h"
#include "OnlineDB/EcalCondDB/interface/FEConfigMainInfo.h"
#include "OnlineDB/EcalCondDB/interface/FEConfigFgrInfo.h"
#include "FWCore/ParameterSet/interface/ParameterSetfwd.h"
#include "Geometry/EcalMapping/interface/EcalElectronicsMapping.h"
#include "Geometry/EcalMapping/interface/EcalMappingRcd.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"

#include<iostream>

popcon::EcalTPGFineGrainTowerEEHandler::EcalTPGFineGrainTowerEEHandler(const edm::ParameterSet & ps)
  :    m_name(ps.getUntrackedParameter<std::string>("name","EcalTPGFineGrainTowerEEHandler")) {

        edm::LogInfo("EcalTPGFineGrainTowerEEHandler") << "EcalTPGFineGrainTowerEE Source handler constructor.";
        m_firstRun=(unsigned long)atoi( ps.getParameter<std::string>("firstRun").c_str());
        m_lastRun=(unsigned long)atoi( ps.getParameter<std::string>("lastRun").c_str());
        m_sid= ps.getParameter<std::string>("OnlineDBSID");
        m_user= ps.getParameter<std::string>("OnlineDBUser");
        m_pass= ps.getParameter<std::string>("OnlineDBPassword");
        m_locationsource= ps.getParameter<std::string>("LocationSource");
        m_location=ps.getParameter<std::string>("Location");
        m_gentag=ps.getParameter<std::string>("GenTag");

        edm::LogInfo("EcalTPGFineGrainTowerEEHandler")<< m_sid<<"/"<<m_user<<"/"<<m_pass<<"/"<<m_location<<"/"<<m_gentag;

}

popcon::EcalTPGFineGrainTowerEEHandler::~EcalTPGFineGrainTowerEEHandler()
{
}

void popcon::EcalTPGFineGrainTowerEEHandler::getNewObjects()
{

	edm::LogInfo("EcalTPGFineGrainTowerEEHandler") << "Started GetNewObjects!!!";

	//check whats already inside of database
	if (tagInfo().size){
  	//check whats already inside of database
    	std::cout << "got offlineInfo = " << std::endl;
	std::cout << "tag name = " << tagInfo().name << std::endl;
	std::cout << "size = " << tagInfo().size <<  std::endl;
    	} else {
    	std::cout << " First object for this tag " << std::endl;
    	}

	unsigned int max_since=0;
	max_since=(int)tagInfo().lastInterval.first;
	edm::LogInfo("EcalTPGFineGrainTowerEEHandler") << "max_since : "  << max_since;
	Ref ped_db = lastPayload();
	
	edm::LogInfo("EcalTPGFineGrainTowerEEHandler") << "retrieved last payload ";

	// here we retrieve all the runs after the last from online DB 
	edm::LogInfo("EcalTPGFineGrainTowerEEHandler") << "Retrieving run list from ONLINE DB ... ";

	edm::LogInfo("EcalTPGFineGrainTowerEEHandler") << "Making connection...";
	econn = new EcalCondDBInterface( m_sid, m_user, m_pass );
	edm::LogInfo("EcalTPGFineGrainTowerEEHandler") << "Done.";
	
	if (!econn)
	  {
	    cout << " connection parameters " <<m_sid <<"/"<<m_user<<"/"<<m_pass<<endl;
	    //	    cerr << e.what() << endl;
	    throw cms::Exception("OMDS not available");
	  } 

	
	LocationDef my_locdef;
	my_locdef.setLocation(m_location); 

	RunTypeDef my_rundef;
	my_rundef.setRunType("PEDESTAL"); 

	RunTag  my_runtag;
	my_runtag.setLocationDef( my_locdef );
	my_runtag.setRunTypeDef(  my_rundef );
	//    my_runtag.setGeneralTag( generalTag );  <- this is normally "GLOBAL"
	my_runtag.setGeneralTag(m_gentag); 

	int min_run=0;
	if(m_firstRun<max_since) {
	  min_run=  (int)max_since+1; // we have to add 1 to the last transferred one
	} else {
	  min_run=(int)m_firstRun;
	}

	int max_run=(int)m_lastRun;
	edm::LogInfo("EcalTPGFineGrainTowerEEHandler") << "min_run= " << min_run << " max_run= " << max_run;

	RunList my_list;
        //my_list=econn->fetchRunListByLocation(my_runtag,min_run,max_run,my_locdef);
	my_list=econn->fetchRunList(my_runtag);
	printf ("after fetchRunList\n");fflush(stdout);
      
	std::vector<RunIOV> run_vec=  my_list.getRuns();
	int mon_runs=run_vec.size();
	edm::LogInfo("EcalTPGFineGrainTowerEEHandler") <<"number of Mon runs is : "<< mon_runs;

	unsigned long irun;
	if(mon_runs>0){

	  for(int kr=0; kr<mon_runs; kr++){

	    irun=(unsigned long) run_vec[kr].getRunNumber();
	    edm::LogInfo("EcalTPGFineGrainTowerEEHandler") << "Here is the run number: "<< run_vec[kr].getRunNumber();
	  

	    edm::LogInfo("EcalTPGFineGrainTowerEEHandler") << "Fetching run by tag";

	    // retrieve the data
	    std::map<EcalLogicID, RunTPGConfigDat> dataset;
	    econn->fetchDataSet(&dataset, &run_vec[kr]);
	    std::string the_config_tag="";
	    std::map< EcalLogicID,  RunTPGConfigDat>::const_iterator it;
	    FEConfigMainInfo fe_main_info;
	    int nr=0;
	    for( it=dataset.begin(); it!=dataset.end(); it++ )
	      {
		++nr;
		EcalLogicID ecalid  = it->first;
		RunTPGConfigDat  dat = it->second;
		std::string the_config_tag=dat.getConfigTag();
		edm::LogInfo("EcalTPGFineGrainTowerEEHandler") <<"config_tag "<< the_config_tag;
		fe_main_info.setConfigTag(the_config_tag);
		econn-> fetchConfigSet(&fe_main_info);

	      }
	    edm::LogInfo("EcalTPGFineGrainTowerEEHandler") << "got " << nr << "objects in dataset.";


	    // now get TPGFineGrainTowerEE
	    int fgrId=fe_main_info.getFgrId();
	    FEConfigFgrInfo fe_fgr_info;
	    fe_fgr_info.setId(fgrId);
	    econn-> fetchConfigSet(&fe_fgr_info);
	    std::map<EcalLogicID, FEConfigFgrDat> dataset_TpgFineGrainEE;
	    econn->fetchDataSet(&dataset_TpgFineGrainEE, &fe_fgr_info);
	    EcalTPGFineGrainTowerEE *fgrMap = new EcalTPGFineGrainTowerEE;
	    
	    typedef std::map<EcalLogicID, FEConfigFgrDat>::const_iterator CIfefgr;
	    EcalLogicID ecid_xt;
	    FEConfigFgrDat  rd_fgr;
	    int itowers=0;
	    
	    for (CIfefgr p = dataset_TpgFineGrainEE.begin(); p != dataset_TpgFineGrainEE.end(); p++) {
	      ecid_xt = p->first;
	      rd_fgr  = p->second;
	      
	      std::string ecid_name=ecid_xt.getName();
	      
	      if (ecid_name=="EE_trigger_tower") {
	        // SM number
	        int smid=ecid_xt.getID1();
	        // TT number
	        int towerid=ecid_xt.getID2();
                
		char identTT[10];
		sprintf(identTT,"%d%d", smid, towerid);
	        
		std::string S="";
		S.insert(0,identTT);
		
		unsigned int towerEEId = 0;
		towerEEId = atoi(S.c_str());
				
                fgrMap->setValue(towerEEId, rd_fgr.getFgrGroupId());
	        ++itowers;
	      }
	    }
	    	
	    edm::LogInfo("EcalTPGFineGrainTowerEEHandler") << "found " << itowers << "towers.";

	    Time_t snc= (Time_t) irun ;
	      	      
	    m_to_transfer.push_back(std::make_pair((EcalTPGFineGrainTowerEE *)fgrMap,snc));

	  }
	}
	  
        delete econn;
	edm::LogInfo("EcalTPGFineGrainTowerEEHandler") << "Ecal - > end of getNewObjects -----------";
}