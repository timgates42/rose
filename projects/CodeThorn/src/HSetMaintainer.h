#ifndef HSET_MAINTAINER_H
#define HSET_MAINTAINER_H

#include "HSet.h"
using namespace br_stl;

//#define HSET_MAINTAINER_DEBUG_MODE

template<typename KeyType,typename HashFun>
class HSetMaintainer : public HSet<KeyType,HashFun> {
public:
  typedef pair<bool,const KeyType*> ProcessingResult;
  bool exists(KeyType& s) { 
	typename HSetMaintainer<KeyType,HashFun>::iterator i;
	i=HSetMaintainer<KeyType,HashFun>::find(s);
	return i!=HSetMaintainer<KeyType,HashFun>::end();
  }
  //! <true,const KeyType> if new element was inserted
  //! <false,const KeyType> if element already existed
  ProcessingResult process(KeyType key) {
	ProcessingResult res2;
	#pragma omp critical
	{
	std::pair<typename HSetMaintainer::iterator, bool> res;
	res=insert(key);
#ifdef HSET_MAINTAINER_DEBUG_MODE
	std::pair<typename HSetMaintainer::iterator, bool> res1;
	res1=insert(key);
	std::pair<typename HSetMaintainer::iterator, bool> res2;
	res2=insert(key);
	if(!(res1==res2)) {
	  cerr<< "Error: HsetMaintainer failed:"<<endl;
	  cerr<< "res1:"<<(*res1.first).toString()<<":"<<res1.second<<endl;
	  cerr<< "res2:"<<(*res2.first).toString()<<":"<<res2.second<<endl;
	  exit(1);
	}
	cerr << "HSET insert OK"<<endl;
#endif
	res2=make_pair(res.second,&(*res.first));
	}
	return res2;
  }
  const KeyType* processNew(KeyType& s) {
	//std::pair<typename HSetMaintainer::iterator, bool> res=process(s);
	ProcessingResult res=process(s);
	if(res.first!=true) {
	  cerr<< "Error: HsetMaintainer::processNew failed:"<<endl;
	  cerr<< "res:";
	  cout <<":"<<res.first<<endl;
	  cout <<res.second->toString();
	  exit(1);
	}
	return res.second;
  }
  const KeyType* processNewOrExisting(KeyType& s) {
	ProcessingResult res=process(s);
	return res.second;
  }
  long numberOf() { return HSetMaintainer<KeyType,HashFun>::size(); }

  long maxCollisions() {
	return HSetMaintainer<KeyType,HashFun>::max_collisions();
  }

  double loadFactor() {
	return HSetMaintainer<KeyType,HashFun>::load_factor();
  }

  long memorySize() const {
	long mem=0;
	for(typename HSetMaintainer<KeyType,HashFun>::const_iterator i
		  =HSetMaintainer<KeyType,HashFun>::begin();
		i!=HSetMaintainer<KeyType,HashFun>::end();
		++i) {
	  mem+=(*i).memorySize();
	}
	return mem+sizeof(*this);
  }

 private:
  const KeyType* ptr(KeyType& s) {}
};

#endif
