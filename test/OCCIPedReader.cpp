// Directly accesses POOL-ORA EcalPedestals from the DB
// with OCCI for comparison purposes


#include "CondFormats/EcalObjects/interface/EcalPedestals.h"

#include <iostream>
#include <string>
#include <sstream>
#include <stdexcept>
#include "occi.h"

using namespace std;
using namespace oracle::occi;

class ReadApp {
public:
  ReadApp(string host, string sid, string user, string pass, int port=1521)
  {
    stringstream ss;
    ss << "//" << host << ":" << port << "/" << sid;
    
    cout << "Connecting to " << host << "..." << flush;
    env = Environment::createEnvironment(Environment::OBJECT);
    conn = env->createConnection(user, pass, ss.str());
    cout << "Done." << endl;
  }

  ~ReadApp()
  {
    env->terminateConnection(conn);
    Environment::terminateEnvironment(env);
  }

  void readPedestals()
  {
    Statement* stmt = conn->createStatement();
    
    // Get the object IOVs
    cout << "Getting the \"IOV\"s... " << flush;
    stmt->setSQL("SELECT id FROM EcalPedestals ORDER BY ID");
    ResultSet* rset = stmt->executeQuery();
    vector<int> ids;
    while (rset->next()) {
      ids.push_back(rset->getInt(1));
    }
    cout << "Done." << endl;

    cout << "Getting the objects..." << endl;
    EcalPedestals ped;
    int detId;
    EcalPedestals::Item item;
    stmt->setSQL("SELECT m_pedestals_int, "
		 "m_pedestals_ecalpedestals_it_1, "
		 "m_pedestals_ecalpedestals_it_2, "
		 "m_pedestals_ecalpedestals_it_3, "
		 "m_pedestals_ecalpedestals_it_4, "
		 "m_pedestals_ecalpedestals_it_5, "
		 "m_pedestals_ecalpedestals_it_6 "
		 "FROM ecalpedestals_m_pe_1 WHERE id_id = :id ORDER BY POS");

    int last = ids.size();
    for (vector<int>::const_iterator p = ids.begin();
	 p != ids.end(); ++p) {
      ped = EcalPedestals();
      cout << "Getting object " << *p + 1 << " of " << last << "..." << flush;
      stmt->setInt(1, *p);
      cout << "executing..." << flush;
      rset = stmt->executeQuery();
      cout << "iterating..." << flush;
      while(rset->next()) {
	detId = rset->getInt(1);
	item.mean_x12 = rset->getInt(2);
	item.rms_x12  = rset->getInt(3);
	item.mean_x6  = rset->getInt(4);
	item.rms_x6   = rset->getInt(5);
	item.mean_x1  = rset->getInt(6);
	item.rms_x1   = rset->getInt(7);
	ped.m_pedestals[detId] = item;
	// ped.m_pedestals.insert(pair<int,EcalPedestals::Item>(detId,item));
      }
      cout << "got " << ped.m_pedestals.size() << " channels.  Done." << endl;
    }
    cout << "Finished getting " << last << " objects." << endl;
  }

private:
  Environment* env;
  Connection* conn;
};

int main(int argc, char* argv[])
{
  string host = "int2r1-v.cern.ch";
  string sid = "cms_val_lb";
  string user = "cms_val_ecal_pool_owner";
  string pass = "val_eca_own_1031";

  try {
    ReadApp app(host, sid, user, pass);
    app.readPedestals();
  } catch (SQLException &e) {
    cerr << "SQLException:  " << e.getMessage() << endl;
  } catch (exception &e) {
    cerr << "exception:  " << e.what() << endl;
  } catch (...) {
    cerr << "Unknown exception caught" << endl;
  }

  return 0;
}
