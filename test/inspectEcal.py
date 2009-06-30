import os,sys, DLFCN,getopt
sys.setdlopenflags(DLFCN.RTLD_GLOBAL+DLFCN.RTLD_LAZY)

from CondCore.Utilities import iovInspector as inspect
from pluginCondDBPyInterface import *

def usage():
    print "inspectEcal -c [connectstring] -P [authpath] -t [tag] -f [outfile] -l"
    print "   dump records in xml" 
    print "               -l: list tags and exit"
    print "               -f [file] : dump to file"
      
try:
    opts, args = getopt.getopt(sys.argv[1:], "c:P:t:f:l", ["connect=","authpath=","tag","file","listtags"])

except getopt.GetoptError:
    #* print help information and exit:*
    usage()
    sys.exit(2)


dbName =  "oracle://cms_orcoff_prod/CMS_COND_31X_ECAL"
authpath= "/afs/cern.ch/cms/DB/conddb"
tag='EcalIntercalibConstants_mc'
do_list_tags= 0
dump_to_file =0
outfile=""

for opt,arg in opts:

    if opt in ("-c","--connect"):
        try: 
            dbname=arg
        except Exception, er :
            print er
    
    if opt in ("-P","--authpath"):
        try: 
            rdbms=RDBMS(arg)
        except Exception, er :
            print er
    if opt in ("-t","--tag"):
        tag=arg

    if opt in ("-l","--listtags"):
        do_list_tags= 1
        
    if opt in ("-f","--file"):
        dump_to_file= 1
        outfile=arg    

a = FWIncantation()

rdbms = RDBMS(authpath)
db = rdbms.getDB(dbName)

if do_list_tags :
    tags=db.allTags()
    for tag in tags.split():
        print tag
    sys.exit(0)    


try :
    iov = inspect.Iov(db,tag)
    print "===iov list ==="
    iovlist=iov.list()
    print iovlist
    print "===iov summaries ==="
    print iov.summaries()
    print "===payload dump ==="
    for p in iovlist:
        payload=inspect.PayLoad(db,p[0])
        #print payload.summary()
        if dump_to_file:
            print "Dumping to file:", outfile 
            out = open(outfile,"w")
            print >> out, payload
        else:
            print payload
   
except Exception, er :
    print er


