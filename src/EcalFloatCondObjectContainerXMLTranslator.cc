#include <iostream>
#include <sstream>
#include <fstream>

#include <xercesc/dom/DOMNode.hpp>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/sax/SAXException.hpp>
#include <xercesc/framework/LocalFileFormatTarget.hpp>



#include "CondTools/Ecal/interface/EcalFloatCondObjectContainerXMLTranslator.h"
#include "CondTools/Ecal/interface/DOMHelperFunctions.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace xuti;
using namespace std;


int  
EcalFloatCondObjectContainerXMLTranslator::readXML(const std::string& filename,
					      EcalCondHeader&          header,
					      EcalFloatCondObjectContainer& record){

  XMLPlatformUtils::Initialize();

  XercesDOMParser* parser = new XercesDOMParser;
  parser->setValidationScheme( XercesDOMParser::Val_Never );
  parser->setDoNamespaces( false );
  parser->setDoSchema( false );
  
  parser->parse(filename.c_str());

  DOMDocument* xmlDoc = parser->getDocument();
  

  if (!xmlDoc) {
    cout << "EcalFloatCondObjectContainerXMLTranslator::Error parsing document" << endl;
    return -1;
  }

  DOMElement* elementRoot = xmlDoc->getDocumentElement();

  xuti::readHeader(elementRoot, header);

  // get the first cell node
  DOMNode * cellnode=getChildNode(elementRoot,Cell_tag);
  
  // loop on cell nodes
  while  (cellnode){

    float val=0;

    // read id
    DetId detid= readCellId(cellnode);
       
    // read value
    DOMNode * c_node = getChildNode(cellnode,EcalFloatCondObjectContainer_tag);
    GetNodeData(c_node,val);

    // fill record
    record[detid]=val;

    // get next cell
    cellnode= cellnode->getNextSibling();
    
    while (cellnode&& cellnode->getNodeType( ) != DOMNode::ELEMENT_NODE)      
      cellnode= cellnode->getNextSibling();
    
    
  }


  delete parser;
  XMLPlatformUtils::Terminate();
  return 0;
    
}





std::string 
EcalFloatCondObjectContainerXMLTranslator::dumpXML(       
				  const EcalCondHeader&   header,
				  const EcalFloatCondObjectContainer& record){
    


  
  XMLPlatformUtils::Initialize();
  
  DOMImplementation*  impl =
    DOMImplementationRegistry::getDOMImplementation(fromNative("LS").c_str());
  
  DOMWriter* writer =
    static_cast<DOMImplementationLS*>(impl)->createDOMWriter( );
  writer->setFeature(XMLUni::fgDOMWRTFormatPrettyPrint, true);
  
  DOMDocumentType* doctype = 
    impl->createDocumentType( fromNative("XML").c_str(), 0, 0 );
  DOMDocument *    doc = 
    impl->createDocument( 0, fromNative(EcalFloatCondObjectContainer_tag).c_str(), doctype );
  
  
  doc->setEncoding(fromNative("UTF-8").c_str() );
  doc->setStandalone(true);
  doc->setVersion(fromNative("1.0").c_str() );
  
  
  DOMElement* root = doc->getDocumentElement();
  xuti::writeHeader(root, header);

  for (int cellid = EBDetId::MIN_HASH; 
       cellid < EBDetId::kSizeForDenseIndexing; 
       ++cellid){// loop on EB cells
    
    uint32_t rawid= EBDetId::unhashIndex(cellid);

    if (!record[rawid]) continue; // cell absent from original record
    
    DOMElement* cellnode = doc->createElement( fromNative(Cell_tag).c_str());
    root->appendChild(cellnode);
     
    writeCellId(cellnode,rawid);

    WriteNodeWithValue(cellnode,Value_tag,record[rawid]);
 

  } // loop on EB cells
  
  
  
  for (int cellid = 0; 
       cellid < EEDetId::kSizeForDenseIndexing; 
       ++cellid){// loop on EE cells
    
    if (!EEDetId::validHashIndex(cellid)) continue;

    uint32_t rawid= EEDetId::unhashIndex(cellid);
    if (!record[rawid]) continue; // cell absent from original record

    DOMElement* cellnode = doc->createElement(fromNative(Cell_tag).c_str());
    root->appendChild(cellnode);
        
    writeCellId(cellnode,rawid);
    WriteNodeWithValue(cellnode,Value_tag,record[rawid]);

    
  } // loop on EE cells
  
  
  std::string dump= toNative(writer->writeToString(*root));
  doc->release();
  return dump;
}



int 
EcalFloatCondObjectContainerXMLTranslator::writeXML(const std::string& filename,         
					       const EcalCondHeader&   header,
					       const EcalFloatCondObjectContainer& record){

  std::fstream fs(filename.c_str(),ios::out);
  fs<< dumpXML(header,record);
  return 0;  
}