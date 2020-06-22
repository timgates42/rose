
#ifndef CTX_LATTICE_H
#define CTX_LATTICE_H 1

/// \author Peter Pirkelbauer

#include "sageGeneric.h"

#include "DFAnalysisBase.h"

// for debugging
#include "RDLattice.h"


namespace CodeThorn
{

namespace
{
  template <class P>
  inline
  std::string type_name(P* p)
  {
    if (p == NULL) return "null";

    return typeid(*p).name();
  }

  static inline
  void dbg_rd(std::ostream& os, char sep, Lattice* lat)
  {
    RDLattice& rdlat = dynamic_cast<RDLattice&>(sg::deref(lat));

    os << lat << sep << rdlat.size() << std::endl;
  }


  /// pseudo type to indicate that an element is not in a sequence
  struct unavailable_t {};

  /// \brief  traverses two ordered associative sequences in order of their elements.
  ///         The elements in the sequences must be convertible. A merge object
  ///         is called with sequence elements in order of their keys in [aa1, zz1[ and [aa2, zz2[.
  /// \tparam _Iterator1 an iterator of an ordered associative container
  /// \tparam _Iterator2 an iterator of an ordered associative container
  /// \tparam BinaryOperator a merge object that provides three operator()
  ///         functions. 
  ///         - void operator()(_Iterator1::value_type, unavailable_t);
  ///           called when an element is in sequence 1 but not in sequence 2.
  ///         - void operator()(unavailable_t, _Iterator2::value_type);
  ///           called when an element is in sequence 2 but not in sequence 1.
  ///         - void operator()(_Iterator1::value_type, _Iterator2::value_type);
  ///           called when an element is in both sequences.
  /// \tparam Comparator compares elements in sequences.
  ///         called using both (_Iterator1::key_type, _Iterator2::key_type)
  //          and (_Iterator2::key_type, _Iterator1::key_type).
  template <class _Iterator1, class _Iterator2, class BinaryOperator, class Comparator>
  BinaryOperator
  merge_keys( _Iterator1 aa1, _Iterator1 zz1, 
              _Iterator2 aa2, _Iterator2 zz2, 
              BinaryOperator binop, 
              Comparator comp 
            )
  {
    static constexpr unavailable_t unavail;

    while (aa1 != zz1 && aa2 != zz2)
    {
      if (comp((*aa1).first, (*aa2).first))
      {
        binop(*aa1, unavail);
        ++aa1;
      }
      else if (comp((*aa2).first, (*aa1).first))
      {
        binop(unavail, *aa2);
        ++aa2;
      }
      else
      {
        binop(*aa1, *aa2);
        ++aa1; ++aa2;
      }
    }

    while (aa1 != zz1)
    {
      binop(*aa1, unavail);
      ++aa1;
    }

    while (aa2 != zz2)
    {
      binop(unavail, *aa2);
      ++aa2;
    }

    return binop;
  }


  /// clones a lattice @ref orig.
  template <class CodeThornFactory, class CodeThornLattice>
  inline
  CodeThornLattice*
  cloneLattice(CodeThornFactory& factory, const CodeThornLattice& elem)
  {
    CodeThornLattice& orig  = const_cast<CodeThornLattice&>(elem);
    CodeThornLattice& clone = sg::deref(factory.create());

    clone.combine(orig);

    ROSE_ASSERT(orig.approximatedBy(clone));
    ROSE_ASSERT(clone.approximatedBy(orig));
    return &clone;
  }


  template <class Map>
  struct LatticeCombiner
  {
      typedef typename Map::value_type entry_t;

      explicit
      LatticeCombiner(Map& lhsLatticemap)
      : lhslattice(lhsLatticemap)
      {}

      void operator()(const unavailable_t&, const entry_t& rhs)
      {
        CodeThorn::PropertyStateFactory& factory = lhslattice.componentFactory();
        Lattice&                         sublat  = sg::deref(rhs.second);

        lhslattice[rhs.first] = cloneLattice(factory, sublat);
      }

      void operator()(entry_t& lhs, const unavailable_t&)
      {
        // nothing to do
      }

      void operator()(entry_t& lhs, const entry_t& rhs)
      {
        lhs.second->combine(const_cast<Lattice&>(sg::deref(rhs.second)));
      }

    private:
      Map& lhslattice;
  };

  template <class M>
  inline
  LatticeCombiner<M>
  latticeCombiner(M& lhsLatticemap)
  {
    return LatticeCombiner<M>(lhsLatticemap);
  }

  /// functor that determines whether an element exists in some map.
  template <class M>
  struct CtxLatticeNotIn
  {
      typedef typename M::value_type entry_t;

      explicit
      CtxLatticeNotIn(const M& rhsLatticemap)
      : rhslattice(rhsLatticemap)
      {}

      // returns true if this element lattice IS NOT in rhslattice
      //   (used by find_if to find the first element not in rhslattice)
      bool operator()(const entry_t& lhslattice)
      {
        typename M::const_iterator rhspos = rhslattice.find(lhslattice.first);
        
        return rhspos == rhslattice.end()
               || !lhslattice.second->approximatedBy(sg::deref(rhspos->second));
      }

    private:
      const M& rhslattice;
  };

  // deduces M for class construction
  template <class M>
  static inline
  CtxLatticeNotIn<M>
  ctxLatticeNotIn(const M& rhslattices)
  {
    return CtxLatticeNotIn<M>(rhslattices);
  }

  /// Deletes lattices in maps
  struct LatticeDeleter
  {
    template<class Key>
    void operator()(std::pair<const Key, Lattice*>& pair)
    {
      delete pair.second;
    }
  };

  /// deletes all Lattice* in maps within the range [aa, zz)
  template <class _ForwardIterator>
  void deleteLattices(_ForwardIterator aa, _ForwardIterator zz)
  {
    std::for_each(aa, zz, LatticeDeleter());
  }

  struct CtxLatticeStreamer
  {
      CtxLatticeStreamer(std::ostream& stream, VariableIdMapping* vmap)
      : os(stream), vm(vmap)
      {}

      template <class CallContext>
      void operator()(const std::pair<const CallContext, Lattice*>& entry)
      {
        os << "[" << entry.first << ": ";
        entry.second->toStream(os, vm);
        os << "]";
      }

    private:
      std::ostream&      os;
      VariableIdMapping* vm;
  };
}

/// A CtxLattice holds information organized by call contexts.
/// Each lattice element (aka call context) has a component lattice
/// that stores the information of some specific analysis.
template <class CallContext>
struct CtxLattice : Lattice, private std::map<CallContext, Lattice*, typename CallContext::comparator>
{
    typedef Lattice                                                         base;
    typedef CallContext                                                     context_t;
    typedef std::map<context_t, Lattice*, typename CallContext::comparator> context_map;

    using context_map::value_type;
    using context_map::iterator;
    using context_map::const_iterator;
    using context_map::const_reverse_iterator;
    using context_map::begin;
    using context_map::end;
    using context_map::rbegin;
    using context_map::rend;
    using context_map::clear;
    using context_map::insert;
    using context_map::lower_bound;
    using context_map::find;
    using context_map::size;
    using context_map::operator[];

    explicit
    CtxLattice(PropertyStateFactory& compfac)
    : base(), context_map(), compPropertyFactory(compfac)
    {}

    ~CtxLattice()
    {
      deleteLattices(begin(), end());
    }

    void swap(CtxLattice<context_t>& that)
    {
      context_map::swap(that);
    }

    bool isBot() const ROSE_OVERRIDE { return context_map::size() == 0; }

    bool isBot() 
    {
      const CtxLattice<context_t>& self = *this;

      return self.isBot();
    }

    bool approximatedBy(Lattice& other) const ROSE_OVERRIDE
    {
      const CtxLattice<context_t>& that = dynamic_cast<CtxLattice<context_t>& >(other);

      // this \ other == {}
      // this is approximated by other, iff this w/o other yields the empty set.
      return std::find_if(begin(), end(), ctxLatticeNotIn(that)) == end();
    }

    void combine(Lattice& other) ROSE_OVERRIDE
    {
      const CtxLattice<context_t>& that = dynamic_cast<CtxLattice<context_t>& >(other);

      merge_keys( begin(), end(), 
                  that.begin(), that.end(), 
                  latticeCombiner(*this), 
                  context_map::key_comp()
                );

      ROSE_ASSERT(this->size() >= that.size());
      ROSE_ASSERT(other.approximatedBy(*this));
    }

    PropertyStateFactory& componentFactory()
    {
      return compPropertyFactory;
    }

    void toStream(std::ostream& os, VariableIdMapping* vm) ROSE_OVERRIDE
    {
      if (isBot()) { os << " bot "; return; }

      os << "{";
      std::for_each(begin(), end(), CtxLatticeStreamer(os, vm));
      os << "}";
    }

  private:
    PropertyStateFactory& compPropertyFactory;

    // CtxLattice(const CtxLattice&) = delete;
    // CtxLattice(CtxLattice&&) = delete;
};

} // namespace CodeThorn

#endif /* CTX_LATTICE_H */
