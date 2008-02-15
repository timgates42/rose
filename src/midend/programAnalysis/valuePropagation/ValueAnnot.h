#ifndef VALUE_ANNOT_H
#define VALUE_ANNOT_H

#include <AnnotExpr.h>
#include <AnnotCollect.h>
#include <OperatorAnnotation.h>
#include <list>

class HasValueDescriptor 
   : public MapDescriptor<StringDescriptor, SymbolicValDescriptor, ';', '{', '}'>
{
 public:
  typedef 
     MapDescriptor<StringDescriptor, SymbolicValDescriptor, ';', '{', '}'>
  BaseClass;
  bool read( std::istream& in)
    {
      return ReadContainer<HasValueDescriptor, 
                           CollectPair<StringDescriptor, SymbolicValDescriptor, '='>,
	                   ';', '{','}'>::read(*this, in);   
    }
  bool merge (const HasValueDescriptor& that); 
  void Dump() const;
  void replace_var( const std::string& name, const SymbolicVal& repl);
  void replace_val( MapObject<SymbolicVal, SymbolicVal>& repl); 

  bool has_value( const std::string& name, SymbolicValDescriptor* r = 0) const;
};

class HasValueCollection : public TypeAnnotCollection< HasValueDescriptor>
{
   typedef TypeAnnotCollection< HasValueDescriptor > BaseClass;
  virtual bool read_annot_name( const std::string& annotName) const 
    { return annotName == "has_value"; }
 public:
  void Dump() const 
    { std::cerr << "has_value: \n"; BaseClass::Dump(); }
};

typedef CollectPair< SymbolicValDescriptor, HasValueDescriptor, '='> RestrictValueDescriptor;
class RestrictValueOpDescriptor 
 : public  OPDescriptorTemp 
    <ContainerDescriptor< std::list <RestrictValueDescriptor>, RestrictValueDescriptor, ';', '{', '}'>  > 
{
  typedef  OPDescriptorTemp
    <ContainerDescriptor< std::list <RestrictValueDescriptor>, RestrictValueDescriptor, ';', '{', '}'>  >
    BaseClass;
 public:
  void replace_val( MapObject<SymbolicVal, SymbolicVal>& repl); 
};

class RestrictValueOpCollection
: public OperatorAnnotCollection<RestrictValueOpDescriptor>
{
  virtual bool read_annot_name( const std::string& annotName) const
    { return annotName == "restrict_value"; }
 public:
  void Dump() const
    {
      std::cerr << "restrict_value: \n";
      OperatorAnnotCollection<RestrictValueOpDescriptor>::Dump();
    }
};

class ValueAnnotation 
{
  HasValueCollection values;
  RestrictValueOpCollection valueRestrict;
  static ValueAnnotation* inst;
  ValueAnnotation() {}
 public:
  static ValueAnnotation* get_inst();
  void register_annot(); 
  void Dump() const;
  bool known_type( const AstNodePtr& exp, HasValueDescriptor* d = 0);
  bool is_value_restrict_op( const AstNodePtr& exp, 
                    Collect2Object< AstNodePtr, HasValueDescriptor>* desc = 0,
                    MapObject<SymbolicVal, SymbolicVal>* valMap = 0,
                    Map2Object<AstInterface*, AstNodePtr, AstNodePtr> *codegen = 0);
  bool is_access_value( const AstNodePtr& exp,
			AstNodePtr* obj=0, std::string* name = 0,
			AstInterface::AstNodeList* args=0,
			HasValueDescriptor* desc = 0);
};

#endif

