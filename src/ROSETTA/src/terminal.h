#ifndef __TERMINAL_H__
#define __TERMINAL_H__

#include "constraintList.h"

#include "ROSETTA_macros.h"

#include <string>
#include "string_functions.h"

// #include <list.h>
// #ifndef STL_LIST_IS_BROKEN
// #include STL_LIST_HEADER_FILE
// #endif

class GrammarTreeNode;
class Grammar;
class ConstraintListType;
class GrammarString;


// #ifdef __GNUG__
// typedef bool Boolean;
// #endif

class Terminal
{
  // Later we will template this class to permit it to refer to an arbitrary lower level grammar

  // Terminals for the basis for the definition of a grammar that defines a language
  // This class represents a terminal (the NonTerminal class is derived from the Terminal class)

 public:

                 // ????? (we don't use ths any more!)
                 // We have multiple variants depending on if the tokenString
                 // is a valid pointer or if the sageToken is a valid pointer.
                 // int variant;

                 // String representing the lexeam
  std::string   name;    // The name we assign to the token (e.g. "BinaryOperatorEquals")

  // where the name of the terminal is saved unmodified (with no grammar suffix)
  std::string baseName;
		   
  // The lexeme is used to build up the character string as the different operators 
  // are evaluated.  The when the operator| is called the lexeme is placed into the 
  // name (effectively naming the nonterminal).
  std::string   lexeme;  // The actual character string (e.g. "operator=")

  // Tag used within enum declaration (to that each terminal can be assigned an ID
  std::string   tag;

  // Link back to the tree representing the grammar (or a subTree within that grammar)
  GrammarTreeNode* grammarSubTree;

  // Relationship in lower level grammar (value == NULL only if a part of the C++ grammar)
  // This needs to be recorded into the classes that are build as well (I think)
  // We want to automatically set these (not sure if we can!)
  Terminal* lowerLevelGramaticalElement;

  bool buildDefaultConstructor;
  bool includeInitializerInDataStrings;

  // When a class is declared to implement the Terminal object we must 
  // provide a mechanism for the declaration of class forwards (pre-declaration) 
  // and global variables (post-delaration) associated with the declaration 
  // of the class representing this terminal (it's object).
  std::string predeclarationString;
  std::string postdeclarationString;

  // Controls automatic generation of Constructors (using
  // declared data as input parameters) and Descructors
  bool automaticGenerationOfDestructor;
  bool automaticGenerationOfConstructor;
  bool automaticGenerationOfDataAccessFunctions;
  bool automaticGenerationOfCopyFunction;

  // Record the grammar that this terminal is associated with!
  Grammar* associatedGrammar;

  // Later this will be a list of contraints
  ConstraintListType constraintList;

  // list of child terminals build from this terminal and constrained
  // through the specification of some user defined contraint.
  std::list<Terminal*> childListOfConstrainedTerminals;

                 // pointer from child to originating terminal (generally and unconstrained terminal)
  Terminal* parentTerminal;

  void setParentTerminal( Terminal* Xptr );
  Terminal* getParentTerminal() const;
  void addChild ( Terminal* Xptr );

                 // The results of expressions are temporaries and need to be designated as such
                 // because they have special settings (often NULL) for there data.  This allows
                 // the error checking to be more aggressive.
  bool temporary;
  bool isTemporary() const;
  void setTemporary( bool input );


  // Header file class definition function prototypes (local and subtree definitions)
  // Use an array of lists since every string has a local node/subtree 
  // representation and a local node/subtree exclude list
  std::list<GrammarString *> memberFunctionPrototypeList[2][2];

  // Header file class definition data declarations (local and subtree definitions)
  std::list<GrammarString *> memberDataPrototypeList[2][2];

  // Source code string lists (local and subtree definitions)
  std::list<GrammarString *> memberFunctionSourceList[2][2];

  enum locationInTree
  {
    LOCAL_LIST   = 0,
    SUBTREE_LIST = 1
  };

  enum includeExcludeMechanism
  {
    INCLUDE_LIST = 0,
    EXCLUDE_LIST = 1
  };

  // Access function for list objects

                 // Test use of non-const access function
  std::list<GrammarString *> & getMemberFunctionPrototypeList ( int i, int j ) const;
  // std::list<GrammarString *> & getMemberFunctionPrototypeList ( int i, int j );

  std::list<GrammarString *> & getMemberDataPrototypeList( int i, int j ) const;
  std::list<GrammarString *> & getMemberFunctionSourceList( int i, int j ) const;

                 // Mechanisms for adding elements to the lists (provides error checking)

                 // testing work around for Insure++ (removing const to avoid copy generated by copy constructor)
#if INSURE_BUG
  static void addElementToList ( std::list<GrammarString *> & list,
				 GrammarString & element );
#else
  static void addElementToList ( std::list<GrammarString *> & list,
				 const GrammarString & element );
#endif

  // Member function for checking lists of GrammarStrings occuring during the
  // class generation phase
  void checkListOfGrammarStrings(std::list<GrammarString *>& list);

                 // Storage of editing strings used for substitution of final output strings
  std::list<GrammarString *> editSubstituteTargetList[2][2];
  std::list<GrammarString *> editSubstituteSourceList[2][2];

  std::list<GrammarString *> & getEditSubstituteTargetList( int i, int j ) const;
  std::list<GrammarString *> & getEditSubstituteSourceList( int i, int j ) const;

                 // void addElementToExcludeList ( std::list<GrammarString *> & list,
                 //                                const GrammarString & element );

                 // Setup functions and data within the implementation
                 // of the grammar represented by this nonTerminal
  void setFunctionPrototype        ( const GrammarString & memberFunction );
  void setFunctionPrototype        ( const std::string& markerString, const std::string& filename, 
				     bool pureVirtual = FALSE );

  void setSubTreeFunctionPrototype ( const GrammarString & memberFunction );
  void setSubTreeFunctionPrototype ( const std::string& markerString, const std::string& filename,
				     bool pureVirtual = FALSE );

  void setFunctionSource           ( const std::string& markerString, const std::string& filename, 
				     bool pureVirtual = FALSE );
  void setSubTreeFunctionSource    ( const std::string& markerString, const std::string& filename, 
				     bool pureVirtual = FALSE );

  // Mechanism for excluding code from specific node or subtrees
  void excludeFunctionPrototype        ( const GrammarString & memberFunction );
  void excludeFunctionPrototype        ( const std::string& markerString, const std::string& filename, 
					 bool pureVirtual = FALSE );
  void excludeSubTreeFunctionPrototype ( const GrammarString & memberFunction );
  void excludeSubTreeFunctionPrototype ( const std::string& markerString, const std::string& filename, 
					 bool pureVirtual = FALSE );

  void excludeFunctionSource           ( const std::string& markerString, const std::string& filename,
					 bool pureVirtual = FALSE );
  void excludeSubTreeFunctionSource    ( const std::string& markerString, const std::string& filename, 
					 bool pureVirtual = FALSE );

  void excludeSubTreeDataPrototype ( const GrammarString & inputMemberData );
  void excludeSubTreeDataPrototype ( const std::string& inputTypeNameString,
				     const std::string& inputVariableNameString,
				     const std::string& defaultInitializer );


  // Data is handled slightly differently, there are no subtree mechanisms
  // for handling data because data in base classes is by definition included
  // within derived classes.
  void setDataPrototype            ( const GrammarString & memberData); 

// AJ (12/3/2004): Added support for deleation of data members
  void setDataPrototype            ( const std::string& inputTypeNameString, 
				     const std::string& inputVariableNameString, 
				     const std::string& defaultInitializer,
				     ConstructParamEnum  constructorParameter,
				     BuildAccessEnum  buildAccessFunctions,
				     bool  toBeTraversedDuringTreeTraversal,
				     bool delete_flag,
                 CopyConfigEnum   toBeCopied = COPY_DATA);

  // Trigger inclusion of initializers in the strings forming the source code referencing data
  // void setIncludeInitializerInDataStrings ( bool X );
  // bool getIncludeInitializerInDataStrings ();

                 // void initialize ( int variant = 0, char* label = NULL, 
                 //                   char* lexemeString = NULL, char* tagString = NULL );

  void initialize ();
  void initialize ( const std::string& label, const std::string& lexemeString, const std::string& tagString );

  ~Terminal ();
  Terminal ();
  Terminal & operator= ( const Terminal & X );
  /* Terminal ( const Terminal & X ); -- JJW 10-16-2007 compiler-generated constructor is fine */
  Terminal ( const std::string& stringVar );
  Terminal ( const std::string& lexeme, Grammar & X, const std::string& stringVar = "", const std::string& tagString = "" );
  // Terminal ( const SageNode* token ) { sageToken = token; };

  // Part of BNF notation (syntax for BNF)
  //                  NonTerminal operator[] ( const NonTerminal & X );
  //                  NonTerminal operator++ ( int );

                 // Access functions
  void setConnectionToLowerLevelGrammar ( Terminal & X );
  Terminal & getConnectionToLowerLevelGrammar ();

  void  setParentTreeNode ( const GrammarTreeNode & inputParent );
  void  setLexeme ( const std::string& label = "defaultLexeme" );
  const std::string& getLexeme () const;
  void  setName   ( const std::string& label = "defaultName", const std::string& tagName = "" );
  const std::string& getName () const;
  void  setTagName   ( const std::string& label = "defaultTagName"   );
  const std::string& getTagName () const;
  const std::string& getBaseName () const;

  void setGrammar ( Grammar* grammarPointer );
  Grammar* getGrammar () const;

                 // builds child terminals from parent terminals
  Terminal & copy ( const std::string& lexeme, const std::string& tagString );

  void addGrammarPrefixToName();

  void    setBuildDefaultConstructor ( bool X );
  bool    getBuildDefaultConstructor () const;

  std::string   buildDataAccessFunctions ( const GrammarString & inputMemberData);

  bool isType() const;
  bool isExpression() const;
  bool isStatement() const;

  std::string buildConstructorBody( bool withInitializers, ConstructParamEnum config );

  // AJ ( 10/26/2004)
  // Builds the destructor body
  std::string buildDestructorBody();

  void displayName ( int indent = 0 ) const;

                 // this stores the old and new string used for the substitution and applies
                 // it before final copying out of a node of the grammar to a file!
  void editSubstitute               ( const std::string& oldString, const std::string& newString );
  void editSubstituteSubTree        ( const std::string& oldString, const std::string& newString );
  void editSubstituteExclude        ( const std::string& oldString, const std::string& newString );
  void editSubstituteExcludeSubTree ( const std::string& oldString, const std::string& newString );

  void editSubstitute               ( const std::string& oldString, const std::string& markerName, const std::string& filename );
  void editSubstituteSubTree        ( const std::string& oldString, const std::string& markerName, const std::string& filename );
  void editSubstituteExclude        ( const std::string& oldString, const std::string& markerName, const std::string& filename );
  void editSubstituteExcludeSubTree ( const std::string& oldString, const std::string& markerName, const std::string& filename );

                 // Access functions for pre and post declaration strings
  void setPredeclarationString  ( const std::string& declarationString );
  void setPostdeclarationString ( const std::string& declarationString );
  void setPredeclarationString  ( const std::string& markerString, const std::string& filename );
  void setPostdeclarationString ( const std::string& markerString, const std::string& filename );

  const std::string& getPredeclarationString () const;
  const std::string& getPostdeclarationString() const;

                 // Sometimes we don't want the constructor to be automatically generated 
                 // because it must be specialied in a way that is not possible for the 
                 // automated tool (such cases represent asymetries in the design and so 
                 // they should be avoided).  In general such details should be handled 
                 // within the post_construction_initialization() member function.
  void setAutomaticGenerationOfConstructor         ( bool X = TRUE );
  void setAutomaticGenerationOfDestructor          ( bool X = TRUE );
  void setAutomaticGenerationOfDataAccessFunctions ( bool X = TRUE );
  void setAutomaticGenerationOfCopyFunction        ( bool X = TRUE );

                 // Access function for automaticGenerationOfDestructor and automaticGenerationOfConstructor
  bool generateDestructor () const;
  bool generateConstructor() const;
  bool generateDataAccessFunctions() const;
  bool generateCopyFunction() const;

// char* buildCopyMemberFunctionSource ();
  StringUtility::FileWithLineNumbers buildCopyMemberFunctionSource ();
  StringUtility::FileWithLineNumbers buildCopyMemberFunctionHeader ();

// DQ & JH (1/17/2006): Added support for building code to check pointers to IR nodes
  std::string buildPointerInMemoryPoolCheck ();

// AS (2/18/06): Added support for building code to return data member pointers to IR nodes
  std::string buildReturnDataMemberPointers ();
  std::string buildListIteratorString(std::string typeName, std::string variableName, std::string classNameString);

// DQ (4/30/2006): Added similar function to buildReturnDataMemberPointers() so that we could 
// return references to the pointers to IR nodes (required low level support for AST Merge).
  std::string buildReturnDataMemberReferenceToPointers ();
  std::string buildListIteratorStringForReferenceToPointers(std::string typeName, std::string variableName, std::string classNameString);

// DQ (3/7/2007): Building support for "long getChildIndex();" to be use for "bool isChild();" and other purposes.
  std::string buildChildIndex ();
  std::string buildListIteratorStringForChildIndex(std::string typeName, std::string variableName, std::string classNameString);

   void show() const;

                 // Interface to contraint mechanism
                 // void constrainName ( char* nameString );
  void addConstraint ( const std::string& terminalName, const std::string& constraintString );

  void consistencyCheck() const;
  void display ( const std::string& label = "" ) const;

  // A terminal is a child if it is the result of a copy from an existing terminal
  bool isChild() const;

  void copyTerminalData ( Terminal* childTerminal );

                 // Used for assignment of expressions to terminals (operator= is 
                 // usually used but we want to have specialized functions so that
                 // the operator= can be reserved for more traditional use)
  void setExpression ( const Terminal & X );

/* JH (21/01/20005): Since in every method for building the source code for the StorageClases
   we need to build specific code for the data members of IRNode classes, we list all cases that
   we have to separate. Some of them can be removed, when ROSE does not longer contain pointers
   to stl vectors or lists. They are marked below, but may cause no trouble if they remain.
*/
#if 0
// DQ (4/6/2006): Old enum from Jochen's original work.
  enum TypeEvaluation {
                       CHAR_POINTER,
                       CONST_CHAR_POINTER,
                       ATTACHEDPREPROCESSINGINFOTYPE,
                       ROSE_HASH_MULTIMAP,
                       SGCLASS_POINTER,
                       ROSEATTRUBUTESLISTCONTAINER,
                       SGCLASS_POINTER_LIST,
                       SGCLASS_POINTER_VECTOR,
                       SGCLASS_POINTER_VECTOR_NAMED_LIST,
                       STL_CONTAINER,
                       STL_SET,
                       STL_MAP,
                       STRING,
                       SGNAME,
                       BIT_VECTOR,
                       DECLARATIONMODIFIERCLASS,
                       MODIFIERCLASS,
                       ASTATTRIBUTEMECHANISM,
                       TO_HANDLE,
                       OSTREAM,
                       ENUM_TYPE,
                       BASIC_DATA_TYPE,
                       SKIP_TYPE,
                    // should be no longer necessary after the implementation is changed to STL lists instead of pointers to lists
                       SGCLASS_POINTER_LIST_POINTER
                      };
#endif
// DQ (4/6/2006): New enum from Jochen's muilt file support.
  enum TypeEvaluation {
                       CHAR_POINTER,
                       CONST_CHAR_POINTER,
                       ATTACHEDPREPROCESSINGINFOTYPE,
                       ROSE_HASH_MULTIMAP,
                       SGCLASS_POINTER,
                       ROSEATTRUBUTESLISTCONTAINER,
                       SGCLASS_POINTER_LIST,
                       SGCLASS_POINTER_VECTOR,
                       SGCLASS_POINTER_VECTOR_NAMED_LIST,
                       STL_CONTAINER,
                       STL_SET,
                       STL_MAP,
                       STRING,
                       SGNAME,
                       BIT_VECTOR,
                       MODIFIERCLASS,
                       MODIFIERCLASS_WITHOUTEASYSTORAGE,
                       ASTATTRIBUTEMECHANISM,
                       TO_HANDLE,
                       OSTREAM,
                       ENUM_TYPE,
                       BASIC_DATA_TYPE,
                       SKIP_TYPE,
                    // should be no longer necessary after the implementation is changed to STL lists instead of pointers to lists
                       SGCLASS_POINTER_LIST_POINTER
                      };
/* JH (21/01/20005): method that evaluates the type string of a data member and returns the corresponding
   TypeEvaluation type
*/
// DQ (4/6/2006): Modified the function paramter as per Jochen's version
//TypeEvaluation evaluateType (const std::string& varTypeString);
  TypeEvaluation evaluateType (std::string& varTypeString);


/* JH (10/28/2005): declaration of the source building methods for the storage classes
   concenrning the ast file IO. More about them one can find in the terminal.C file. They
   are called while the code generation of ROSETTA by the method 'buildStringForStorageClassSource'
   in the file grammar.C
*/
  std::string buildStorageClassHeader ();
  std::string buildStorageClassDeleteStaticDataSource ();
  std::string buildStorageClassArrangeStaticDataInOneBlockSource ();
  std::string buildStorageClassPickOutIRNodeDataSource ();
  std::string buildPointerInPoolCheck();
  std::string buildSourceForIRNodeStorageClassConstructor();
  std::string buildSourceForStoringStaticMembers ();
  std::string buildInitializationForStaticMembers ();
  std::string buildStaticDataConstructorSource();
//std::string buildSourceForRebuildingStaticMembers();
  std::string buildStorageClassWriteStaticDataToFileSource () ;
  std::string buildStorageClassReadStaticDataFromFileSource ();
//std::string buildWriteStaticDataToFileSource () ;
//std::string buildReadStaticDataFromFileSource ();
  bool hasMembersThatAreStoredInEasyStorageClass();
  bool hasStaticMembers ();

// DQ (4/6/2006): Added these to match Jochen's new version
  std::string buildStaticDataMemberList();
  std::string buildStaticDataMemberListSetStaticData();
  std::string buildStaticDataMemberListDeleteStaticData();
  std::string buildStaticDataMemberListConstructor();
  std::string buildAccessFunctionsForStaticDataMember();
  std::string buildAccessFunctionsForStaticDataMemberSource();
  std::string buildStaticDataMemberListOfStorageClass();
  std::string buildStaticDataWriteEasyStorageDataToFileSource () ;
  std::string buildStaticDataReadEasyStorageDataFromFileSource();
  std::string buildStaticDataArrangeEasyStorageInOnePoolSource();
  std::string buildStaticDataDeleteEasyStorageMemoryPoolSource();

// DQ (5/18/2007): support for documentation to handle mapping to KDM
  std::string outputClassesAndFields ();

};

#endif
