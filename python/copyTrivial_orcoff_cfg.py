import FWCore.ParameterSet.Config as cms

process = cms.Process("TEST")
process.load("CalibCalorimetry.EcalTrivialCondModules.EcalTrivialCondRetriever_cfi")

process.load("CondCore.DBCommon.CondDBCommon_cfi")
#process.CondDBCommon.connect = 'oracle://cms_orcoff_prep/CMS_COND_ECAL'
process.CondDBCommon.DBParameters.authenticationPath = '/afs/cern.ch/cms/DB/conddb/'
process.CondDBCommon.connect = 'sqlite_file:DB.db'

process.MessageLogger = cms.Service("MessageLogger",
    debugModules = cms.untracked.vstring('*'),
    destinations = cms.untracked.vstring('cout')
)

process.source = cms.Source("EmptyIOVSource",
    firstValue = cms.uint64(1),
    lastValue = cms.uint64(1),
    timetype = cms.string('runnumber'),
    interval = cms.uint64(1)
)

process.PoolDBOutputService = cms.Service("PoolDBOutputService",
    process.CondDBCommon,
    toPut = cms.VPSet(cms.PSet(
        record = cms.string('EcalPedestalsRcd'),
        tag = cms.string('EcalPedestals_mc')
    ), 
        cms.PSet(
            record = cms.string('EcalADCToGeVConstantRcd'),
            tag = cms.string('EcalADCToGeVConstant_mc')
        ), 
        cms.PSet(
            record = cms.string('EcalChannelStatusRcd'),
            tag = cms.string('EcalChannelStatus_mc')
        ), 
        cms.PSet(
            record = cms.string('EcalIntercalibConstantsRcd'),
            tag = cms.string('EcalIntercalibConstants_mc')
        ), 
        cms.PSet(
            record = cms.string('EcalIntercalibErrorsRcd'),
            tag = cms.string('EcalIntercalibErrors_mc')
        ), 
        cms.PSet(
            record = cms.string('EcalGainRatiosRcd'),
            tag = cms.string('EcalGainRatios_mc')
        ), 
        cms.PSet(
            record = cms.string('EcalWeightXtalGroupsRcd'),
            tag = cms.string('EcalWeightXtalGroups_mc')
        ), 
        cms.PSet(
            record = cms.string('EcalTBWeightsRcd'),
            tag = cms.string('EcalTBWeights_mc')
        ), 
        cms.PSet(
            record = cms.string('EcalLaserAlphasRcd'),
            tag = cms.string('EcalLaserAlphas_mc')
        ), 
        cms.PSet(
            record = cms.string('EcalLaserAPDPNRatiosRcd'),
            tag = cms.string('EcalLaserAPDPNRatios_mc')
        ), 
        cms.PSet(
            record = cms.string('EcalLaserAPDPNRatiosRefRcd'),
            tag = cms.string('EcalLaserAPDPNRatiosRef_mc')
        ))
)

process.dbCopy = cms.EDAnalyzer("EcalDBCopy",
    timetype = cms.string('runnumber'),
    toCopy = cms.VPSet(cms.PSet(
        record = cms.string('EcalPedestalsRcd'),
        container = cms.string('EcalPedestals')
    ), 
        cms.PSet(
            record = cms.string('EcalADCToGeVConstantRcd'),
            container = cms.string('EcalADCToGeVConstant')
        ), 
        cms.PSet(
            record = cms.string('EcalChannelStatusRcd'),
            container = cms.string('EcalChannelStatus')
        ), 
        cms.PSet(
            record = cms.string('EcalIntercalibConstantsRcd'),
            container = cms.string('EcalIntercalibConstants')
        ), 
        cms.PSet(
            record = cms.string('EcalIntercalibErrorsRcd'),
            container = cms.string('EcalIntercalibErrors')
        ), 
        cms.PSet(
            record = cms.string('EcalGainRatiosRcd'),
            container = cms.string('EcalGainRatios')
        ), 
        cms.PSet(
            record = cms.string('EcalWeightXtalGroupsRcd'),
            container = cms.string('EcalWeightXtalGroups')
        ), 
        cms.PSet(
            record = cms.string('EcalTBWeightsRcd'),
            container = cms.string('EcalTBWeights')
        ), 
        cms.PSet(
            record = cms.string('EcalLaserAlphasRcd'),
            container = cms.string('EcalLaserAlphas')
        ), 
        cms.PSet(
            record = cms.string('EcalLaserAPDPNRatiosRcd'),
            container = cms.string('EcalLaserAPDPNRatios')
        ), 
        cms.PSet(
            record = cms.string('EcalLaserAPDPNRatiosRefRcd'),
            container = cms.string('EcalLaserAPDPNRatiosRef')
        ))
)

process.prod = cms.EDAnalyzer("EcalTrivialObjectAnalyzer")

process.p = cms.Path(process.prod*process.dbCopy)
