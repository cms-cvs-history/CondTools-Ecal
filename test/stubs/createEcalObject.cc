#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Framework/interface/Event.h"
#include "CondCore/DBOutputService/interface/PoolDBOutputService.h"
#include "createEcalObject.h"


createEcalObject::createEcalObject(const edm::ParameterSet& iConfig)
{
  std::cout<<"createEcalObject::createEcalObject"<<endl;



  prog_name_ = "createEcalObject";
  
  inpFileName_ = iConfig.getParameter<std::string>("inpFileName");
  objectName_ = iConfig.getParameter<std::string>("calibType");
  sm_constr_=0;
  firstrun_=0;
  lastrun_=0; 

  cout << "Done." << endl;

}

createEcalObject::~createEcalObject()
{
  std::cout<<"createEcalObject::createEcalObject"<<std::endl;

}

//-------------------------------------------------------------
//-------------------------------------------------------------
//inline std::string to_string( const char & value)
inline std::string to_string( char value[])
{
    std::ostringstream streamOut;

    streamOut << value;

    return streamOut.str();
}




//-------------------------------------------------------------
void* createEcalObject::readSMRunFile() {
//-------------------------------------------------------------

  int sm_num[100]={0};
  int start_run[100]={0};
  int end_run[100]={0};
  
  FILE *inpFile; // input file
  inpFile = fopen("/afs/cern.ch/cms/ECAL/testbeam/pedestal/2006/sm_run.txt","r");
  if(!inpFile) {
    cout<<"*** Can not open file: /afs/cern.ch/cms/ECAL/testbeam/pedestal/2006/sm_run.txt"<<endl<<endl;
  }

  char line[256];
  
  
  int ii = 0;
  cout << "reading file ...  " << endl;
  
  while(fgets(line,255,inpFile)) {
    sscanf(line, "%d %d %d",  &sm_num[ii], &start_run[ii],&end_run[ii] );
     cout << "sm="<<sm_num[ii]<<" start run " << start_run[ii] << " end run"  << end_run[ii] <<endl;
    
    ii++ ;
  }
  cout << ii <<endl;	
   
  fclose(inpFile);           // close inp. file
  
  for (int ism=0; ism<ii; ism++){
    if(sm_constr_ ==sm_num[ism]) {
      firstrun_=start_run[ism];
      lastrun_=end_run[ism];
      cout << "sm required is ="<<sm_num[ism]<<" start run " << start_run[ism] << " end run"  << end_run[ism] <<endl;	
    } 
  }
  
  if(firstrun_ ==0 && lastrun_==0) {
    cout << "ERROR FROM createEcalObject::readSMRunFile() no run number found in file for SM="<<sm_constr_ <<endl;	
  }
  
}

//------------------------------------------------------------
void* createEcalObject::writeToLogFile(string a, string b) {
//-------------------------------------------------------------
  
    FILE *outFile; // output log file for appending 
    outFile = fopen("/afs/cern.ch/cms/ECAL/testbeam/pedestal/2006/LOGFILE/offdb.log","a");  
    if(!outFile) {
      cout<<"*** Can not open file: /afs/cern.ch/cms/ECAL/testbeam/pedestal/2006/LOGFILE/offdb.log"<<endl<<endl;
    }
    fprintf(outFile, "%s %s \n", a,b );

    fclose(outFile);           // close out file

}
//------------------------------------------------------------
void* createEcalObject::writeToLogFileResults(char* arg) {
//-------------------------------------------------------------
  
    FILE *outFile; // output log file for appending 
    outFile = fopen("/afs/cern.ch/cms/ECAL/testbeam/pedestal/2006/LOGFILE/offdb.log","a");  
    if(!outFile) {
      cout<<"*** Can not open file: /afs/cern.ch/cms/ECAL/testbeam/pedestal/2006/LOGFILE/offdb.log"<<endl<<endl;
    }
    fprintf(outFile, "%s \n", arg );

    fclose(outFile);           // close out file

}


//-------------------------------------------------------------
EcalWeightXtalGroups*
createEcalObject::readEcalWeightXtalGroupsFromFile() {
//-------------------------------------------------------------

// Zabi code to be written here 

  EcalWeightXtalGroups* xtalGroups = new EcalWeightXtalGroups();
  for(int ieta=-EBDetId::MAX_IETA; ieta<=EBDetId::MAX_IETA; ++ieta) {
    if(ieta==0) continue;
    for(int iphi=EBDetId::MIN_IPHI; iphi<=EBDetId:: MAX_IPHI; ++iphi) {
      EBDetId ebid(ieta,iphi);
      xtalGroups->setValue(ebid.rawId(), EcalXtalGroupId(ieta) ); // define rings in eta
    }
  }
  return xtalGroups;
}

//-------------------------------------------------------------
EcalTBWeights*
createEcalObject::readEcalTBWeightsFromFile() {
//-------------------------------------------------------------

// Zabi code to be written here 

  EcalTBWeights* tbwgt = new EcalTBWeights();

  // create weights for each distinct group ID
  int nMaxTDC = 10;
  for(int igrp=-EBDetId::MAX_IETA; igrp<=EBDetId::MAX_IETA; ++igrp) {
    if(igrp==0) continue;
    for(int itdc=1; itdc<=nMaxTDC; ++itdc) {
      // generate random number
      double r = (double)std::rand()/( double(RAND_MAX)+double(1) );

      // make a new set of weights
      EcalWeightSet wgt;
      EcalWeightSet::EcalWeightMatrix& mat1 = wgt.getWeightsBeforeGainSwitch();
      EcalWeightSet::EcalWeightMatrix& mat2 = wgt.getWeightsAfterGainSwitch();

      for(size_t i=0; i<3; ++i) {
      std::vector<EcalWeight> tv1, tv2;
        for(size_t j=0; j<10; ++j) {
          double ww = igrp*itdc*r + i*10. + j;
          //std::cout << "row: " << i << " col: " << j << " -  val: " << ww  << std::endl;
          tv1.push_back( EcalWeight(ww) );
          tv2.push_back( EcalWeight(100+ww) );
        }
        mat1.push_back(tv1);
        mat2.push_back(tv2);
      }

      // fill the chi2 matrcies
      r = (double)std::rand()/( double(RAND_MAX)+double(1) );
      EcalWeightSet::EcalWeightMatrix& mat3 = wgt.getChi2WeightsBeforeGainSwitch();
      EcalWeightSet::EcalWeightMatrix& mat4 = wgt.getChi2WeightsAfterGainSwitch();
      for(size_t i=0; i<10; ++i) {
        std::vector<EcalWeight> tv1, tv2;
        for(size_t j=0; j<10; ++j) {
          double ww = igrp*itdc*r + i*10. + j;
          tv1.push_back( EcalWeight(1000+ww) );
          tv2.push_back( EcalWeight(1000+100+ww) );
        }
        mat3.push_back(tv1);
        mat4.push_back(tv2);
      }

      // put the weight in the container
      tbwgt->setValue(std::make_pair(igrp,itdc), wgt);
    }
  }
  return tbwgt;
}


//-------------------------------------------------------------
EcalADCToGeVConstant*
createEcalObject::readEcalADCToGeVConstantFromFile() {
//-------------------------------------------------------------
  
    
    FILE *inpFile; // input file
    inpFile = fopen(inpFileName_.c_str(),"r");
    if(!inpFile) {
      cout<<"*** Can not open file: "<<inpFileName_<<endl<<endl;
     
    }

    char line[256];
              
    fgets(line,255,inpFile);
    int sm_number=atoi(line);

    fgets(line,255,inpFile);
    int nevents=atoi(line); // not necessary here just for online conddb
    
    fgets(line,255,inpFile);
    string gen_tag=to_string(line);
    cout << "gen tag " << gen_tag << endl ;  // should I use this? 

    fgets(line,255,inpFile);
    string cali_method=to_string(line);
    cout << "cali method " << cali_method << endl ; // not important 

    fgets(line,255,inpFile);
    string cali_version=to_string(line);
    cout << "cali version " << cali_version << endl ; // not important 

    fgets(line,255,inpFile);
    string cali_type=to_string(line);
    cout << "cali type " << cali_type << endl ; // not important


    cout << "reading file ... I print out the value " << endl;

    fgets(line,255,inpFile);
    double adc_to_gev=0;
    sscanf(line, "%f", &adc_to_gev );
    cout <<" calib="<< adc_to_gev <<endl;
    
    fclose(inpFile);           // close inp. file


    cout << " File reading done." << endl;
    
    
    // Get channel ID 
    
    sm_constr_ =sm_number;
    readSMRunFile() ; // this to read the correct IOV from the official file 

    // Set the data
    
 
    // DB supermodule always set to 1 
    
    int sm_db=1;

    // barrel and endcaps the same 
  EcalADCToGeVConstant* agc = new EcalADCToGeVConstant(adc_to_gev,adc_to_gev );
  return agc;


}


//-------------------------------------------------------------
EcalIntercalibConstants*
createEcalObject::readEcalIntercalibConstantsFromFile() {
//-------------------------------------------------------------

  EcalIntercalibConstants* ical = new EcalIntercalibConstants();

    
    FILE *inpFile; // input file
    inpFile = fopen(inpFileName_.c_str(),"r");
    if(!inpFile) {
      cout<<"*** Can not open file: "<<inpFileName_<<endl<<endl;
     
    }

    char line[256];
              
    fgets(line,255,inpFile);
    int sm_number=atoi(line);

    fgets(line,255,inpFile);
    int nevents=atoi(line); // not necessary here just for online conddb
    
    fgets(line,255,inpFile);
    string gen_tag=to_string(line);
    cout << "gen tag " << gen_tag << endl ;  // should I use this? 

    fgets(line,255,inpFile);
    string cali_method=to_string(line);
    cout << "cali method " << cali_method << endl ; // not important 

    fgets(line,255,inpFile);
    string cali_version=to_string(line);
    cout << "cali version " << cali_version << endl ; // not important 

    fgets(line,255,inpFile);
    string cali_type=to_string(line);
    cout << "cali type " << cali_type << endl ; // not important


    int cry_num[1700]={0};
    float calib[1700]={0};
    float calib_rms[1700]={0};
    int calib_nevents[1700]={0};
    bool calib_status[1700]={0};

    int ii = 0;
    cout << "reading file ... I print out only the first 10 channels " << endl;

    while(fgets(line,255,inpFile)) {
      sscanf(line, "%d %f %f %d %d", &cry_num[ii], &calib[ii], &calib_rms[ii], &calib_nevents[ii], &calib_status[ii] );
      if(ii<10) { // print out only the first ten channels 
	cout << "cry="<<cry_num[ii]<<" calib="<<calib[ii]<<endl;
      }
      ii++ ;
    }
    

    //    inf.close();           // close inp. file
    fclose(inpFile);           // close inp. file


    cout << " I read the calibrations for "<< ii<< " crystals " << endl;
    if(ii!=1700) cout << " WARNING >> not 1700 crystals the missing channels will be set to 0" << endl;

    cout << " File reading done." << endl;
    
    
    // Get channel ID 
    
    sm_constr_ =sm_number;

    readSMRunFile() ; // this to read the correct IOV from the official file 


    // Set the data
    
 
    // DB supermodule always set to 1 
    
    int sm_db=1;

    for(int i=0; i<1700; i++){
    
    // EBDetId(int index1, int index2, int mode = ETAPHIMODE)
    // sm and crys index SMCRYSTALMODE index1 is SM index2 is crystal number a la H4
    
      EBDetId ebid(sm_db,cry_num[i],EBDetId::SMCRYSTALMODE);

      ical->setValue( ebid.rawId(), calib[i]  );
   
  } // loop over channels 

  return ical;
}


//-------------------------------------------------------------
EcalGainRatios*
createEcalObject::readEcalGainRatiosFromFile() {
//-------------------------------------------------------------

  // create gain ratios
  EcalGainRatios* gratio = new EcalGainRatios;
    
    FILE *inpFile; // input file
    inpFile = fopen(inpFileName_.c_str(),"r");
    if(!inpFile) {
      cout<<"*** Can not open file: "<<inpFileName_<<endl<<endl;
     
    }

    char line[256];
              
    fgets(line,255,inpFile);
    int sm_number=atoi(line);

    fgets(line,255,inpFile);
    int nevents=atoi(line);
    
    fgets(line,255,inpFile);
    string gen_tag=to_string(line);
    cout << "gen tag " << gen_tag << endl ;    

    fgets(line,255,inpFile);
    string cali_method=to_string(line);
    cout << "cali method " << cali_method << endl ;

    fgets(line,255,inpFile);
    string cali_version=to_string(line);
    cout << "cali version " << cali_version << endl ;


    fgets(line,255,inpFile);
    string cali_type=to_string(line);
    cout << "cali type " << cali_type << endl ;


    cout << "Writing GainRatio objects to database..." << endl;
    

    
    int cry_num[1700]={0};
    float g1_g12[1700]={0};
    float g6_g12[1700]={0};
    bool calib_status[1700]={0};
    long dummy1=0;
    int dummy2=0;


    int ii = 0;
    cout << "reading file ... I print out only the first 10 channels " << endl;

    while(fgets(line,255,inpFile)) {
      sscanf(line, "%d %d %d %f %f %d", &dummy1, &dummy2, &cry_num[ii], &g1_g12[ii], &g6_g12[ii], &calib_status[ii] );
      if(ii<10){
	cout << "cry="<<cry_num[ii]<<" gains="
	     <<g1_g12[ii]<< "and "<< g6_g12[ii]<<endl;
      }
      ii++ ;
    }
    
    
    fclose(inpFile);           // close inp. file

    cout << " I read the calibrations for "<< ii<< " crystals " << endl;
    if(ii!=1700) cout << " WARNING >> not 1700 crystals the missing channels will be set to 0" << endl;

    cout << " File reading done." << endl;
    
    // Get channel ID 
    sm_constr_ =sm_number;
    readSMRunFile() ; // this to read the correct IOV from the official file 

    cout << "going to do gratio"<< endl;
    
    // DB supermodule always set to 1 
    int sm_db=1;
    for(int i=0; i<1700; i++){
      // EBDetId(int index1, int index2, int mode = ETAPHIMODE)
      // sm and crys index SMCRYSTALMODE index1 is SM index2 is crystal number a la H4
      EBDetId ebid(sm_db,cry_num[i],EBDetId::SMCRYSTALMODE);
      // cout << "ebid.rawId()"<< ebid.rawId()<< endl;
      EcalMGPAGainRatio gr;
      gr.setGain12Over6( g6_g12[ii] );
      gr.setGain6Over1(g1_g12[ii]/g6_g12[ii]);
      gratio->setValue( ebid.rawId(), gr );
    } // loop over channels 

    cout << "done gratio"<< endl;

    return gratio;

}




//-------------------------------------------------------------
void createEcalObject::analyze( const edm::Event& evt, const edm::EventSetup& evtSetup)
//-------------------------------------------------------------
{
  std::cout<<"createEcalObject::analyze "<<std::endl;
  edm::Service<cond::service::PoolDBOutputService> mydbservice;
  if( !mydbservice.isAvailable() ){
    std::cout<<"Service is unavailable"<<std::endl;
    return;
  }
  
  // writeToLogFile(prog_name_, inpFileName_);

  
  size_t callbackToken=mydbservice->callbackToken( objectName_);
  

  
  //   unsigned int irun=evt.id().run();
  unsigned int irun=0;

  // FC I think we must change the mydbservice->currentTime() in 
  //here 	mydbservice->newValidityForNewPayload<EcalWeightXtalGroups>(mycali,mydbservice->endOfTime(),callbackToken);
  // to start from the firstrun


  try{    
    if (objectName_ == "EcalWeightXtalGroups") {
      EcalWeightXtalGroups* mycali=readEcalWeightXtalGroupsFromFile();
      irun=firstrun_;
      try{
	std::cout<<"current time "<<mydbservice->currentTime()<<std::endl;
	mydbservice->newValidityForNewPayload<EcalWeightXtalGroups>(mycali,mydbservice->currentTime(),callbackToken);
	
      }catch(const cond::Exception& er){
	std::cout<<er.what()<<std::endl;
      }catch(const std::exception& er){
	std::cout<<"caught std::exception "<<er.what()<<std::endl;
      }catch(...){
	std::cout<<"Funny error"<<std::endl;
      }
    } else if (objectName_  =="EcalTBWeights") {

      EcalTBWeights* mycali=readEcalTBWeightsFromFile();
      irun=firstrun_;

      try{
	std::cout<<"current time "<<mydbservice->currentTime()<<std::endl;
	mydbservice->newValidityForNewPayload<EcalTBWeights>(mycali,mydbservice->currentTime(),callbackToken);
	
      }catch(const cond::Exception& er){
	std::cout<<er.what()<<std::endl;
      }catch(const std::exception& er){
	std::cout<<"caught std::exception "<<er.what()<<std::endl;
      }catch(...){
	std::cout<<"Funny error"<<std::endl;
      }
    }else if (objectName_  == "EcalADCToGeVConstant") {


      EcalADCToGeVConstant* mycali=readEcalADCToGeVConstantFromFile();
      irun=firstrun_;

      try{
	std::cout<<"current time "<<mydbservice->currentTime()<<std::endl;
	mydbservice->newValidityForNewPayload<EcalADCToGeVConstant>(mycali,mydbservice->currentTime(),callbackToken);
	
      }catch(const cond::Exception& er){
	std::cout<<er.what()<<std::endl;
      }catch(const std::exception& er){
	std::cout<<"caught std::exception "<<er.what()<<std::endl;
      }catch(...){
	std::cout<<"Funny error"<<std::endl;
      }
      
    } else if (objectName_  ==  "EcalIntercalibConstants") {
      EcalIntercalibConstants* mycali=readEcalIntercalibConstantsFromFile();
      irun=firstrun_;

      try{
	std::cout<<"current time "<<mydbservice->currentTime()<<std::endl;
	mydbservice->newValidityForNewPayload<EcalIntercalibConstants>(mycali,mydbservice->currentTime(),callbackToken);
	
      }catch(const cond::Exception& er){
	std::cout<<er.what()<<std::endl;
      }catch(const std::exception& er){
	std::cout<<"caught std::exception "<<er.what()<<std::endl;
      }catch(...){
	std::cout<<"Funny error"<<std::endl;
      }

    } else if (objectName_  ==  "EcalGainRatios") {
	
      EcalGainRatios* mycali=readEcalGainRatiosFromFile();
      irun=firstrun_;

      try{
	std::cout<<"current time "<<mydbservice->currentTime()<<std::endl;
	mydbservice->newValidityForNewPayload<EcalGainRatios>(mycali,mydbservice->endOfTime(),callbackToken);
	
      }catch(const cond::Exception& er){
	std::cout<<er.what()<<std::endl;
      }catch(const std::exception& er){
	std::cout<<"caught std::exception "<<er.what()<<std::endl;
      }catch(...){
	std::cout<<"Funny error"<<std::endl;
      }
   
    } else {
      cout << "ERROR:  Object " << objectName_  << " is not supported by this program." << endl;
    }
    

  } catch(cond::Exception &e) {
    
    //    writeToLogFileResults("finished with exception\n");
    cout << e.what() << endl;
  } catch(std::exception &e) {
   
    // writeToLogFileResults("finished with exception\n");
    cout << e.what() << endl;
  } catch(...) {
    
    //writeToLogFileResults("finished with exception\n");
    cout << "Unknown exception" << endl;
  }

  cout << "about to write to logfile " << endl ;

  // writeToLogFileResults("finished OK\n");
  cout << "done write to logfile " << endl ;



}

#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE(createEcalObject);
