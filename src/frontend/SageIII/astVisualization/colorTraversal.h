#ifndef COLOR_TRAVERSAL_H
#define COLOR_TRAVERSAL_H

class CustomAstDOTGenerationEdgeType
   {
  // EdgeType holdes information required to specify an edge to DOT

     public:
          SgNode* start;
          SgNode* end;

       // DQ (3/5/2007): Added label string (mostly for symmetry with CustomAstDOTGenerationNodeType)
          std::string labelString;

          std::string optionString;

       // DQ (3/5/2007): Need to set the labelString, but not yet.
          CustomAstDOTGenerationEdgeType (SgNode* n1, SgNode* n2, std::string label = "", std::string options = "")
             : start(n1), end(n2), labelString(label), optionString(options) {}

       // DQ (3/6/2007): Modified to only focus on the start and end nodes, actually we need the label test
       // These are implemented to support the STL find function (STL algorithm)
       // bool operator!=(const CustomAstDOTGenerationEdgeType & edge) const { return (edge.start != start) || (edge.end != end) || (edge.optionString != optionString); }
       // bool operator==(const CustomAstDOTGenerationEdgeType & edge) const { return (edge.start == start) && (edge.end == end) && (edge.optionString == optionString); }
       // bool operator!=(const CustomAstDOTGenerationEdgeType & edge) const { return (edge.start != start) || (edge.end != end); }
       // bool operator==(const CustomAstDOTGenerationEdgeType & edge) const { return (edge.start == start) && (edge.end == end); }
          bool operator!=(const CustomAstDOTGenerationEdgeType & edge) const { return (edge.start != start) || (edge.end != end) || (edge.labelString != labelString); }
          bool operator==(const CustomAstDOTGenerationEdgeType & edge) const { return (edge.start == start) && (edge.end == end) && (edge.labelString == labelString); }
       // bool operator< (const CustomAstDOTGenerationEdgeType & edge) const { return (edge.start < start) || (edge.start == start) && (edge.end < end); }
          bool operator< (const CustomAstDOTGenerationEdgeType & edge) const { return (edge.start < start) || ((edge.start == start) && (edge.labelString == labelString)) && (edge.end < end); }
   };

class CustomAstDOTGenerationNodeType
   {
  // NodeType holdes information required to specify additional info for any node to DOT

     public:
          SgNode* target;
          std::string labelString;
          std::string optionString;

          CustomAstDOTGenerationNodeType (SgNode* node, std::string label, std::string options )
             : target(node), labelString(label), optionString(options)
             {
               ROSE_ASSERT(node != NULL);
               ROSE_ASSERT(labelString.size() < 4000);
               ROSE_ASSERT(optionString.size() < 4000);
             }

       // DQ (3/21/2007): Added copy constructor
          CustomAstDOTGenerationNodeType ( const CustomAstDOTGenerationNodeType & X )
             : target(X.target), labelString(X.labelString), optionString(X.optionString)
             {
               ROSE_ASSERT(X.optionString.size() < 4000);
               ROSE_ASSERT(optionString.size() < 4000);
             }

       // These are implemented to support the STL find function (STL algorithm)
          bool operator!=(const CustomAstDOTGenerationNodeType & node) const { return node.target != target; }
          bool operator==(const CustomAstDOTGenerationNodeType & node) const { return node.target == target; }
   };

class CustomAstDOTGenerationData : public AstDOTGeneration
   {
  // Specialization of AST DOT graphics output class to support the overloaded operators
  // Note that this class is derived from a traversal and so can not be the class that is
  // used in the multiple inhertance design where CustomDOTGeneration is used.

     public:
          typedef CustomAstDOTGenerationNodeType NodeType;
          typedef CustomAstDOTGenerationEdgeType EdgeType;

          typedef std::list<NodeType> NodeListType;
          typedef std::list<EdgeType> EdgeListType;

          NodeListType nodeList;
          EdgeListType edgeList;

       // Call add functions in AstDOTGeneration class
          void addNode(NodeType n);
          void addEdge(EdgeType e);

          void addEdges();

       // void generate(string filename, SgProject* node);
       // void generateInputFiles(SgProject* node, traversalType tt=TOPDOWNBOTTOMUP, std::string filenamePostfix="");

       // void addEdges ( std::list<EdgeType> & edgeList );
          virtual std::string additionalNodeInfo(SgNode* node);
          virtual std::string additionalNodeOptions(SgNode* node);

       // DQ (3/5/2007) added mechanism to add options (to add color, etc.)
          virtual std::string additionalEdgeInfo    ( SgNode* from, SgNode* to, std::string label );
          virtual std::string additionalEdgeOptions ( SgNode* from, SgNode* to, std::string label );

       // This generates code for the DOT graph node
          std::string unparseToCompleteStringForDOT( SgNode* astNode );

       // Wrapup details and output the file (generates the rest of the nodes in the AST)
          void internalGenerateGraph( std::string filename, SgProject* project );
   };

// In order to get the CustomAstDOTGenerationData data member into the traversal
// we will represent it in a class and use multiple inheritance.
class CustomAstDOTGeneration
   {
     public:
          typedef CustomAstDOTGenerationData::NodeType NodeType;
          typedef CustomAstDOTGenerationData::EdgeType EdgeType;

       // Store a DOT graph (really just a list of nodes and edges in a traversal)
          CustomAstDOTGenerationData DOTgraph;

         ~CustomAstDOTGeneration();

       // Call add functions in CustomAstDOTGenerationData class
          void addNode(NodeType n);
          void addEdge(EdgeType e);

       // Wrapup details and output the file (generates the rest of the nodes in the AST)
          void internalGenerateGraph( std::string filename, SgProject* project );
   };


// Build a similar coloring mechanism as for the ROSE traversals developed by Markus,
// but do this for the Memory Pool based traversals.
class CustomMemoryPoolDOTGenerationData : public ROSE_VisitTraversal
   {
     public:
          typedef CustomAstDOTGenerationNodeType NodeType;
          typedef CustomAstDOTGenerationEdgeType EdgeType;

          typedef std::list<NodeType> NodeListType;
          typedef std::list<EdgeType> EdgeListType;

          typedef std::set<EdgeType> EdgeSetType;

          NodeListType nodeList;
          EdgeListType edgeList;

       // Nodes to ignore when building the DOT graph, we have to remove it from the nodeList if it is there.
          std::set<SgNode*> skipNodeList;

       // Edges to ignore when building whole AST graph
          EdgeSetType skipEdgeSet;

          virtual ~CustomMemoryPoolDOTGenerationData();

          void internalGenerateGraph(std::string dotfilename);

  // protected:
          void visit(SgNode* node);

       // Call the functions in the DOTRepresentation
          void addNode(NodeType n);
          void addEdge(EdgeType e);

          void skipNode(SgNode* n);
          void skipEdge(EdgeType e);

          void addEdges();

          virtual std::string additionalNodeInfo(SgNode* node);

       // DQ (11/1/2003) added mechanism to add options (to add color, etc.)
          virtual std::string additionalNodeOptions(SgNode* node);

       // DQ (3/5/2007) added mechanism to add options (to add color, etc.)
          virtual std::string additionalEdgeInfo    ( SgNode* from, SgNode* to, std::string label );
          virtual std::string additionalEdgeOptions ( SgNode* from, SgNode* to, std::string label );

          DOTRepresentation<SgNode*> dotrep;

     protected:
          std::string filename;
          std::set<SgNode*> visitedNodes;
   };

// In order to get the CustomAstDOTGenerationData data member into the traversal
// we will represent it in a class and use multiple inheritance.
// This is a copy of CustomDOTGeneration using a CustomMemoryPoolDOTGenerationData
// instead of a CustomAstDOTGenerationData data member
class CustomMemoryPoolDOTGeneration
   {
     public:
          typedef CustomAstDOTGenerationData::NodeType NodeType;
          typedef CustomAstDOTGenerationData::EdgeType EdgeType;

       // Store a DOT graph (really just a list of nodes and edges in a traversal)
          CustomMemoryPoolDOTGenerationData DOTgraph;

      // ~CustomMemoryPoolDOTGeneration();

       // Call add functions in CustomMemoryPoolDOTGenerationData class
          void addNode(NodeType n);
          void addEdge(EdgeType e);

          void skipNode(SgNode* n);
          void skipEdge(EdgeType e);

       // Wrapup details and output the file (generates the rest of the nodes in the AST)
          void internalGenerateGraph( std::string filename );

       // DQ (5/11/2006): This filters out the gnu compatability IR nodes (which tend to confuse everyone!)
          void frontendCompatabilityFilter(SgNode* n);
          void typeFilter(SgNode* n);
          void commentAndDirectiveFilter(SgNode* n);

       // DQ (5/14/2006): General mechanism to remove edges from generated DOT graph
          void edgeFilter(SgNode* nodeSource, SgNode* nodeSink, std::string edgeName );

       // DQ (5/11/2006): This adds colors to the whole AST graph
          void defaultColorFilter(SgNode* n);

       // Default fileter to simplify the whole AST graph
          void defaultFilter(SgNode* n);

       // Mechanism to ignore all SgFile Info IR nodes (often they just get in the way)
          void fileInfoFilter(SgNode* node);
   };






// **********************************************************
// **********************************************************
//       Source code which can be more later
// **********************************************************
// **********************************************************

// using namespace std;

#if 1
CustomAstDOTGeneration::~CustomAstDOTGeneration()
   {
   }
#endif

void
CustomAstDOTGenerationData::addNode(NodeType n)
   {
     ROSE_ASSERT(this != NULL);
  // printf ("In CustomAstDOTGenerationData::addNode(): n->target = %p = %s options = %s size_ld = %ld size_d = %d \n",
  //      n.target,n.target->class_name().c_str(),n.optionString.c_str(),n.optionString.size(),n.optionString.size());
     ROSE_ASSERT(n.optionString.size() < 4000);
     nodeList.push_back(n);
   }

void
CustomAstDOTGenerationData::addEdge(EdgeType e)
   {
     ROSE_ASSERT(this != NULL);
     edgeList.push_back(e);
   }

void 
CustomAstDOTGenerationData::addEdges()
   {
  // Add the local list of edges to the dot graph

     for (std::list<EdgeType>::iterator i = edgeList.begin(); i != edgeList.end(); i++)
        {
          ROSE_ASSERT ((*i).start != NULL);
          ROSE_ASSERT ((*i).end   != NULL);
       // printf ("Adding an edge from a %s to a %s with options = %s \n",
       //      (*i).start->sage_class_name(),(*i).end->sage_class_name(),(*i).optionString.c_str());
          dotrep.addEdge((*i).start,(*i).end,(*i).optionString);
        }
   }

std::string
CustomAstDOTGenerationData::additionalNodeInfo(SgNode* node)
   {
  // This virtual function only modifies the node label not the options list that would 
  // follow the node's label in the final dot output file.
     std::string returnString;

     const NodeType tmp (node,"","");
     std::list<NodeType>::iterator i = nodeList.end();
     i = find(nodeList.begin(),nodeList.end(),tmp);
     if (i != nodeList.end())
        {
          returnString = (*i).labelString;
       // printf ("Adding node label info (%s) to %s \n",returnString.c_str(),node->sage_class_name());
        }

     return returnString;
   }

std::string
CustomAstDOTGenerationData::additionalNodeOptions(SgNode* node)
   {
  // This virtual function adds options (after the node label) in the output DOT file
  // printf ("Called for node = %s \n",node->class_name().c_str());

     std::string returnString;
  // printf ("default: returnString = %s size() = %ld nodeList.size() = %ld \n",returnString.c_str(),returnString.size(),nodeList.size());

     const NodeType tmp (node,"","");
     std::list<NodeType>::iterator i = nodeList.begin();
     i = find(nodeList.begin(),nodeList.end(),tmp);
     if (i != nodeList.end())
        {
       // printf ("Before: Adding node option info (%s) to %s (*i).optionString.size() = %ld \n",returnString.c_str(),node->sage_class_name(),(*i).optionString.size());
          ROSE_ASSERT(i->target == node);
          returnString = (*i).optionString;
       // printf ("Adding node option info (%s) to %s \n",returnString.c_str(),node->sage_class_name());
        }

  // printf ("returnString = %s size() = %ld \n",returnString.c_str(),returnString.size());

     return returnString;
   }

std::string
CustomAstDOTGenerationData::additionalEdgeInfo(SgNode* from, SgNode* to, std::string label)
   {
  // This virtual function only modifies the node label not the options list that would 
  // follow the node's label in the final dot output file.
     std::string returnString;

     const EdgeType tmp (from,to,label,"");
     std::list<EdgeType>::iterator i = edgeList.end();
     i = find(edgeList.begin(),edgeList.end(),tmp);
     if (i != edgeList.end())
        {
          returnString = (*i).labelString;
       // printf ("Adding edge label info (%p) to (%p) \n",from,to);
        }

     return returnString;
   }

std::string
CustomAstDOTGenerationData::additionalEdgeOptions(SgNode* from, SgNode* to, std::string label)
   {
  // This virtual function adds options (after the node label) in the output DOT file
  // printf ("CustomAstDOTGenerationData::additionalEdgeOptions from = %p = %s -> label = %s -> to = %p = %s \n",
  //      from,from->class_name().c_str(),label.c_str(),to,to->class_name().c_str());

     std::string returnString;
#if 0
  // This will find the edge in the edge list, but the edgList is always empty, unless we 
  // add edges explicitly.  So this code is not the way to identify edges. Instead we
  // should just have the base class generate an empty string.

     const EdgeType tmp (from,to,label,"");
     std::list<EdgeType>::iterator i = edgeList.end();
     i = find(edgeList.begin(),edgeList.end(),tmp);
     if (i != edgeList.end())
        {
          returnString = (*i).optionString;
       // printf ("Adding edge label info (%p) to (%p) \n",from,to);
        }
#endif
     return returnString;
   }

std::string
CustomAstDOTGenerationData::unparseToCompleteStringForDOT( SgNode* astNode )
   {
  // Generate the std::string (pass a SgUnparse_Info object)
     SgUnparse_Info* inputUnparseInfoPointer = new SgUnparse_Info();
     inputUnparseInfoPointer->unset_SkipComments();    // generate comments
     inputUnparseInfoPointer->unset_SkipWhitespaces(); // generate all whitespaces to format the code
     std::string outputString = globalUnparseToString(astNode,inputUnparseInfoPointer);
     std::string returnString = StringUtility::escapeNewLineCharaters(outputString);

     delete inputUnparseInfoPointer;

     return returnString;
   }

void
CustomAstDOTGenerationData::internalGenerateGraph( std::string filename, SgProject* project )
   {
  // DQ (2/2/2007): Introduce tracking of performance of within AST merge
     std::string label =  "CustomAstDOTGenerationData::internalGenerateGraph(" + filename + "):";
     TimingPerformance timer (label);

  // Required setup on base class
     init();

  // Specify the traversal that will be used (I forget why this is required)
  // DOTgraph.traversal = DOTGeneration::TOPDOWNBOTTOMUP;
     traversal = AstDOTGeneration::TOPDOWNBOTTOMUP;

  // Build the DOT specific inherited attribute
     DOTInheritedAttribute ia;

     traverse(project,ia);
  // traverseInputFiles(project,ia);

  // Put extra code here
     addEdges();

  // Output the source code to a new node (we need a function to escape all the """ and "\n")
  // std::string sourceCodeString = unparseToCompleteStringForDOT(project);
  // dotrep.addNode(project,sourceCodeString,"shape=box");

     dotrep.writeToFileAsGraph(filename+".dot");
   }

void
CustomAstDOTGeneration::addNode( NodeType n )
   {
     ROSE_ASSERT(this != NULL);
     ROSE_ASSERT(n.optionString.size() < 4000);
     DOTgraph.addNode(n);
   }

void
CustomAstDOTGeneration::addEdge( EdgeType e )
   {
     ROSE_ASSERT(this != NULL);
     DOTgraph.addEdge(e);
   }

void
CustomAstDOTGeneration::internalGenerateGraph( std::string filename, SgProject* project )
   {
     ROSE_ASSERT(this != NULL);
     DOTgraph.internalGenerateGraph(filename,project);
   }











#if 1
CustomMemoryPoolDOTGenerationData::~CustomMemoryPoolDOTGenerationData() 
   {
   }
#endif

void
CustomMemoryPoolDOTGenerationData::internalGenerateGraph(std::string dotfilename)
   {
  // DQ (2/2/2007): Introduce tracking of performance of within AST merge
     std::string label =  "CustomMemoryPoolDOTGenerationData::internalGenerateGraph(" + dotfilename + "):";
     TimingPerformance timer (label);

     filename = dotfilename;

     printf ("skipNodeList.size() = %ld \n",skipNodeList.size());

#if 1
     std::set<SgNode*>::iterator i = skipNodeList.begin();
     while(i != skipNodeList.end())
        {
          printf ("     *i = %p = %s \n",*i,(*i)->class_name().c_str());
          Sg_File_Info* fileInfo = isSg_File_Info(*i);
          if (fileInfo != NULL)
             {
                if (fileInfo->get_parent() != NULL)
                   {
                     printf ("parent = %p = %s \n",fileInfo->get_parent(),fileInfo->get_parent()->class_name().c_str());
                     fileInfo->display("debugging CustomMemoryPoolDOTGenerationData");
                   }
             }
          i++;
        }
#endif

     traverseMemoryPool();
     dotrep.writeToFileAsGraph(filename+".dot");
   }

void
CustomMemoryPoolDOTGenerationData::addNode(NodeType n)
   {
  // printf ("Adding an node in CustomMemoryPoolDOTGenerationData::addNode \n");
     nodeList.push_back(n);
   }

void
CustomMemoryPoolDOTGenerationData::addEdge(EdgeType e)
   {
  // printf ("Adding an edge in CustomMemoryPoolDOTGenerationData::addEdge \n");
     edgeList.push_back(e);
   }

void
CustomMemoryPoolDOTGenerationData::skipNode(SgNode* n)
   {
     skipNodeList.insert(n);
  // visitedNodes.insert(n);
   }

void
CustomMemoryPoolDOTGenerationData::skipEdge(EdgeType e)
   {
     skipEdgeSet.insert(e);
   }

void 
CustomMemoryPoolDOTGenerationData::addEdges()
   {
  // Add the local list of edges to the dot graph
  // printf ("CustomAstDOTGenerationData::addEdges(): skipEdgeSet.size() = %ld \n",skipEdgeSet.size());

     for (std::list<EdgeType>::iterator i = edgeList.begin(); i != edgeList.end(); i++)
        {
          ROSE_ASSERT ((*i).start != NULL);
          ROSE_ASSERT ((*i).end   != NULL);
       // printf ("Adding an edge from a %s to a %s with options = %s \n",
       //      (*i).start->sage_class_name(),(*i).end->sage_class_name(),(*i).optionString.c_str());
#if 1
//          dotrep.addEdge((*i).start,(*i).end,(*i).optionString);
#else
          EdgeType e((*i).start,(*i).end);
          if (skipEdgeSet.find(e) != skipEdgeSet.end())
             {
               dotrep.addEdge((*i).start,(*i).end,(*i).optionString);
             }
#endif
        }
   }

std::string
CustomMemoryPoolDOTGenerationData::additionalNodeInfo(SgNode* node)
   {
  // This virtual function only modifies the node label not the options list that would 
  // follow the node's label in the final dot output file.
     std::string returnString;

     const NodeType tmp (node,"","");
     std::list<NodeType>::iterator i = nodeList.end();
     i = find(nodeList.begin(),nodeList.end(),tmp);
     if (i != nodeList.end())
        {
          returnString = (*i).labelString;
       // printf ("Adding node label info (%s) to %s \n",returnString.c_str(),node->sage_class_name());
        }

     return returnString;
   }

std::string
CustomMemoryPoolDOTGenerationData::additionalNodeOptions(SgNode* node)
   {
  // This virtual function adds options (after the node label) in the output DOT file
  // printf ("Called for node = %s \n",node->class_name().c_str());

     std::string returnString;

     const NodeType tmp (node,"","");
     std::list<NodeType>::iterator i = nodeList.end(); 
     i = find(nodeList.begin(),nodeList.end(),tmp);
     if (i != nodeList.end())
        {
          returnString = (*i).optionString;
       // printf ("Adding node option info (%s) to %s \n",returnString.c_str(),node->sage_class_name());
        }

     return returnString;
   }

#if 0
// These are inferior defaults (I think).
// To improve the default output add additional information here
// Note you need to add "\\n" for newline
string
CustomMemoryPoolDOTGenerationData::additionalNodeInfo(SgNode* node)
   {
     ostringstream ss;
     ss << "\\n";

  // print number of max successors (= container size)
     AstSuccessorsSelectors::SuccessorsContainer c;
     AstSuccessorsSelectors::selectDefaultSuccessors(node,c);
     ss << c.size() << "\\n";

  // add class name
     if (SgClassDeclaration* n = dynamic_cast<SgClassDeclaration*>(node))
        {
          ss << n->get_qualified_name().str() << "\\n";
        }

  // add memory location of node to dot output
     ss << node << "\\n";

     return ss.str();
   }

string
CustomMemoryPoolDOTGenerationData::additionalNodeOptions(SgNode* node)
   {
  // return an empty string for default implementation
     return string("");
   }
#endif

std::string
CustomMemoryPoolDOTGenerationData::additionalEdgeInfo(SgNode* from, SgNode* to, std::string label)
   {
  // This virtual function only modifies the node label not the options list that would 
  // follow the node's label in the final dot output file.
     std::string returnString;

     const EdgeType tmp (from,to,label);
     std::list<EdgeType>::iterator i = edgeList.end();
     i = find(edgeList.begin(),edgeList.end(),tmp);
     if (i != edgeList.end())
        {
          returnString = (*i).labelString;
       // printf ("Adding edge label info (%p) to (%p) \n",from,to);
        }

     return returnString;
   }

std::string
CustomMemoryPoolDOTGenerationData::additionalEdgeOptions(SgNode* from, SgNode* to, std::string label)
   {
  // This virtual function adds options (after the node label) in the output DOT file
  // printf ("CustomMemoryPoolDOTGenerationData::additionalEdgeOptions(from = %s to %s) label = %s edgeList.size() = %ld \n",from->class_name().c_str(),to->class_name().c_str(),label.c_str(),edgeList.size());

     std::string returnString;
#if 0
  // This will find the edge in the edge list, but the edgList is always empty, unless we 
  // add edges explicitly.  So this code is not the way to identify edges. Instead we
  // should just have the base class generate an empty string.

  // Build an example edge so that we can test againt it in the edge list
     const EdgeType tmp (from,to,label);
     std::list<EdgeType>::iterator i = edgeList.end();

  // Find the unique edge between the associated IR nodes that also matches the given label
     i = find(edgeList.begin(),edgeList.end(),tmp);
     if (i != edgeList.end())
        {
          returnString = i->optionString;
          printf ("i->optionString = %s \n",i->optionString.c_str());

       // DQ (3/5/2007): color the parent edges blue
          if (from->get_parent() == to)
             {
            // Note that labelfontcolor does not appear to work
            // returnString = "color=\"Blue\" decorate labelfontcolor=blue4";
            // returnString = "color=\"Blue\" decorate";
               returnString = "color=\"Blue\" decorate labelfontcolor=\"blue4\"";
             }

       // DQ (3/5/2007): color the scope edges green (that are explicitly stored in the AST)
       // SgScopeStatement* scopeStatement = isSgScopeStatement(to);
          SgStatement* statement = isSgStatement(from);
          if (statement != NULL)
             {
               if (statement->hasExplicitScope() == true && statement->get_scope() == to)
                  {
                 // returnString = "color=\"Green\" decorate labelfontcolor=green4";
                    returnString = "color=\"Green\" decorate";
                  }
             }
     
          SgInitializedName* initializedName = isSgInitializedName(from);
          if (initializedName != NULL)
             {
               if (initializedName->get_scope() == to)
                  {
                 // returnString = "color=\"Green\" decorate labelfontcolor=green4";
                    returnString = "color=\"Green\" decorate";
                  }
             }

          SgType* type = isSgType(to);
          if (type != NULL)
             {
            // returnString = "color=\"gold1\" decorate labelfontcolor=gold4";
               returnString = "color=\"gold1\" decorate";
             }

          printf ("Adding edge option info (%p) to (%p) returnString = %s \n",from,to,returnString.c_str());
        }
#else
  // DQ (3/6/2007): This builds a default set of mappings of edge colors and edge options to edges.

  // Color all edges that lead to a SgType (this is overwritten for the parent edges)
     SgType* type = isSgType(to);
     if (type != NULL)
        {
       // returnString = "color=\"gold1\" decorate labelfontcolor=gold4";
          returnString = "color=\"gold1\" decorate ";
        }

  // DQ (3/5/2007): color the parent edges blue
     if (from->get_parent() == to && label == "parent")
        {
       // Note that labelfontcolor does not appear to work
       // returnString = "color=\"Blue\" decorate labelfontcolor=blue4";
       // returnString = "color=\"Blue\" decorate labelfontcolor=\"blue4\"";
          returnString = "color=\"Blue\" decorate ";
        }

  // DQ (3/5/2007): color the scope edges green (that are explicitly stored in the AST)
  // SgScopeStatement* scopeStatement = isSgScopeStatement(to);
     SgStatement* statement = isSgStatement(from);
     if (statement != NULL)
        {
          if (statement->hasExplicitScope() == true && statement->get_scope() == to && label == "scope")
             {
            // returnString = "color=\"Green\" decorate labelfontcolor=green4";
               returnString = "color=\"Green\" decorate ";
             }
        }
     
     SgInitializedName* initializedName = isSgInitializedName(from);
     if (initializedName != NULL)
        {
          if (initializedName->get_scope() == to && label == "scope")
             {
            // returnString = "color=\"Green\" decorate labelfontcolor=green4";
               returnString = "color=\"Green\" decorate ";
             }
        }

  // returnString += "decorate labelfontcolor=red";
#endif
     return returnString;
   }


void
CustomMemoryPoolDOTGenerationData::visit(SgNode* node)
   {
     ROSE_ASSERT(node!= NULL);

  // printf ("CustomMemoryPoolDOTGenerationData::visit(): node = %p = %s \n",node,node->class_name().c_str());

     bool ignoreThisIRnode = false;
  // bool ignoreThisIRnode = true;
#if 0
     Sg_File_Info* fileInfo = node->get_file_info();
     if (fileInfo != NULL)
        {
          if (fileInfo->isFrontendSpecific() == true)
             {
            // visitedNodes.insert(node);  I don't think we have to do this, it will be added at the end of the function
               ignoreThisIRnode = true;
             }
        }
#endif

#if 1
  // If the node is to be skipped then set: ignoreThisIRnode = true
     if ( skipNodeList.find(node) != skipNodeList.end() )
          ignoreThisIRnode = true;
#endif

     if (visitedNodes.find(node) == visitedNodes.end() && ignoreThisIRnode == false )
        {
       // This node has not been visited previously (and should not be ignored)
          std::string nodeoption;
          std::string nodelabel = std::string("\\n") + typeid(*node).name();

       // These is both virtual function calls
          nodelabel  += additionalNodeInfo(node);
          nodeoption += additionalNodeOptions(node);

       // DQ (5/9/2006): Added skipNodeList to permit filtering of IR nodes
          if ( skipNodeList.find(node) == skipNodeList.end() )
             {
               dotrep.addNode(node,nodelabel,nodeoption);
             }

       // Now call Andres's mechanism to get the list of ALL pointers to IR nodes
          std::vector<std::pair<SgNode*,std::string> > listOfIRnodes = node->returnDataMemberPointers();
          std::vector<std::pair<SgNode*,std::string> >::iterator i = listOfIRnodes.begin();
          while (i != listOfIRnodes.end())
             {
            // Might we want to test if this is node previously ignored (a gnu compatability IR node)
            // and do something like include that IR node in the AST?
               SgNode* n             = (*i).first;
               std::string edgelabel = (*i).second;
#if 0
               if (isSgFunctionDeclaration(node) != NULL)
                  {
                    printf ("From: node = %p = %s  To: node = %p %s \n",node,node->class_name().c_str(),n,(n != NULL) ? n->class_name().c_str() : "NULL value");
                  }
#endif
               if ( n == NULL)
                  {
                    if ( skipNodeList.find(node) == skipNodeList.end() )
                       {
                         dotrep.addNullValue(node,"",edgelabel,"");
                       }
                  }
                 else
                  {
                 // Only build the edge if the node is not on the list of IR nodes to skip
                 // We can't use the visitedNodes list since just be cause we have visited
                 // the IR node does not imply that we should skip the edge (quite the opposent!)
                    if ( skipNodeList.find(n) == skipNodeList.end() )
                       {
                      // printf ("Edge is being output \n");
                         bool ignoreEdge = false;
#if 0
                         Sg_File_Info* fileInfo = n->get_file_info();
                         if (fileInfo != NULL)
                            {
                           // Check if this is a GNU compatability IR node generated just 
                           // to support frontend compatabilty but should be ignored.
                           // Since such IR nodes are ignored (not assembled into the DOT 
                           // graph, we want to ignore any edges to them as well.
                              ignoreEdge = fileInfo->isFrontendSpecific();
                            }
#endif

                         if (ignoreEdge == false)
                            {
                           // DQ (3/5/2007): Support for edge options
                           // These are both virtual function calls
                              std::string additionalEdgeOption = additionalEdgeOptions(node,n,edgelabel);
                              std::string additionalEdgeLabel  = additionalEdgeInfo(node,n,edgelabel);

                           // dotrep.addEdge ( node, edgelabel + additionalEdgeLabel, n, additionalEdgeOption + "dir=forward" );
                              dotrep.addEdge ( node, edgelabel + additionalEdgeLabel, n, additionalEdgeOption + "dir=forward" );
                           // printf ("Added edge using %p -> label = %s -> %p option = %s \n",node, (edgelabel + additionalEdgeLabel).c_str(), n, (additionalEdgeOption + "dir=forward").c_str());
                            }
                       }
                      else
                       {
                      // printf ("Edge is being skipped \n");
                       }
                  }

               i++;
             }
        }
       else
        {
       // Ignoring this IR node which has previously been visited (or added by the user as a colored IR node)
        }


  // Add this node to the list of visited nodes so that we will not traverse it again 
  // This allows for it to have been placed into this list previously as a colored IR node
  // and thus prevents it from showing up twice.
     visitedNodes.insert(node);
   }

#if 0
CustomMemoryPoolDOTGeneration::~CustomMemoryPoolDOTGeneration() 
   {
   }
#endif

void
CustomMemoryPoolDOTGeneration::addNode( NodeType n )
   {
  // printf ("Adding an node in CustomMemoryPoolDOTGeneration::addNode \n");
     DOTgraph.addNode(n);
   }

void
CustomMemoryPoolDOTGeneration::addEdge( EdgeType e )
   {
  // printf ("Adding an edge in CustomMemoryPoolDOTGeneration::addEdge \n");
     DOTgraph.addEdge(e);
   }

void
CustomMemoryPoolDOTGeneration::skipNode(SgNode* n)
   {
     DOTgraph.skipNode(n);
   }

void
CustomMemoryPoolDOTGeneration::skipEdge(EdgeType e)
   {
     DOTgraph.skipEdge(e);
   }

void
CustomMemoryPoolDOTGeneration::internalGenerateGraph( std::string filename )
   {
     DOTgraph.internalGenerateGraph(filename);
   }

void
CustomMemoryPoolDOTGeneration::edgeFilter(SgNode* nodeSource, SgNode* nodeSink, std::string edgeName )
   {
  // This function skips the representation of edges
     if (edgeName == "parent")
        {
          EdgeType edge(nodeSource,nodeSink,edgeName);
          skipEdge(edge);
        }

     if (edgeName == "scope")
        {
          EdgeType edge(nodeSource,nodeSink,edgeName);
          skipEdge(edge);
        }
#if 1
  // DQ (1/24/2007): Added skipNodeList to permit filtering of IR nodes, if the source node is not present 
  // then we don't have an edge to skip, if the source node is present and the sink node is to be skipped, 
  // then skip the edge.
  // printf ("CustomMemoryPoolDOTGeneration::edgeFilter(): testing edge from %p = %s to %p = %s \n",nodeSource,nodeSource->class_name().c_str(),nodeSink,nodeSink->class_name().c_str());
     if ( DOTgraph.skipNodeList.find(nodeSource) != DOTgraph.skipNodeList.end() )
        {
          if ( nodeSink != NULL && DOTgraph.skipNodeList.find(nodeSink) != DOTgraph.skipNodeList.end() )
             {
            // printf ("CustomMemoryPoolDOTGeneration::edgeFilter(): Skipping this edge since it is to a node that is skipped nodeSink = %p = %s \n",nodeSink,nodeSink->class_name().c_str());
               printf ("CustomMemoryPoolDOTGeneration::edgeFilter(): Skipping this edge since it is to a node that is skipped nodeSink = %p = %s \n",nodeSink,nodeSink->class_name().c_str());

            // ROSE_ASSERT(false);

            // skipEdge(nodeSource,nodeSink,edgeName);
               EdgeType edge(nodeSource,nodeSink,edgeName);
               skipEdge(edge);
             }
        }
#endif
#if 0
     printf ("Exiting as a test in CustomMemoryPoolDOTGeneration::edgeFilter() \n");
     ROSE_ASSERT(false);
#endif
   }

void
CustomMemoryPoolDOTGeneration::typeFilter(SgNode* node)
   {
  // This function skips the representation ot types and IR nodes associated with types
     if (isSgType(node) != NULL)
        {
          skipNode(node);
        }

     if (isSgFunctionParameterTypeList(node) != NULL)
        {
          skipNode(node);
        }

     if (isSgTypedefSeq(node) != NULL)
        {
          skipNode(node);
        }
   }

void
CustomMemoryPoolDOTGeneration::commentAndDirectiveFilter(SgNode* node)
   {
  // Skip the Sg_File_Info objects associated with comments

     SgLocatedNode* locatedNode = isSgLocatedNode(node);
     if (locatedNode != NULL)
        {
          AttachedPreprocessingInfoType* comments = locatedNode->getAttachedPreprocessingInfo();
          if (comments != NULL)
             {
            // printf ("Found attached comments (at %p of type: %s): \n",locatedNode,locatedNode->sage_class_name());
               AttachedPreprocessingInfoType::iterator i;
               for (i = comments->begin(); i != comments->end(); i++)
                  {
                    ROSE_ASSERT ( (*i) != NULL );
#if 0
                    printf ("          Attached Comment (relativePosition=%s): %s",
                         ((*i)->getRelativePosition() == PreprocessingInfo::before) ? "before" : "after",
                         (*i)->getString().c_str());
#endif
                    ROSE_ASSERT((*i)->get_file_info() != NULL);
                    skipNode((*i)->get_file_info());

                  }
             }
            else
             {
#if 0
               printf ("No attached comments (at %p of type: %s): \n",locatedNode,locatedNode->sage_class_name());
#endif
             }
        }
   }

void
CustomMemoryPoolDOTGeneration::fileInfoFilter(SgNode* node)
   {
  // Skip the Sg_File_Info objects associated with comments

     Sg_File_Info* fileInfoNode = isSg_File_Info(node);
     if (fileInfoNode != NULL)
        {
          skipNode(fileInfoNode);
        }
   }

void
CustomMemoryPoolDOTGeneration::frontendCompatabilityFilter(SgNode* node)
   {
  // DQ (5/11/2006): This filters out the gnu compatability IR nodes (which tend to confuse everyone!)

     ROSE_ASSERT(node != NULL);

#if 0
  // DQ (5/11/2006): Skip IR nodes that are explicitly marked as frontend specific
     Sg_File_Info* fileInfo = node->get_file_info();
     if (fileInfo != NULL)
        {
          if (fileInfo->isFrontendSpecific() == true)
             {
               skipNode(node);
             }
        }
#endif

  // Mark all SgFunctionSymbol IR nodes that are associated with the a frontend specific function
     SgFunctionSymbol* functionSymbol = isSgFunctionSymbol(node);
     if (functionSymbol != NULL)
        {
          SgDeclarationStatement* declaration = functionSymbol->get_declaration();
          ROSE_ASSERT(declaration != NULL);
          if (declaration->get_file_info() == NULL)
             {
               printf ("Error: declaration->get_file_info() == NULL declaration = %p = %s \n",declaration,declaration->class_name().c_str());
             }
          ROSE_ASSERT(declaration->get_file_info() != NULL);
          if (declaration != NULL && declaration->get_file_info()->isFrontendSpecific() == true)
             {
               skipNode(functionSymbol);
             }
        }

     SgTypedefSymbol* typedefSymbol = isSgTypedefSymbol(node);
     if (typedefSymbol != NULL)
        {
          SgDeclarationStatement* declaration = typedefSymbol->get_declaration();
          if (declaration->get_file_info()->isFrontendSpecific() == true)
             {
               skipNode(typedefSymbol);
             }
        }

     SgVariableSymbol* variableSymbol = isSgVariableSymbol(node);
     if (variableSymbol != NULL)
        {
          SgInitializedName* declaration = variableSymbol->get_declaration();
          ROSE_ASSERT(declaration != NULL);
          if (declaration->get_file_info()->isFrontendSpecific() == true)
             {
               skipNode(variableSymbol);
            // This is redundant
            // skipNode(declaration);
             }

          SgStorageModifier & storageModifier = declaration->get_storageModifier();
          skipNode(&storageModifier);
        }

     SgFunctionParameterList* functionParateterList = isSgFunctionParameterList(node);
     if (functionParateterList != NULL)
        {
          SgInitializedNamePtrList::iterator i = 	functionParateterList->get_args().begin();
          while (i != functionParateterList->get_args().end())
             {
               SgInitializedName* declaration = *i;
               ROSE_ASSERT(declaration != NULL);
               if (declaration->get_file_info()->isFrontendSpecific() == true)
                  {
                 // skipNode(declaration);
                    SgStorageModifier & storageModifier = declaration->get_storageModifier();
                    skipNode(&storageModifier);
                  }

               i++;
             }
        }

  // Skip the Sg_File_Info objects that are associated with frontend specific IR nodes.
     Sg_File_Info* fileInfo = node->get_file_info();
     if (fileInfo != NULL)
        {
          if (fileInfo->isFrontendSpecific() == true)
             {
               printf ("skip fileInfo = %p parent is %p = %s \n",fileInfo,node,node->class_name().c_str());
            // skipNode(fileInfo);
               skipNode(node);
             }
            else
             {
            // printf ("Node %p = %s has normal Sg_File_Info object with parent = %s \n",node,node->class_name().c_str(),(fileInfo->get_parent() != NULL) ? fileInfo->get_parent()->class_name().c_str() : "NULL value");
            // fileInfo->display("extranious Sg_File_Info object");
             }
        }

  // DQ (5/11/2006): Skip any IR nodes that are part of a gnu compatability specific subtree of the AST
     Sg_File_Info* currentNodeFileInfo = isSg_File_Info(node);
     if (currentNodeFileInfo != NULL)
        {
       // skipNode(currentNodeFileInfo);
       // if (currentNodeFileInfo->isFrontendSpecific() == true)
          if (false)
             {
               skipNode(currentNodeFileInfo);
             }
#if 1
            else
             {
               string additionalNodeOptions = "shape=polygon,regular=0,URL=\"\\N\",tooltip=\"more info at \\N\",sides=6,peripheries=1,color=\"Blue\",fillcolor=cyan4,fontname=\"7x13bold\",fontcolor=black,style=filled";
            // Make this statement different in the generated dot graph
               string labelWithSourceCode = "\\n" + currentNodeFileInfo->get_filenameString() + 
                                            "\\n" +  StringUtility::numberToString(currentNodeFileInfo) + "  ";

               NodeType graphNode(node,labelWithSourceCode,additionalNodeOptions);

               addNode(graphNode);
             }
#endif
        }

#if 0
     if (isSgType(node) != NULL)
        {
          string additionalNodeOptions = "shape=polygon,regular=0,URL=\"\\N\",tooltip=\"more info at \\N\",sides=3,peripheries=1,color=\"Blue\",fillcolor=yellow,fontname=\"7x13bold\",fontcolor=black,style=filled";
       // Make this statement different in the generated dot graph
          
          string labelWithSourceCode;
          if (isSgClassType(node) == NULL)
               labelWithSourceCode = string("\\n  ") + node->unparseToString() + "  ";

          NodeType graphNode(node,labelWithSourceCode,additionalNodeOptions);
       // addNode(graphNode);

       // skipNode(node);
        }
#endif

     if (isSgModifier(node) != NULL)
        {
       // skipNode(node);
        }

#if 0
     if (isSgSymbol(node) != NULL)
        {
          skipNode(node);
        }
#endif
#if 0
     if (isSg_File_Info(node) != NULL)
        {
          skipNode(node);
        }
#endif
     if (isSgTypedefSeq(node) != NULL)
        {
       // skipNode(node);
        }

     if (isSgFunctionParameterTypeList(node) != NULL)
        {
       // skipNode(node);
        }
   }


void
CustomMemoryPoolDOTGeneration::defaultFilter(SgNode* node)
   {
  // Default fileter to simplify the whole AST graph

  // printf ("Exiting in CustomMemoryPoolDOTGeneration::defaultFilter() \n");
  // ROSE_ASSERT(false);
#if 1
     frontendCompatabilityFilter(node);
#endif
     commentAndDirectiveFilter(node);

     std::vector<std::pair<SgNode*,std::string> > listOfIRnodes = node->returnDataMemberPointers();
     std::vector<std::pair<SgNode*,std::string> >::iterator i = listOfIRnodes.begin();
     while (i != listOfIRnodes.end())
        {
       // Might we want to test if this is node previously ignored (a gnu compatability IR node)
       // and do something like include that IR node in the AST?
          SgNode* n             = (*i).first;
          std::string edgelabel = (*i).second;

       // Run all the edges through the filter (the filter will single out the ones to be skipped)
          edgeFilter(node,n,edgelabel);

          i++;
        }
   }

void
CustomMemoryPoolDOTGeneration::defaultColorFilter(SgNode* node)
   {
     ROSE_ASSERT(node != NULL);

  // DQ (12/15/2007): I think this file is no longer used!
     printf ("This code is not used! \n");
     ROSE_ASSERT(false);

     SgStatement* statement = isSgStatement(node);
     if (statement != NULL)
        {
          string additionalNodeOptions;

       // Make this statement different in the generated dot graph
          string labelWithSourceCode;

          switch(statement->variantT())
             {
               case V_SgFunctionDeclaration:
               case V_SgProgramHeaderStatement:
               case V_SgProcedureHeaderStatement:
               case V_SgMemberFunctionDeclaration:
               case V_SgTemplateInstantiationFunctionDecl:
               case V_SgTemplateInstantiationMemberFunctionDecl:
                  {
                    SgFunctionDeclaration* functionDeclaration = isSgFunctionDeclaration(node);
                    additionalNodeOptions = "shape=polygon,regular=0,URL=\"\\N\",tooltip=\"more info at \\N\",sides=5,peripheries=1,color=\"Blue\",fillcolor=DeepSkyBlue,fontname=\"7x13bold\",fontcolor=black,style=filled";
                    labelWithSourceCode = string("\\n  ") + functionDeclaration->get_name().getString() +
                                          "\\n  " +  StringUtility::numberToString(functionDeclaration) + "  ";
                 // printf ("########## functionDeclaration->get_name() = %s \n",functionDeclaration->get_name().str());
                    break;
                  }

               case V_SgTemplateDeclaration:
                  {
                    SgTemplateDeclaration* templateDeclaration = isSgTemplateDeclaration(node);
                    additionalNodeOptions = "shape=polygon,regular=0,URL=\"\\N\",tooltip=\"more info at \\N\",sides=8,peripheries=2,color=\"Blue\",fillcolor=green,fontname=\"7x13bold\",fontcolor=black,style=filled";
                    labelWithSourceCode = "\\n  " + templateDeclaration->get_name().getString() + 
                                          "\\n  " +  StringUtility::numberToString(templateDeclaration) + "  ";
                    break;
                  }

               case V_SgClassDeclaration:
               case V_SgTemplateInstantiationDecl:
                  {
                    SgClassDeclaration* classDeclaration = isSgClassDeclaration(node);
                    additionalNodeOptions = "shape=polygon,regular=0,URL=\"\\N\",tooltip=\"more info at \\N\",sides=8,peripheries=2,color=\"Blue\",fillcolor=peru,fontname=\"7x13bold\",fontcolor=black,style=filled";
                    labelWithSourceCode = "\\n  " + classDeclaration->get_name().getString() + 
                                          "\\n  " +  StringUtility::numberToString(classDeclaration) + "  ";
                 // printf ("########## classDeclaration->get_name() = %s \n",classDeclaration->get_name().str());
                    break;
                  }

               case V_SgTypedefDeclaration:
                  {
                    SgTypedefDeclaration* typedefDeclaration = isSgTypedefDeclaration(node);
                    additionalNodeOptions = "shape=polygon,regular=0,URL=\"\\N\",tooltip=\"more info at \\N\",sides=8,peripheries=2,color=\"Blue\",fillcolor=peru,fontname=\"7x13bold\",fontcolor=black,style=filled";
                    labelWithSourceCode = "\\n  " + typedefDeclaration->get_name().getString() + 
                                          "\\n  " +  StringUtility::numberToString(typedefDeclaration) + "  ";
                    break;
                  }

               case V_SgEnumDeclaration:
                  {
                    SgEnumDeclaration* enumDeclaration = isSgEnumDeclaration(node);
                    additionalNodeOptions = "shape=polygon,regular=0,URL=\"\\N\",tooltip=\"more info at \\N\",sides=8,peripheries=2,color=\"Blue\",fillcolor=green,fontname=\"7x13bold\",fontcolor=black,style=filled";
                    labelWithSourceCode = "\\n  " + enumDeclaration->get_name().getString() + 
                                          "\\n  " +  StringUtility::numberToString(enumDeclaration) + "  ";
                    break;
                  }

               default:
                  {
                 // It appears that we can't unparse one of these (not implemented)
                    additionalNodeOptions = "shape=polygon,regular=0,URL=\"\\N\",tooltip=\"more info at \\N\",sides=5,peripheries=1,color=\"Blue\",fillcolor=blue2,fontname=\"7x13bold\",fontcolor=black,style=filled";
                 // labelWithSourceCode = string("\\n  ") + functionDeclaration->get_name().getString() + "  ";
                    labelWithSourceCode = string("\\n  ") + StringUtility::numberToString(node) + "  ";
#if 0
                 // DQ (5/14/2006): this is an error when processing stdio.h
                 // DQ (5/14/2006): This fails for SgClassDeclaration
                 // if (isSgVariableDefinition(statement) == NULL)
                    if ( (isSgVariableDefinition(statement) == NULL) && (isSgClassDeclaration(node) == NULL) )
                         labelWithSourceCode = string("\\n  ") + node->unparseToString() + "  ";
#endif
                    break;
                  }
             }

          NodeType graphNode(node,labelWithSourceCode,additionalNodeOptions);
          addNode(graphNode);
        }

     if (isSgExpression(node) != NULL)
        {
          string additionalNodeOptions = "shape=polygon,regular=0,URL=\"\\N\",tooltip=\"more info at \\N\",sides=6,peripheries=1,color=\"Blue\",fillcolor=green,fontname=\"7x13bold\",fontcolor=black,style=filled";
       // Make this statement different in the generated dot graph

       // DQ (5/14/2006): this is an error when processing stdio.h
       // string labelWithSourceCode = string("\\n  ") + node->unparseToString() + "  ";
          string labelWithSourceCode;
          labelWithSourceCode = string("\\n  ") + StringUtility::numberToString(node) + "  ";

          SgValueExp* valueExp = isSgValueExp(node);
          if (valueExp != NULL)
             {
               if (valueExp->get_parent() == NULL)
                  {
                    printf ("Error: valueExp = %p valueExp->get_parent() == NULL \n",valueExp);
                    valueExp->get_file_info()->display("Error: valueExp->get_parent() == NULL");
                  }
               ROSE_ASSERT(valueExp->get_parent() != NULL);
            // labelWithSourceCode = "\\n value = " + valueExp->unparseToString() + "\\n" + StringUtility::numberToString(node) + "  ";
            // labelWithSourceCode = string("\\n value = nnn") + "\\n" + StringUtility::numberToString(node) + "  ";
             }

          NodeType graphNode(node,labelWithSourceCode,additionalNodeOptions);
          addNode(graphNode);
        }

     if (isSgType(node) != NULL)
        {
          string additionalNodeOptions = "shape=polygon,regular=0,URL=\"\\N\",tooltip=\"more info at \\N\",sides=3,peripheries=1,color=\"Blue\",fillcolor=yellow,fontname=\"7x13bold\",fontcolor=black,style=filled";

       // Make this statement different in the generated dot graph
       // string labelWithSourceCode = string("\\n  ") + node->unparseToString() + "  ";
          string labelWithSourceCode;
       // printf ("Graph this node (%s) \n",node->class_name().c_str());
          if (isSgClassType(node) == NULL)
             {
            // printf ("Graph this node (%s) \n",node->class_name().c_str());
            // labelWithSourceCode = string("\\n  ") + node->unparseToString() + "  ";
             }

       // labelWithSourceCode = string("\\n  ") + StringUtility::numberToString(node) + "  ";
          labelWithSourceCode = string("\\n  ") + StringUtility::numberToString(node) + "  ";

          NodeType graphNode(node,labelWithSourceCode,additionalNodeOptions);
          addNode(graphNode);
        }

     SgSupport* supportNode = isSgSupport(node);
     if (supportNode != NULL)
        {
          string additionalNodeOptions;

       // Make this statement different in the generated dot graph
          string labelWithSourceCode;

          switch(supportNode->variantT())
             {
               case V_SgProject:
                  {
                    SgProject* project = isSgProject(node);
                    additionalNodeOptions = "shape=ellipse,regular=0,URL=\"\\N\",tooltip=\"more info at \\N\",sides=5,peripheries=3,color=\"Blue\",fillcolor=pink,fontname=\"7x13bold\",fontcolor=black,style=filled";
                    labelWithSourceCode = string("\\n  ") + project->get_outputFileName() + "  ";
                 // printf ("########## SgProject = %s \n",project->get_outputFileName().c_str());
                    break;
                  }

               case V_SgFile:
                  {
                    SgFile* file = isSgFile(node);
                    additionalNodeOptions = "shape=ellipse,regular=0,URL=\"\\N\",tooltip=\"more info at \\N\",sides=5,peripheries=2,color=\"Blue\",fillcolor=pink,fontname=\"7x13bold\",fontcolor=black,style=filled";
                    labelWithSourceCode = string("\\n  ") + file->get_sourceFileNameWithPath() + "  ";
                 // printf ("########## SgFile = %s \n",file->get_sourceFileNameWithPath().c_str());
                    ROSE_ASSERT(SgNode::get_globalFunctionTypeTable() != NULL);
                 // printf ("     SgNode::SgFunctionTypeTable = %p size = %ld \n",SgNode::get_globalFunctionTypeTable(),SgNode::get_globalFunctionTypeTable()->get_function_type_table()->size());
                    break;
                  }

               case V_Sg_File_Info:
                  {
                 // DQ (5/11/2006): Skip any IR nodes that are part of a gnu compatability specific subtree of the AST
                    Sg_File_Info* currentNodeFileInfo = isSg_File_Info(node);
                    if (currentNodeFileInfo != NULL)
                       {
                      // skipNode(currentNodeFileInfo);
                         if (currentNodeFileInfo->isFrontendSpecific() == true)
                            {
                           // skipNode(currentNodeFileInfo);
                            }
                           else
                            {
                           // additionalNodeOptions = "shape=polygon,regular=0,URL=\"\\N\",tooltip=\"more info at \\N\",sides=6,peripheries=1,color=\"Blue\",fillcolor=cyan4,fontname=\"7x13bold\",fontcolor=black,style=filled";
                              additionalNodeOptions = "shape=polygon,regular=0,URL=\"\\N\",tooltip=\"more info at \\N\",sides=6,peripheries=1,color=\"Blue\",fillcolor=brown,fontname=\"7x13bold\",fontcolor=black,style=filled";
                           // Make this statement different in the generated dot graph
                              labelWithSourceCode = 
                                 "\\n" + currentNodeFileInfo->get_filenameString() + 
                                 "\\n line: " +  StringUtility::numberToString(currentNodeFileInfo->get_line()) + 
                                 " column: " +  StringUtility::numberToString(currentNodeFileInfo->get_col()) + "  " +
                                 "\\n raw line: " +  StringUtility::numberToString(currentNodeFileInfo->get_raw_line()) + 
                                 " raw column: " +  StringUtility::numberToString(currentNodeFileInfo->get_raw_col()) + "  " +
                                 "\\n pointer value: " +  StringUtility::numberToString(currentNodeFileInfo) + "  ";
                            }
                       }
                    break;
                  }

               case V_SgInitializedName:
                  {
                    SgInitializedName* initializedName = isSgInitializedName(node);
                    additionalNodeOptions = "shape=house,regular=0,URL=\"\\N\",tooltip=\"more info at \\N\",sides=5,peripheries=1,color=\"Blue\",fillcolor=darkturquoise,fontname=\"7x13bold\",fontcolor=black,style=filled";
                    labelWithSourceCode = string("\\n  ") + initializedName->get_name().getString() +
                                          string("\\n  ") + StringUtility::numberToString(initializedName) + "  ";
                 // printf ("########## initializedName->get_name() = %s \n",initializedName->get_name().str());
                    break;
                  }

               case V_SgSymbolTable:
                  {
                    SgSymbolTable* symbolTable = isSgSymbolTable(node);
                    additionalNodeOptions = "shape=house,regular=0,URL=\"\\N\",tooltip=\"more info at \\N\",sides=5,peripheries=1,color=\"Blue\",fillcolor=orange,fontname=\"7x13bold\",fontcolor=black,style=filled";
                    string symbolTableName = "Unnamed Symbol Table";
                    if (symbolTable->get_no_name() == true)
                         symbolTableName = symbolTable->get_name().getString();
                    labelWithSourceCode = string("\\n  ") + symbolTableName +
                                          string("\\n  ") + StringUtility::numberToString(symbolTable) + "  ";
                 // printf ("########## symbolTable->get_name() = %s \n",symbolTable->get_name().str());
                    break;
                  }

               case V_SgTypedefSeq:
                  {
                    SgTypedefSeq* typedefSeq = isSgTypedefSeq(node);
                    additionalNodeOptions = "shape=house,regular=0,URL=\"\\N\",tooltip=\"more info at \\N\",sides=5,peripheries=1,color=\"Blue\",fillcolor=red,fontname=\"7x13bold\",fontcolor=black,style=filled";
                    labelWithSourceCode = string("\\n  ") + StringUtility::numberToString(typedefSeq) + "  ";
                 // printf ("########## typedefSeq->get_name() = %s \n",typedefSeq->get_name().str());
                    break;
                  }

               case V_SgTemplateArgument:
                  {
                    SgTemplateArgument* templateArgument = isSgTemplateArgument(node);
                    additionalNodeOptions = "shape=circle,regular=0,URL=\"\\N\",tooltip=\"more info at \\N\",sides=5,peripheries=1,color=\"Blue\",fillcolor=yellow,fontname=\"7x13bold\",fontcolor=black,style=filled";
                    labelWithSourceCode = string("\\n  ") + StringUtility::numberToString(templateArgument) + "  ";
                    break;
                  }

               default:
                  {
                    additionalNodeOptions = "shape=polygon,regular=0,URL=\"\\N\",tooltip=\"more info at \\N\",sides=4,peripheries=1,color=\"Blue\",fillcolor=red,fontname=\"7x13bold\",fontcolor=black,style=filled";
                 // labelWithSourceCode;
                    break;
                  }
             }

          NodeType graphNode(node,labelWithSourceCode,additionalNodeOptions);
          addNode(graphNode);
        }

#if 1
     if (isSgSymbol(node) != NULL)
        {
          string additionalNodeOptions = "shape=polygon,regular=0,URL=\"\\N\",tooltip=\"more info at \\N\",sides=8,peripheries=1,color=\"Blue\",fillcolor=purple,fontname=\"7x13bold\",fontcolor=black,style=filled";
       // Make this statement different in the generated dot graph
       // string labelWithSourceCode = string("\\n  ") + node->unparseToString() + "  ";
          string labelWithSourceCode;

          labelWithSourceCode = string("\\n  ") + StringUtility::numberToString(node) + "  ";
          NodeType graphNode(node,labelWithSourceCode,additionalNodeOptions);

          addNode(graphNode);
        }
#endif

  // DQ (3/25/2007): Added support for graphs including SgToken IR nodes.
     if (isSgToken(node) != NULL)
        {
          SgToken* token = isSgToken(node);
          string additionalNodeOptions = "shape=polygon,regular=0,URL=\"\\N\",tooltip=\"more info at \\N\",sides=8,peripheries=1,color=\"Blue\",fillcolor=moccasin,fontname=\"7x13bold\",fontcolor=black,style=filled";
       // Make this statement different in the generated dot graph
       // string labelWithSourceCode = string("\\n  ") + node->unparseToString() + "  ";
          string labelWithSourceCode;

          std::string token_classification_string = SgToken::ROSE_Fortran_keyword_map [token->get_classification_code()].token_lexeme;
          labelWithSourceCode = string("\\n  ") + token->get_lexeme_string() + "  " + 
                                string("\\n  ") + token_classification_string + "  " + 
                                string("\\n  ") + StringUtility::numberToString(node) + "  ";

          NodeType graphNode(node,labelWithSourceCode,additionalNodeOptions);

          addNode(graphNode);
        }



#if 0
  // DQ (3/5/2007): Mark the parent edge in a different color, this does not appear to work!!!!
     if (node->get_parent() != NULL)
        {
          printf ("Adding a colored edge to the parent from = %p to = %p \n",node,node->get_parent());
          string additionalEdgeOptions = "color=\"Blue\" decorate labelfontcolor=\"Red\"";
          EdgeType graphEdge(node,node->get_parent(),"",additionalEdgeOptions);
          addEdge(graphEdge);
        }
#endif
#if 0
  // DQ (3/6/2007): In general it is difficult to specify edges when all we have is the IR node

  // DQ (3/5/2007): color the parent edges blue
     if (node->get_parent() != NULL)
        {
          returnString = "color=\"Blue\"";
        }

  // DQ (3/5/2007): color the scope edges green (that are explicitly stored in the AST)
  // SgScopeStatement* scopeStatement = isSgScopeStatement(to);
     SgStatement* statement = isSgStatement(node);
     if (statement != NULL)
        {
          if (statement->hasExplicitScope() == true && statement->get_scope() == to)
             {
               returnString = "color=\"Green\"";
             }
        }
     
     SgInitializedName* initializedName = isSgInitializedName(from);
     if (initializedName != NULL)
        {
          if (initializedName->get_scope() == to)
             {
               returnString = "color=\"Green\"";
             }
        }

     SgType* type = isSgType(to);
     if (type != NULL)
        {
          returnString = "color=\"Yellow\"";
        }
#endif
   }






class SimpleColorMemoryPoolTraversal
   : public CustomMemoryPoolDOTGeneration,
     public ROSE_VisitTraversal
   {
     public:
       // MangledNameMapTraversal::SetOfNodesType     & setOfIRnodes;
          static const set<SgNode*> defaultSetOfIRnodes;
          const set<SgNode*> & setOfIRnodes;

      //! Required traversal function
          void visit (SgNode* node);

          static void generateGraph( string filename, const set<SgNode*> & firstAST = defaultSetOfIRnodes );

          void markFirstAST();
          void buildExcludeList();

          SimpleColorMemoryPoolTraversal(const set<SgNode*> & s = defaultSetOfIRnodes ) : setOfIRnodes(s) {};

       // Destructor defined because base class has virtual members
       // virtual ~SimpleColorMemoryPoolTraversal();
          virtual ~SimpleColorMemoryPoolTraversal();
   };

const set<SgNode*> SimpleColorMemoryPoolTraversal::defaultSetOfIRnodes;

#if 1
SimpleColorMemoryPoolTraversal::~SimpleColorMemoryPoolTraversal()
   {
   }
#endif

void
SimpleColorMemoryPoolTraversal::generateGraph(string filename, const set<SgNode*> & firstAST)
   {
  // DQ (2/2/2007): Introduce tracking of performance of within AST merge
     std::string label =  "SimpleColorMemoryPoolTraversal::generateGraph(" + filename + "):";
     TimingPerformance timer (label);

  // Custom control over the coloring of the "whole" AST for a memory pool traversal
     SimpleColorMemoryPoolTraversal traversal(firstAST);
     traversal.traverseMemoryPool();

  // DQ (1/20/2007): We want this to be an exclude mechanism
  // traversal.markFirstAST();
     traversal.buildExcludeList();

     traversal.internalGenerateGraph(filename);
   }

void
SimpleColorMemoryPoolTraversal::markFirstAST ()
   {
  // Mark the IR node that are in the first AST before the deep AST copy
     set<SgNode*>::iterator i = setOfIRnodes.begin();
     while (i != setOfIRnodes.end())
        {
          string additionalNodeOptions = "shape=polygon,regular=0,URL=\"\\N\",tooltip=\"more info at \\N\",sides=6,peripheries=1,color=\"Blue\",fillcolor=peru,fontname=\"7x13bold\",fontcolor=black,style=filled";
       // Make this statement different in the generated dot graph

       // DQ (5/14/2006): this is an error when processing stdio.h
       // string labelWithSourceCode = string("\\n  ") + node->unparseToString() + "  ";
          string labelWithSourceCode = "\\n" +  StringUtility::numberToString(*i) + "  ";

          NodeType graphNode(*i,labelWithSourceCode,additionalNodeOptions);
          addNode(graphNode);
#if 0
          additionalNodeOptions = "shape=polygon,regular=0,URL=\"\\N\",tooltip=\"more info at \\N\",sides=6,peripheries=1,color=\"Blue\",fillcolor=peru,fontname=\"7x13bold\",fontcolor=white,style=filled";
       // Make this statement different in the generated dot graph

       // DQ (5/14/2006): this is an error when processing stdio.h
       // string labelWithSourceCode = string("\\n  ") + node->unparseToString() + "  ";
          labelWithSourceCode = "\\n" +  StringUtility::numberToString(*i) + "  ";

          NodeType graphNode2(*i,labelWithSourceCode,additionalNodeOptions);
          addNode(graphNode2);
#endif
          i++;
        }
   }

void
SimpleColorMemoryPoolTraversal::buildExcludeList ()
   {
  // Mark the IR node that are in the first AST before the deep AST copy
     set<SgNode*>::iterator i = setOfIRnodes.begin();
     while (i != setOfIRnodes.end())
        {
#if 0
          printf ("In SimpleColorMemoryPoolTraversal::buildExcludeList(): skipping node = %p = %s = %s \n",*i,(*i)->class_name().c_str(),SageInterface::get_name(*i).c_str());
#endif
          skipNode(*i);
          i++;
        }
   }

void
SimpleColorMemoryPoolTraversal::visit(SgNode* node)
   {
#if 0
     defaultFilter(node);
#endif
#if 0
     typeFilter(node);
#endif
#if 1
     defaultColorFilter(node);
#endif
#if 1
  // Ignore Sg_File_Info objects associated with comments and CPP directives
     commentAndDirectiveFilter(node);
#endif
#if 0
  // Control output of Sg_File_Info object in graph of whole AST.
     fileInfoFilter(node);
#endif
#if 1
  // DQ (5/11/2006): Skip any IR nodes that are part of a gnu compatability specific subtree of the AST
     Sg_File_Info* currentNodeFileInfo = isSg_File_Info(node);
     if (currentNodeFileInfo != NULL)
        {
       // skipNode(currentNodeFileInfo);
       // if (currentNodeFileInfo->isFrontendSpecific() == true)
          if (false)
             {
           // skipNode(currentNodeFileInfo);
             }
#if 1
            else
             {
               string additionalNodeOptions;
            // string additionalNodeOptions = "shape=polygon,regular=0,URL=\"\\N\",tooltip=\"more info at \\N\",sides=6,peripheries=1,color=\"Blue\",fillcolor=cyan4,fontname=\"7x13bold\",fontcolor=black,style=filled";
            // Make this statement different in the generated dot graph
               string labelWithSourceCode = 
                                 "\\n" + currentNodeFileInfo->get_filenameString() + 
                                 "\\n line: " +  StringUtility::numberToString(currentNodeFileInfo->get_line()) + 
                                 " column: " +  StringUtility::numberToString(currentNodeFileInfo->get_col()) + "  " +
                                 "\\n pointer value: " +  StringUtility::numberToString(currentNodeFileInfo) + "  ";

               NodeType graphNode(node,labelWithSourceCode,additionalNodeOptions);

               addNode(graphNode);
             }
#endif
        }
#endif

  // Color this IR node differently if it in NOT in the original AST
     if (setOfIRnodes.find(node) == setOfIRnodes.end())
        {
          string additionalNodeOptions = "shape=polygon,regular=0,URL=\"\\N\",tooltip=\"more info at \\N\",sides=6,peripheries=1,color=\"Blue\",fillcolor=greenyellow,fontname=\"7x13bold\",fontcolor=black,style=filled";
       // Make this statement different in the generated dot graph

          string labelWithSourceCode = "\\n" +  StringUtility::numberToString(node) + "  ";

          NodeType graphNode(node,labelWithSourceCode,additionalNodeOptions);
          addNode(graphNode);
        }
   }



// Build an inherited attribute for the tree traversal to test the rewrite mechanism
class SimpleColorFilesInheritedAttribute
   {
     public:
      //! Max number of different types of nodes that we define
          static const int maxSize;
          static const std::string additionalNodeOptionsArray[5];

       // Current options to use with DOT visualizations
          string currentNodeOptions;

       // True if subtree is shared
          bool sharedSubTree;

          void setNodeOptions(SgFile* file);

       // Default constructor
          SimpleColorFilesInheritedAttribute () : sharedSubTree(false)
             {
               ROSE_ASSERT(currentNodeOptions.size() < 4000);
             }

      //! Specific constructors are required
      //  SimpleColorFilesInheritedAttribute () {};
      //  SimpleColorFilesInheritedAttribute ( const SimpleColorFilesInheritedAttribute & X ) {};
   };

const int SimpleColorFilesInheritedAttribute::maxSize = 5;

// colors that will be represented as "black": azure1
const string SimpleColorFilesInheritedAttribute::additionalNodeOptionsArray[SimpleColorFilesInheritedAttribute::maxSize] = 
   { "shape=ellipse,regular=0,URL=\"\\N\",tooltip=\"more info at \\N\",sides=6,peripheries=1,color=\"Blue\",fillcolor=blue1,fontname=\"7x13bold\",fontcolor=black,style=filled",
     "shape=ellipse,regular=0,URL=\"\\N\",tooltip=\"more info at \\N\",sides=6,peripheries=1,color=\"Blue\",fillcolor=greenyellow,fontname=\"7x13bold\",fontcolor=black,style=filled",
     "shape=ellipse,regular=0,URL=\"\\N\",tooltip=\"more info at \\N\",sides=6,peripheries=1,color=\"Blue\",fillcolor=red,fontname=\"7x13bold\",fontcolor=black,style=filled",
     "shape=ellipse,regular=0,URL=\"\\N\",tooltip=\"more info at \\N\",sides=6,peripheries=1,color=\"Blue\",fillcolor=gold1,fontname=\"7x13bold\",fontcolor=black,style=filled",
     "shape=ellipse,regular=0,URL=\"\\N\",tooltip=\"more info at \\N\",sides=6,peripheries=1,color=\"Blue\",fillcolor=green,fontname=\"7x13bold\",fontcolor=black,style=filled"
   };


void
SimpleColorFilesInheritedAttribute::setNodeOptions(SgFile* file)
   {
     ROSE_ASSERT(file->get_file_info() != NULL);

  // DQ (3/22/2007): file_id's can be negative for defalut generated files (NULL_FILE, Transforamtions, etc.)
     int file_id = file->get_file_info()->get_file_id();
     int index = 0;
     if (file_id > 0)
        {
       // Select from a small subset of optional setting strings.
          index = (file->get_file_info()->get_file_id() + 0) % maxSize;
        }
  // printf ("SimpleColorFilesInheritedAttribute::setNodeOptions(): index = %d \n",index);
     currentNodeOptions = additionalNodeOptionsArray[index];
     ROSE_ASSERT(currentNodeOptions.size() < 4000);

#if 0
     printf ("SimpleColorFilesInheritedAttribute::setNodeOptions(): index = %d \n",index);
     file->get_file_info()->display("SimpleColorFilesInheritedAttribute::setNodeOptions()");
     printf ("currentNodeOptions = %s \n",currentNodeOptions.c_str());
#endif
   }

class SimpleColorFilesTraversal
// : public CustomMemoryPoolDOTGeneration,
   : public CustomAstDOTGeneration,
     public SgTopDownProcessing<SimpleColorFilesInheritedAttribute>
   {
     public:
       // Use this as an empty list to support the default argument option to generateGraph
          static set<SgNode*> emptyNodeList;

       // Reference to list that will be colored differently
          set<SgNode*> & specialNodeList;

      //! Required traversal function
          SimpleColorFilesInheritedAttribute evaluateInheritedAttribute (
             SgNode* astNode,
             SimpleColorFilesInheritedAttribute inheritedAttribute );

          static void generateGraph( SgProject* project, string filename, set<SgNode*> & specialNodeList = SimpleColorFilesTraversal::emptyNodeList );

       // Constructor
          SimpleColorFilesTraversal(set<SgNode*> & s) : specialNodeList(s) {}

       // Destructor defined because base class has virtual members
          virtual ~SimpleColorFilesTraversal() {}
   };

// Use this as an empty list to support the default argument option to generateGraph
set<SgNode*> SimpleColorFilesTraversal::emptyNodeList;

// Functions required by the tree traversal mechanism
SimpleColorFilesInheritedAttribute
SimpleColorFilesTraversal::evaluateInheritedAttribute (
     SgNode* node,
     SimpleColorFilesInheritedAttribute inheritedAttribute )
   {
     ROSE_ASSERT(node != NULL);
  // printf ("SimpleColorFilesTraversal::evaluateInheritedAttribute(): node = %s specialNodeList.size() = %ld \n",node->class_name().c_str(),specialNodeList.size());
     ROSE_ASSERT(inheritedAttribute.currentNodeOptions.size() < 4000);

     SgProject* project = isSgProject(node);
     if (project != NULL)
        {
          inheritedAttribute.currentNodeOptions =
             "shape=circle,regular=0,URL=\"\\N\",tooltip=\"more info at \\N\",sides=6,peripheries=3,color=\"Blue\",fillcolor=cyan4,fontname=\"12x24bold\",fontcolor=black,style=filled";
        }

     ROSE_ASSERT(inheritedAttribute.currentNodeOptions.size() < 4000);
     SgFile* file = isSgFile(node);
     if (file != NULL)
        {
          inheritedAttribute.setNodeOptions(file);
        }

     ROSE_ASSERT(inheritedAttribute.currentNodeOptions.size() < 4000);
     Sg_File_Info* fileInfo = node->get_file_info();
     if (fileInfo != NULL)
        {
          if (fileInfo->isShared() == true)
               inheritedAttribute.sharedSubTree = true;
        }

     ROSE_ASSERT(inheritedAttribute.currentNodeOptions.size() < 4000);
     if (inheritedAttribute.sharedSubTree == true)
        {
          inheritedAttribute.currentNodeOptions =
             "shape=diamond,regular=0,URL=\"\\N\",tooltip=\"more info at \\N\",sides=6,peripheries=1,color=\"Blue\",fillcolor=magenta1,fontname=\"12x24bold\",fontcolor=black,style=filled";
        }
     ROSE_ASSERT(inheritedAttribute.currentNodeOptions.size() < 4000);

     string additionalNodeOptions = inheritedAttribute.currentNodeOptions;
  // string labelWithSourceCode   = "\\n" +  StringUtility::numberToString(node) + "  ";
     string labelWithSourceCode;

     SgClassDeclaration* classDeclaration = isSgClassDeclaration(node);
     if (classDeclaration != NULL)
        {
          labelWithSourceCode   = "\\n" +  classDeclaration->get_name().getString() + "\\n" +  StringUtility::numberToString(node) + "  ";
        }
       else
        {
          SgFunctionDeclaration* functionDeclaration = isSgFunctionDeclaration(node);
          if (functionDeclaration != NULL)
             {
               labelWithSourceCode   = "\\n" +  functionDeclaration->get_name().getString() + "\\n" +  StringUtility::numberToString(node) + "  ";
             }
            else
             {
               labelWithSourceCode   = "\\n" +  StringUtility::numberToString(node) + "  ";
             }
        }
     ROSE_ASSERT(inheritedAttribute.currentNodeOptions.size() < 4000);

  // Check if this is an IR node that is to be handled specially
     if (specialNodeList.find(node) != specialNodeList.end())
        {
          inheritedAttribute.currentNodeOptions =
             "shape=circle,regular=0,URL=\"\\N\",tooltip=\"more info at \\N\",sides=6,peripheries=3,color=\"Blue\",fillcolor=black,fontname=\"12x24bold\",fontcolor=white,style=filled";
          ROSE_ASSERT(inheritedAttribute.currentNodeOptions.size() < 4000);
        }
     ROSE_ASSERT(inheritedAttribute.currentNodeOptions.size() < 4000);

     additionalNodeOptions = inheritedAttribute.currentNodeOptions;
     ROSE_ASSERT(additionalNodeOptions.size() < 4000);

     NodeType graphNode(node,labelWithSourceCode,additionalNodeOptions);
     ROSE_ASSERT(graphNode.optionString.size() < 4000);
     addNode(graphNode);

     return inheritedAttribute;
   }

void
SimpleColorFilesTraversal::generateGraph(SgProject* project, string filename, set<SgNode*> & specialNodeList )
   {
  // DQ (2/2/2007): Introduce tracking of performance of within AST merge
     std::string label =  "SimpleColorFilesTraversal::generateGraph(" + filename + "):";
     TimingPerformance timer (label);

  // Custom control over the coloring of the "whole" AST for a memory pool traversal
     SimpleColorFilesTraversal traversal(specialNodeList);
     SimpleColorFilesInheritedAttribute ih;
     traversal.traverse(project,ih);

     traversal.internalGenerateGraph(filename,project);
   }

// endif for COLOR_TRAVERSAL_H
#endif
