
/* unparser.h
 * This header file contains the class declaration for the newest unparser. Six
 * C files include this header file: unparser.C, modified_sage.C, unparse_stmt.C, 
 * unparse_expr.C, unparse_type.C, and unparse_sym.C.
 */

#ifndef UNPARSER_NAME_QUALIFICATION
#define UNPARSER_NAME_QUALIFICATION


// #include "rose.h"
// #include "unparser_opt.h"
// #include "unparser.h"
// #include "unparse_type.h"

class Unparser;

class Unparser_Nameq
   {
  // This class provides low level functions to control how we get line number 
  // and column number information.  All formating information comes
  // from this class and the user can then control the formatting of source 
  // code by deriving their own class from this one.  Our goal is not particularly
  // to represent the most complex pretty printing by to allow the output to be
  // easily tailored separately from the implementation of the code generation.
   private:
     Unparser* unp;
     public:
     Unparser_Nameq(Unparser* unp):unp(unp){};
          virtual ~Unparser_Nameq() {};
       // DQ (6/4/2007): Support for name qualification (detects of a name in a scope is visible from another scope).
       // This is true iff targetScope is an outer enclosing scope of currentScope.
          bool isAnOuterScope( SgScopeStatement* currentScope, SgScopeStatement* targetScope );

       // DQ (11/11/2004) Factored code specific to qualifier for qualified names
       //! Generate the qualified for the name associated with the SgDeclarationStatement
       // SgName generateNameQualifier( SgDeclarationStatement* declarationStatement, const SgUnparse_Info & info );
       // SgName generateNameQualifier( SgDeclarationStatement* declarationStatement, const SgUnparse_Info & info, int debugSupport = 0 );
          SgName generateNameQualifier( SgDeclarationStatement* declarationStatement, const SgUnparse_Info & info, bool nestedQualification = false );

      //! Generate the qualified for the name associated with the SgInitializedName
       // SgName generateNameQualifier( SgInitializedName* initializedName, const SgUnparse_Info& info, int debugSupport = 0 );
          SgName generateNameQualifier( SgInitializedName* initializedName, const SgUnparse_Info& info, bool nestedQualification = false );

       // DQ (5/22/2007): Added specialized support for name qualification (part of new interface 
       // using hidden type, declaration and class elaboration lists).
          SgName generateNameQualifierForType( SgType* type, const SgUnparse_Info & info );



   };

#endif

