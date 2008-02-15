// Author: Markus Schordan
// $Id: AstAttributeMechanism.h,v 1.3 2006/04/24 00:21:32 dquinlan Exp $

#ifndef ASTATTRIBUTEMECHANISM_H
#define ASTATTRIBUTEMECHANISM_H

#include "AttributeMechanism.h"

class AstAttribute {
public:
  AstAttribute() {}
  virtual ~AstAttribute() {}
  /*! This function is used by other components to print the value of an attribute. For example the pdf generation
      calls this function to print the value of an attribute in the pdf file. The default implementation is to
      return an empty string.
  */
  virtual std::string toString() { return ""; }

// JH (12/21/2005): Adding Methods for storing the Ast Attribute data
  AstAttribute* constructor() {return new AstAttribute();}
  std::string attribute_class_name() { return "AstAttribute"; }
  virtual int packed_size() { return 0; }
  virtual char* packed_data() { return NULL; }
  virtual void unpacked_data( int size, char* data ) { ; }


};

class AstAttributeMechanism : public AttributeMechanism<std::string,AstAttribute*> {
};

#endif
