/**
   Translates a EcalFloatCondObjectContainer record to XML
   and vice versa   

   \author Stefano ARGIRO
   \version $Id: EcalIntercalibConstantsXMLTranslator.h,v 1.1 2008/11/14 15:46:05 argiro Exp $
   \date 20 Jun 2008
*/

#ifndef __EcalFloatCondObjectContainterXMLTranslator_h_
#define __EcalFloatCondObjectContainterXMLTranslator_h_

#include "CondTools/Ecal/interface/XercesString.h"
#include "CondTools/Ecal/interface/EcalCondHeader.h"
#include "CondFormats/EcalObjects/interface/EcalCondObjectContainer.h"
#include <xercesc/dom/DOMNode.hpp>
#include <string>



class EcalFloatCondObjectContainerXMLTranslator {

public:
  

  static int readXML  (const std::string& filename,
		       EcalCondHeader& header,
		       EcalFloatCondObjectContainer& record);

  static int writeXML (const std::string& filename, 
		       const EcalCondHeader& header,
		       const EcalFloatCondObjectContainer& record);
  
  static std::string dumpXML(const EcalCondHeader& header,
			     const EcalFloatCondObjectContainer& record);

};



#endif // __EcalFloatCondObjectContainerXMLTranslator_h_

// Configure (x)emacs for this file ...
// Local Variables:
// mode:c++
// compile-command: "cd ..; scram b"
// End: