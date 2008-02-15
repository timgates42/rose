/****************************************************
 * RoseBin :: Binary Analysis for ROSE
 * Author : tps
 * Date : Sep26 07
 * Decription : Visualization in DOT
 ****************************************************/

#ifndef __RoseBin_DotGraph__
#define __RoseBin_DotGraph__

#include "RoseBin_Graph.h"


class RoseBin_DotGraph : public RoseBin_Graph {
 private:
  void printInternalNodes( bool dfg,  bool forward_analysis, 
			   std::ofstream &myfile, std::string& recursiveFunctionName,
			   SgAsmFunctionDeclaration* p_binFunc);

  typedef std::multimap < SgAsmFunctionDeclaration*, 
    std::pair <std::string, SgDirectedGraphNode*> > inverseNodeType;
  inverseNodeType inverse_nodesMap;

  void printEdges_single( bool forward_analysis, std::ofstream& myfile);
  void printEdges_multiple( bool forward_analysis, std::ofstream& myfile);
  void printEdges( bool forward_analysis, std::ofstream& myfile, SgDirectedGraphEdge* edge);

 public:
  RoseBin_DotGraph() {}
  ~RoseBin_DotGraph() {}

  void printNodes( bool dfg,  bool forward_analysis, 
		     std::ofstream &myfile, std::string& recursiveFunctionName);

  void printEdges( bool forward_analysis, std::ofstream& myfile, bool mergedEdges);

  void printProlog(  std::ofstream& myfile, std::string& fileType);
  void printEpilog(  std::ofstream& myfile);

};

#endif
