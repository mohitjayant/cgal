// TODO: Add licence
//
// This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
// WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
//
// $URL$
// $Id$
// 
//
// Author(s)     : Michael Hemmer <hemmer@informatik.uni-mainz.de> 
//                 Sebastian Limbach <slimbach@mpi-inf.mpg.de>
//
// ============================================================================
#ifndef CGAL_POLYNOMIAL_TRAITS_D_H
#define CGAL_POLYNOMIAL_TRAITS_D_H

#include <CGAL/basic.h>
#include <functional>

#include <CGAL/Polynomial/fwd.h>
#include <CGAL/Polynomial/misc.h>
#include <CGAL/Polynomial/Polynomial_type.h>
#include <CGAL/Polynomial/polynomial_utils.h>
#include <CGAL/Polynomial/resultant.h>
#include <CGAL/Polynomial/subresultants.h>
#include <CGAL/Polynomial/sturm_habicht_sequence.h>
#include <CGAL/Polynomial/square_free_factorize.h>
#include <CGAL/Polynomial/modular_filter.h>
#include <CGAL/extended_euclidean_algorithm.h>

#define CGAL_POLYNOMIAL_TRAITS_D_BASE_TYPEDEFS                          \
  private:                                                              \
  typedef Polynomial_traits_d< Polynomial< Coefficient_type_ > > PT;    \
  typedef Polynomial_traits_d< Coefficient_type_ > PTC;                 \
                                                                        \
  typedef Polynomial<Coefficient_type_>               Polynomial_d;     \
  typedef Coefficient_type_                          Coefficient_type;  \
                                                                        \
  typedef typename Innermost_coefficient_type<Polynomial_d>::Type       \
  Innermost_coefficient_type;                                           \
  static const int d = Dimension<Polynomial_d>::value;                  \
                                                                        \
                                                                        \
  typedef std::pair< Exponent_vector, Innermost_coefficient_type >      \
  Exponents_coeff_pair;                                                 \
  typedef std::vector< Exponents_coeff_pair > Monom_rep;                \
                                                                        \
  typedef CGAL::Recursive_const_flattening< d-1,                        \
    typename CGAL::Polynomial<Coefficient_type>::const_iterator >       \
  Coefficient_const_flattening;                                         \
                                                                        \
  typedef typename                                                      \
  Coefficient_const_flattening::Recursive_flattening_iterator           \
  Innermost_coefficient_const_iterator;                                 \
                                                                        \
  typedef typename  Polynomial_d::const_iterator                        \
  Coefficient_const_iterator;
 

CGAL_BEGIN_NAMESPACE;

namespace CGALi {

// Base class for functors depending on the algebraic category of the
// innermost coefficient
template< class Coefficient_type_, class ICoeffAlgebraicCategory >
class Polynomial_traits_d_base_icoeff_algebraic_category {    
public:    
  typedef Null_functor    Multivariate_content;
};

// Specializations
template< class Coefficient_type_ >
class Polynomial_traits_d_base_icoeff_algebraic_category< 
            Polynomial< Coefficient_type_ >, Integral_domain_without_division_tag > 
  : public Polynomial_traits_d_base_icoeff_algebraic_category< 
                Polynomial< Coefficient_type_ >, Null_tag > {};

template< class Coefficient_type_ >
class Polynomial_traits_d_base_icoeff_algebraic_category< 
       Polynomial< Coefficient_type_ >, Integral_domain_tag >
  : public Polynomial_traits_d_base_icoeff_algebraic_category< 
       Polynomial< Coefficient_type_ >, Integral_domain_without_division_tag > {}; 

template< class Coefficient_type_ >
class Polynomial_traits_d_base_icoeff_algebraic_category< 
            Polynomial< Coefficient_type_ >, Unique_factorization_domain_tag >
  : public Polynomial_traits_d_base_icoeff_algebraic_category< 
                Polynomial< Coefficient_type_ >, Integral_domain_tag > {
  CGAL_POLYNOMIAL_TRAITS_D_BASE_TYPEDEFS
    
public:    

 
 struct Multivariate_content
    : public std::unary_function< Polynomial_d , Innermost_coefficient_type >{
    Innermost_coefficient_type 
    operator()(const Polynomial_d& p) const {
      typedef Innermost_coefficient_const_iterator IT;
      Innermost_coefficient_type content(0);
      for (IT it = typename PT::Innermost_coefficient_const_begin()(p);
           it != typename PT::Innermost_coefficient_const_end()(p);
           it++){
        content = CGAL::gcd(content, *it);
        if(CGAL::is_one(content)) break;
      }
      return content;
    }
  };
}; 

template< class Coefficient_type_ >
class Polynomial_traits_d_base_icoeff_algebraic_category< 
            Polynomial< Coefficient_type_ >, Euclidean_ring_tag >
  : public Polynomial_traits_d_base_icoeff_algebraic_category< 
                Polynomial< Coefficient_type_ >, Unique_factorization_domain_tag >
{}; 

template< class Coefficient_type_ >
class Polynomial_traits_d_base_icoeff_algebraic_category< 
            Polynomial< Coefficient_type_ >, Field_tag >
  : public Polynomial_traits_d_base_icoeff_algebraic_category< 
                Polynomial< Coefficient_type_ >, Integral_domain_tag > {
  CGAL_POLYNOMIAL_TRAITS_D_BASE_TYPEDEFS

public:
    
  //       Multivariate_content;
  struct Multivariate_content
    : public std::unary_function< Polynomial_d , Innermost_coefficient_type >{
    Innermost_coefficient_type operator()(const Polynomial_d& p) const {
      if( CGAL::is_zero(p) )
        return Innermost_coefficient_type(0);
      else
        return Innermost_coefficient_type(1);
    }
  };
};

template< class Coefficient_type_ >
class Polynomial_traits_d_base_icoeff_algebraic_category< 
            Polynomial< Coefficient_type_ >, Field_with_sqrt_tag >
  : public Polynomial_traits_d_base_icoeff_algebraic_category< 
                Polynomial< Coefficient_type_ >, Field_tag > {}; 

template< class Coefficient_type_ >
class Polynomial_traits_d_base_icoeff_algebraic_category< 
            Polynomial< Coefficient_type_ >, Field_with_kth_root_tag >
  : public Polynomial_traits_d_base_icoeff_algebraic_category< 
                Polynomial< Coefficient_type_ >, Field_with_sqrt_tag > {}; 
 
template< class Coefficient_type_ >
class Polynomial_traits_d_base_icoeff_algebraic_category< 
            Polynomial< Coefficient_type_ >, Field_with_root_of_tag >
  : public Polynomial_traits_d_base_icoeff_algebraic_category< 
                Polynomial< Coefficient_type_ >, Field_with_kth_root_tag > {}; 

// Base class for functors depending on the algebraic category of the
// Polynomial type
template< class Coefficient_type_, class PolynomialAlgebraicCategory >
class Polynomial_traits_d_base_polynomial_algebraic_category {        
public:
  typedef Null_functor    Univariate_content;
  typedef Null_functor    Square_free_factorize;
};

// Specializations
template< class Coefficient_type_ >
class Polynomial_traits_d_base_polynomial_algebraic_category<
            Polynomial< Coefficient_type_ >, Integral_domain_without_division_tag >
  : public Polynomial_traits_d_base_polynomial_algebraic_category<
                Polynomial< Coefficient_type_ >, Null_tag > {};

template< class Coefficient_type_ >
class Polynomial_traits_d_base_polynomial_algebraic_category<
            Polynomial< Coefficient_type_ >, Integral_domain_tag >
  : public Polynomial_traits_d_base_polynomial_algebraic_category<
          Polynomial< Coefficient_type_ >, Integral_domain_without_division_tag > {};

template< class Coefficient_type_ >
class Polynomial_traits_d_base_polynomial_algebraic_category<
            Polynomial< Coefficient_type_ >, Unique_factorization_domain_tag >
  : public Polynomial_traits_d_base_polynomial_algebraic_category<
                Polynomial< Coefficient_type_ >, Integral_domain_tag > {
  CGAL_POLYNOMIAL_TRAITS_D_BASE_TYPEDEFS

public:
    
  //       Univariate_content
  struct Univariate_content
    : public std::unary_function< Polynomial_d , Coefficient_type>{
    Coefficient_type operator()(const Polynomial_d& p) const {
      return p.content();
    }
    Coefficient_type operator()(Polynomial_d p, int i) const {
      return typename PT::Swap()(p,i,PT::d-1).content();
    }
  };
    
  //       Square_free_factorize;
  struct Square_free_factorize{
    
    template < class OutputIterator >
    OutputIterator operator()( const Polynomial_d& p, OutputIterator oi) const {
      std::vector<Polynomial_d> factors;
      std::vector<int> mults; 
      
      square_free_factorize
        ( p, std::back_inserter(factors), std::back_inserter(mults) );
      
      CGAL_postcondition( factors.size() == mults.size() );
      for(unsigned int i = 0; i < factors.size(); i++){
        *oi++=std::make_pair(factors[i],mults[i]);
      }
      
      return oi;
    }
    
    template< class OutputIterator >
    OutputIterator operator()( 
        const Polynomial_d&    p , 
        OutputIterator         oi, 
        Innermost_coefficient_type& a ) {
      
      if( CGAL::is_zero(p) ) {
        a = Innermost_coefficient_type(0);
        return oi;
      }

      typedef Polynomial_traits_d< Polynomial_d > PT;
      typename PT::Innermost_leading_coefficient ilcoeff;
      typename PT::Multivariate_content mcontent;
      a = CGAL::unit_part( ilcoeff( p ) ) * mcontent( p ); 
      
      return (*this)( p/Polynomial_d(a), oi);
    }
  };   
};

template< class Coefficient_type_ >
class Polynomial_traits_d_base_polynomial_algebraic_category<
            Polynomial< Coefficient_type_ >, Euclidean_ring_tag >
  : public Polynomial_traits_d_base_polynomial_algebraic_category<
                Polynomial< Coefficient_type_ >, Unique_factorization_domain_tag > {};


// Polynomial_traits_d_base class connecting the two base classes which depend
//  on the algebraic category of the innermost coefficient type and the poly-
//  nomial type.

// First the general base class for the innermost coefficient
template< class InnermostCoefficient_type, 
          class ICoeffAlgebraicCategory, class PolynomialAlgebraicCategory >
class Polynomial_traits_d_base {
  typedef InnermostCoefficient_type ICoeff;
public:
  static const int d = 0;
    
  typedef ICoeff Polynomial_d;
  typedef ICoeff Coefficient_type;
  typedef ICoeff Innermost_coefficient_type;
    
  struct Degree 
    : public std::unary_function< ICoeff , int > {
    int operator()(const ICoeff&) const { return 0; }
  };
  struct Total_degree 
    : public std::unary_function< ICoeff , int > {
    int operator()(const ICoeff&) const { return 0; }
  };
    
  typedef Null_functor  Construct_polynomial;
  typedef Null_functor  Get_coefficient;
  typedef Null_functor  Leading_coefficient;
  typedef Null_functor  Univariate_content;
  typedef Null_functor  Multivariate_content;
  typedef Null_functor  Shift;
  typedef Null_functor  Negate;
  typedef Null_functor  Invert;
  typedef Null_functor  Translate;
  typedef Null_functor  Translate_homogeneous;
  typedef Null_functor  Scale_homogeneous;
  typedef Null_functor  Differentiate;
    
  struct Is_square_free
    : public std::unary_function< ICoeff, bool > {
    bool operator()( const ICoeff& ) const {
      return true;
    }
  };
    
  struct Make_square_free 
    : public std::unary_function< ICoeff, ICoeff>{
    ICoeff operator()( const ICoeff& x ) const {
      if (CGAL::is_zero(x)) return x ;
      else  return ICoeff(1);
    }
  };
        
  typedef Null_functor  Square_free_factorize;
  typedef Null_functor  Pseudo_division;
  typedef Null_functor  Pseudo_division_remainder;
  typedef Null_functor  Pseudo_division_quotient;

  struct Gcd_up_to_constant_factor 
    : public std::binary_function< ICoeff, ICoeff, ICoeff >{
    ICoeff operator()(const ICoeff& x, const ICoeff& y) const {
      if (CGAL::is_zero(x) && CGAL::is_zero(y)) 
        return ICoeff(0);
      else
        return ICoeff(1);
    }
  };

  typedef Null_functor Integral_division_up_to_constant_factor;

  struct Univariate_content_up_to_constant_factor
    : public std::unary_function< ICoeff, ICoeff >{
    ICoeff operator()(const ICoeff& ) const {
      // TODO: Why not return 0 if argument is 0 ? 
      return ICoeff(1);
    }
  };

  typedef Null_functor  Square_free_factorize_up_to_constant_factor;
  typedef Null_functor  Resultant;
  typedef Null_functor  Canonicalize;
  typedef Null_functor  Evaluate_homogeneous;
    
  struct Innermost_leading_coefficient
    :public std::unary_function <ICoeff, ICoeff>{
    ICoeff operator()(const ICoeff& x){return x;}
  };
    
  struct Degree_vector{
    typedef Exponent_vector         result_type;
    typedef Coefficient_type             argument_type;
    // returns the exponent vector of inner_most_lcoeff. 
    result_type operator()(const Coefficient_type&){
      return Exponent_vector();
    }
  };
    
  struct Get_innermost_coefficient 
    : public std::binary_function< ICoeff, Polynomial_d, Exponent_vector > {
        
    ICoeff operator()( const Polynomial_d& p, Exponent_vector ev ) {
      CGAL_precondition( ev.empty() );
      return p;
    }
  };
    
  typedef Null_functor Evaluate ;
    
  struct Substitute{
  public:
    template <class Input_iterator>
    typename 
    CGAL::Coercion_traits<
        typename std::iterator_traits<Input_iterator>::value_type,
                                   Innermost_coefficient_type>::Type
    operator()(
        const Innermost_coefficient_type& p, 
        Input_iterator begin, 
        Input_iterator end){ 
      CGAL_precondition(end == begin);
      typedef typename std::iterator_traits<Input_iterator>::value_type 
        value_type;
      typedef CGAL::Coercion_traits<Innermost_coefficient_type,value_type> CT;
      return typename CT::Cast()(p);
    }
  };

  struct Substitute_homogeneous{
  public:
    // this is the end of the recursion
    // begin contains the homogeneous variabel 
    // hdegree is the remaining degree 
    template <class Input_iterator>
    typename 
    CGAL::Coercion_traits<
        typename std::iterator_traits<Input_iterator>::value_type,
                                   Innermost_coefficient_type>::Type
    operator()(
        const Innermost_coefficient_type& p, 
        Input_iterator begin, 
        Input_iterator end,
        int hdegree){ 
      
      typedef typename std::iterator_traits<Input_iterator>::value_type 
        value_type;
      typedef CGAL::Coercion_traits<Innermost_coefficient_type,value_type> CT;
      typename CT::Type result =   
        typename CT::Cast()(CGAL::ipower(*begin++,hdegree)) 
        * typename CT::Cast()(p);

      CGAL_precondition(end == begin);
      CGAL_precondition(hdegree >= 0);
      return result;
    }
  };
};

// Now the version for the polynomials with all functors provided by all 
// polynomials
template< class Coefficient_type_,
          class ICoeffAlgebraicCategory, class PolynomialAlgebraicCategory >
class Polynomial_traits_d_base< Polynomial< Coefficient_type_ >,
          ICoeffAlgebraicCategory, PolynomialAlgebraicCategory >
  : public Polynomial_traits_d_base_icoeff_algebraic_category< 
        Polynomial< Coefficient_type_ >, ICoeffAlgebraicCategory >,
    public Polynomial_traits_d_base_polynomial_algebraic_category<
        Polynomial< Coefficient_type_ >, PolynomialAlgebraicCategory > {      
                 
  typedef Polynomial_traits_d< Polynomial< Coefficient_type_ > > PT;         
  typedef Polynomial_traits_d< Coefficient_type_ > PTC;                      
                                                                        
  public:                                                               
  typedef Polynomial<Coefficient_type_>                  Polynomial_d;       
  typedef Coefficient_type_                              Coefficient_type;        
                                                                        
  typedef typename Innermost_coefficient_type<Polynomial_d>::Type            
  Innermost_coefficient_type;                                                
  static const int d = Dimension<Polynomial_d>::value;                  
                                                                        
                                                                        
  private:                                                              
  typedef std::pair< Exponent_vector, Innermost_coefficient_type >           
  Exponents_coeff_pair;                                                 
  typedef std::vector< Exponents_coeff_pair > Monom_rep;                
                                                                        
  typedef CGAL::Recursive_const_flattening< d-1,                        
    typename CGAL::Polynomial<Coefficient_type>::const_iterator >            
  Coefficient_const_flattening;                                               
                                                                        
  public:                                                               
  typedef typename Coefficient_const_flattening::Recursive_flattening_iterator 
  Innermost_coefficient_const_iterator;                                       
  typedef typename  Polynomial_d::const_iterator Coefficient_const_iterator;       


  // We use our own Strict Weak Ordering predicate in order to avoid
  // problems when calling sort for a Exponents_coeff_pair where the
  // coeff type has no comparison operators available.
private:
  struct Compare_exponents_coeff_pair 
    : public std::binary_function< 
       std::pair< Exponent_vector, Innermost_coefficient_type >,
       std::pair< Exponent_vector, Innermost_coefficient_type >,
       bool > 
  {
    bool operator()( 
        const std::pair< Exponent_vector, Innermost_coefficient_type >& p1,
        const std::pair< Exponent_vector, Innermost_coefficient_type >& p2 ) const {
      // TODO: Precondition leads to an error within test_translate in 
      // Polynomial_traits_d test
      // CGAL_precondition( p1.first != p2.first );
      return p1.first < p2.first;
    }            
  }; 

public:

  //
  // Functors as defined in the reference manual 
  // (with sometimes slightly extended functionality)


  // Construct_polynomial;
  struct Construct_polynomial {
        
    typedef Polynomial_d  result_type;
        
    Polynomial_d operator()()  const {
      return Polynomial_d(0);
    }
        
    template <class T>
    Polynomial_d operator()( T a ) const {
      return Polynomial_d(a);
    }
        
    //! construct the constant polynomial a0
    Polynomial_d operator() (const Coefficient_type& a0) const
    {return Polynomial_d(a0);}
        
    //! construct the polynomial a0 + a1*x
    Polynomial_d operator() (
        const Coefficient_type& a0, const Coefficient_type& a1) const
    {return Polynomial_d(a0,a1);}
        
    //! construct the polynomial a0 + a1*x + a2*x^2
    Polynomial_d operator() (
        const Coefficient_type& a0, const Coefficient_type& a1,
        const Coefficient_type& a2) const
    {return Polynomial_d(a0,a1,a2);}
        
    //! construct the polynomial a0 + a1*x + ... + a3*x^3
    Polynomial_d operator() (
        const Coefficient_type& a0, const Coefficient_type& a1,
        const Coefficient_type& a2, const Coefficient_type& a3) const
    {return Polynomial_d(a0,a1,a2,a3);}
        
    //! construct the polynomial a0 + a1*x + ... + a4*x^4
    Polynomial_d operator() (
        const Coefficient_type& a0, const Coefficient_type& a1,
        const Coefficient_type& a2, const Coefficient_type& a3,
        const Coefficient_type& a4) const
    {return Polynomial_d(a0,a1,a2,a3,a4);}
        
    //! construct the polynomial a0 + a1*x + ... + a5*x^5
    Polynomial_d operator() (
        const Coefficient_type& a0, const Coefficient_type& a1,
        const Coefficient_type& a2, const Coefficient_type& a3,
        const Coefficient_type& a4, const Coefficient_type& a5) const
    {return Polynomial_d(a0,a1,a2,a3,a4,a5);}
        
    //! construct the polynomial a0 + a1*x + ... + a6*x^6
    Polynomial_d operator() (
        const Coefficient_type& a0, const Coefficient_type& a1,
        const Coefficient_type& a2, const Coefficient_type& a3,
        const Coefficient_type& a4, const Coefficient_type& a5, 
        const Coefficient_type& a6) const
    {return Polynomial_d(a0,a1,a2,a3,a4,a5,a6);}
        
    //! construct the polynomial a0 + a1*x + ... + a7*x^7
    Polynomial_d operator() (
        const Coefficient_type& a0, const Coefficient_type& a1,
        const Coefficient_type& a2, const Coefficient_type& a3,
        const Coefficient_type& a4, const Coefficient_type& a5, 
        const Coefficient_type& a6, const Coefficient_type& a7) const
    {return Polynomial_d(a0,a1,a2,a3,a4,a5,a6,a7);}
        
    //! construct the polynomial a0 + a1*x + ... + a8*x^8
    Polynomial_d operator() (
        const Coefficient_type& a0, const Coefficient_type& a1,
        const Coefficient_type& a2, const Coefficient_type& a3,
        const Coefficient_type& a4, const Coefficient_type& a5, 
        const Coefficient_type& a6, const Coefficient_type& a7,
        const Coefficient_type& a8) const 
    {return Polynomial_d(a0,a1,a2,a3,a4,a5,a6,a7,a8);}
        


    template< class Input_iterator >
    inline
    Polynomial_d construct( 
        Input_iterator begin, 
        Input_iterator end , 
        Tag_true) const {
      if(begin == end ) return Polynomial_d(0);
      return Polynomial_d(begin,end);
    }
        
    template< class Input_iterator >
    inline
    Polynomial_d construct( 
        Input_iterator begin, 
        Input_iterator end , 
        Tag_false) const {
      std::sort(begin,end,Compare_exponents_coeff_pair()); 
      return Create_polynomial_from_monom_rep< Coefficient_type >()( begin, end ); 
    }
        
    template< class Input_iterator >
    Polynomial_d 
    operator()( Input_iterator begin, Input_iterator end ) const {
      if(begin == end ) return Polynomial_d(0);
      typedef typename Input_iterator::value_type value_type;
      typedef Boolean_tag<boost::is_same<value_type,Coefficient_type>::value> 
        Is_coeff;
      std::vector<value_type> vec(begin,end);
      return construct(vec.begin(),vec.end(),Is_coeff());
    }

    
    template< class Input_iterator >
    Polynomial_d 
    operator()( Input_iterator begin, Input_iterator end , bool is_sorted) const {
      if(is_sorted) 
        return Create_polynomial_from_monom_rep< Coefficient_type >()( begin, end );
      else
        return (*this)(begin,end);
    }
    
  public: 

    template< class T >
    class Create_polynomial_from_monom_rep {
    public:
      template <class Monom_rep_iterator>
      Polynomial_d operator()( 
          Monom_rep_iterator begin,
          Monom_rep_iterator end) const {
                
        std::vector< Innermost_coefficient_type > coefficients;
        for(Monom_rep_iterator it = begin; it != end; it++){
          while( it->first[0] > (int) coefficients.size() ){
            coefficients.push_back(Innermost_coefficient_type(0));
          }
          coefficients.push_back(it->second);
        }
        return Polynomial_d(coefficients.begin(),coefficients.end());
      }
    };
    template< class T >
    class Create_polynomial_from_monom_rep< Polynomial < T > > {
    public:
      template <class Monom_rep_iterator>
      Polynomial_d operator()( 
          Monom_rep_iterator begin,
          Monom_rep_iterator end) const {
                
        typedef Polynomial_traits_d<Coefficient_type> PT;
        typename PT::Construct_polynomial construct;
                
        BOOST_STATIC_ASSERT(PT::d != 0); // Coefficient_type is a Polynomial
        std::vector<Coefficient_type> coefficients;
                
        Monom_rep_iterator it = begin; 
        while(it != end){
          int current_exp = it->first[PT::d];
          // fill up with zeros until current exp is reached
          while( (int) coefficients.size() < current_exp){
            coefficients.push_back(Coefficient_type(0));
          }
          // collect all coeffs for this exp
          Monom_rep monoms; 
          while(  it != end && it->first[PT::d] == current_exp ){
            Exponent_vector ev = it->first;
            ev.pop_back();
            monoms.push_back( Exponents_coeff_pair(ev,it->second));
            it++;
          }                    
          coefficients.push_back(
              construct(monoms.begin(), monoms.end()));
        }
        return Polynomial_d(coefficients.begin(),coefficients.end());
      }
    };
  };

  //       Get_coefficient;
  struct Get_coefficient 
    : public std::binary_function<Polynomial_d, int, Coefficient_type > {
        
    Coefficient_type operator()( const Polynomial_d& p, int i) const {
      CGAL_precondition( i >= 0 );
      typename PT::Degree degree;
      if( i >  degree(p) )
        return Coefficient_type(0);
      return p[i];
    }
    
    Coefficient_type operator()( const Polynomial_d& p, int i, int index) const {
      CGAL_precondition(0 <= index && index < d);
      return (*this)(Swap()(p,index,d-1),i);
    }       
  };
    
  //     Get_innermost_coefficient;
  struct Get_innermost_coefficient
    : public 
    std::binary_function< Polynomial_d, Exponent_vector, Innermost_coefficient_type >
  {
        
    Innermost_coefficient_type 
    operator()( const Polynomial_d& p, Exponent_vector ev ) const {
      CGAL_precondition( !ev.empty() );
      typename PTC::Get_innermost_coefficient gic;
      typename PT::Get_coefficient gc;
      int exponent = ev.back();
      ev.pop_back();
      return gic( gc( p, exponent ), ev ); 
    }; 
  };
     
  // Swap variable x_i with x_j
  struct Swap {
    typedef Polynomial_d        result_type;  
    typedef Polynomial_d        first_argument_type;
    typedef int                 second_argument_type;
    typedef int                 third_argument_type;
             
  public:

    Polynomial_d operator()(const Polynomial_d& p, int i, int j ) const {
      CGAL_precondition(0 <= i && i < d);
      CGAL_precondition(0 <= j && j < d);
      typedef std::pair< Exponent_vector, Innermost_coefficient_type >
        Exponents_coeff_pair;
      typedef std::vector< Exponents_coeff_pair > Monom_rep; 
      Get_monom_representation gmr;
      typename Construct_polynomial
        ::template Create_polynomial_from_monom_rep< Coefficient_type > construct;
      Monom_rep mon_rep;
      gmr( p, std::back_inserter( mon_rep ) );
      for( typename Monom_rep::iterator it = mon_rep.begin(); 
           it != mon_rep.end();
           ++it ) {
        std::swap(it->first[i],it->first[j]);
        // it->first.swap( i, j );
      }
      std::sort
        ( mon_rep.begin(), mon_rep.end(), Compare_exponents_coeff_pair() );
      return construct( mon_rep.begin(), mon_rep.end() );
    }
  };

  //       Move;    
  // move variable x_i to position of x_j
  // order of other variables remains 
  // default j = d makes x_i the othermost variable
  struct Move {
    typedef Polynomial_d        result_type;  
    typedef Polynomial_d        first_argument_type;
    typedef int                 second_argument_type;
    typedef int                 third_argument_type;
        
    Polynomial_d 
    operator()(const Polynomial_d& p, int i, int j = (d-1) ) const {
      CGAL_precondition(0 <= i && i < d);
      CGAL_precondition(0 <= j && j < d);
      typedef std::pair< Exponent_vector, Innermost_coefficient_type >
        Exponents_coeff_pair;
      typedef std::vector< Exponents_coeff_pair > Monom_rep; 
      Get_monom_representation gmr;
      Construct_polynomial construct;
      Monom_rep mon_rep;
      gmr( p, std::back_inserter( mon_rep ) );
      for( typename Monom_rep::iterator it = mon_rep.begin(); 
           it != mon_rep.end();
           ++it ) {
        // this is as good as std::rotate since it uses swap also
        if (i < j) 
          for( int k = i; k < j; k++ )
            std::swap(it->first[k],it->first[k+1]);
        else
          for( int k = i; k > j; k-- )
            std::swap(it->first[k],it->first[k-1]);
                
      }
      std::sort
        ( mon_rep.begin(), mon_rep.end(),Compare_exponents_coeff_pair());
      return construct( mon_rep.begin(), mon_rep.end() );
    }
  };

  //       Degree;    
  struct Degree : public std::unary_function< Polynomial_d , int  >{
    int operator()(const Polynomial_d& p, int i = (d-1)) const {      
      if (i == (d-1)) return p.degree();                           
      else return Swap()(p,i,d-1).degree();
    }     
  };

  //       Total_degree;
  struct Total_degree : public std::unary_function< Polynomial_d , int >{
    int operator()(const Polynomial_d& p) const {
      typedef Polynomial_traits_d<Coefficient_type> COEFF_POLY_TRAITS;
      typename COEFF_POLY_TRAITS::Total_degree total_degree;
      Degree degree;
      CGAL_precondition( degree(p) >= 0);

      int result = 0;
      for(int i = 0; i <= degree(p) ; i++){
        if( ! CGAL::is_zero( p[i]) )
          result = std::max(result , total_degree(p[i]) + i );
      } 
      return result;
    }
  };

  //       Leading_coefficient;
  struct Leading_coefficient 
    : public std::unary_function< Polynomial_d , Coefficient_type>{
    Coefficient_type operator()(const Polynomial_d& p) const {
      return p.lcoeff();
    }
    Coefficient_type operator()(Polynomial_d p, int i) const {
      return Swap()(p,i,PT::d-1).lcoeff();
    }
  };
    
  //       Innermost_leading_coefficient;
  struct Innermost_leading_coefficient 
    : public std::unary_function< Polynomial_d , Innermost_coefficient_type>{
    Innermost_coefficient_type 
    operator()(const Polynomial_d& p) const {
      typename PTC::Innermost_leading_coefficient ilcoeff;
      typename PT::Leading_coefficient lcoeff;
      return ilcoeff(lcoeff(p));
    }
  };

  //       Canonicalize;
  struct Canonicalize
    : public std::unary_function<Polynomial_d, Polynomial_d>{
    Polynomial_d
    operator()( const Polynomial_d& p ) const {
      return CGAL::CGALi::canonicalize_polynomial(p);
    }  
  };

  //       Differentiate;
  struct Differentiate 
    : public std::unary_function<Polynomial_d, Polynomial_d>{
    Polynomial_d
    operator()(Polynomial_d p, int i = (d-1)) const {
      if (i == (d-1) ){
        p.diff();
      }else{
        Swap swap;
        p = swap(p,i,d-1);
        p.diff();
        p = swap(p,i,d-1);
      }
      return p;
    }
  };

  // Evaluate;
  struct Evaluate
    :public std::unary_function<Polynomial_d,Coefficient_type>{
    // Evaluate with respect to one variable 
    Coefficient_type
    operator()(const Polynomial_d& p, Coefficient_type x) const {
      return p.evaluate(x);
    }
    Coefficient_type 
    operator()(const Polynomial_d& p, Coefficient_type x, int i ) const {
      return Move()(p,i).evaluate(x);
    } 
#define ICOEFF typename First_if_different<Innermost_coefficient_type, Coefficient_type>::Type 
    Coefficient_type operator()
      ( const Polynomial_d& p, const ICOEFF& x) const 
    {
      return p.evaluate(x);
    }
    Coefficient_type operator()
      (const Polynomial_d& p, const ICOEFF& x, int i) const 
    {
      return Move()(p,i,PT::d-1).evaluate(x);
    }
#undef ICOEFF      
  };
  
  // Evaluate_homogeneous;
  struct Evaluate_homogeneous{
    typedef Coefficient_type           result_type;  
    typedef Polynomial_d          first_argument_type;
    typedef Coefficient_type           second_argument_type;
    typedef Coefficient_type           third_argument_type;
       
    Coefficient_type operator()(
        const Polynomial_d& p, Coefficient_type a, Coefficient_type b) const 
    {
      return p.evaluate_homogeneous(a,b);
    }
    Coefficient_type operator()(
        const Polynomial_d& p, Coefficient_type a, Coefficient_type b, int i) const 
    {
      return Move()(p,i,PT::d-1).evaluate_homogeneous(a,b);
    }

#define ICOEFF typename First_if_different<Innermost_coefficient_type, Coefficient_type>::Type 
    Coefficient_type operator()
      ( const Polynomial_d& p, const ICOEFF& a, const ICOEFF& b) const 
    {
      return p.evaluate_homogeneous(a,b);
    }
    Coefficient_type operator()
      (const Polynomial_d& p, const ICOEFF& a, const ICOEFF& b, int i) const 
    {
      return Move()(p,i,PT::d-1).evaluate_homogeneous(a,b);
    }
#undef ICOEFF    

  };
    
  // Is_zero_at;
  struct Is_zero_at {
  private:
    typedef Algebraic_structure_traits<Innermost_coefficient_type> AST;
    typedef typename AST::Is_zero::result_type BOOL;
  public:
    typedef BOOL result_type;
        
    template< class Input_iterator >
    BOOL operator()( 
        const Polynomial_d& p, 
        Input_iterator begin, 
        Input_iterator end ) const {
      typename PT::Substitute substitute;            
      return( CGAL::is_zero( substitute( p, begin, end ) ) );
    } 
  };
    
  //  Is_zero_at_homogeneous;
  struct Is_zero_at_homogeneous {
 private:
    typedef Algebraic_structure_traits<Innermost_coefficient_type> AST;
    typedef typename AST::Is_zero::result_type BOOL;
  public:
    typedef BOOL result_type;
        
    template< class Input_iterator >
    BOOL operator()
      ( const Polynomial_d& p, Input_iterator begin, Input_iterator end ) const 
    {
      typename PT::Substitute_homogeneous substitute_homogeneous;
      return( CGAL::is_zero( substitute_homogeneous( p, begin, end ) ) );
    }
  };

  // Sign_at, Sign_at_homogeneous, Compare 
  // define XXX_ even though ICoeff may not be Real_embeddable 
  // select propoer XXX among XXX_ or Null_functor using ::boost::mpl::if_
private:
  struct Sign_at_ {
  private:
    typedef Real_embeddable_traits<Innermost_coefficient_type> RT;
  public:
    typedef typename RT::Sign result_type;

    template< class Input_iterator >
    result_type operator()( 
        const Polynomial_d& p, 
        Input_iterator begin, 
        Input_iterator end ) const 
    {
      typename PT::Substitute substitute;
      return CGAL::sign( substitute( p, begin, end ) );
    }
  };
  
  struct Sign_at_homogeneous_ {
    typedef Real_embeddable_traits<Innermost_coefficient_type> RT;
  public:
    typedef typename RT::Sign result_type;
        
    template< class Input_iterator >
    result_type operator()( 
        const Polynomial_d& p, 
        Input_iterator begin, 
        Input_iterator end) const {
      typename PT::Substitute_homogeneous substitute_homogeneous;
      return CGAL::sign( substitute_homogeneous( p, begin, end ) );
    }
  };
    
  // Compare;
  struct Compare_
    : public std::binary_function< Comparison_result, Polynomial_d, Polynomial_d > {
    Comparison_result operator()
      ( const Polynomial_d& p1, const Polynomial_d& p2 ) const {
      return p1.compare( p2 );
    }
  };
 
  typedef Real_embeddable_traits<Innermost_coefficient_type> RET_IC;
  typedef typename RET_IC::Is_real_embeddable IC_is_real_embeddable;
public:
  typedef typename ::boost::mpl::if_<IC_is_real_embeddable,Sign_at_,Null_functor>::type Sign_at; 
  typedef typename ::boost::mpl::if_<IC_is_real_embeddable,Sign_at_homogeneous_,Null_functor>::type Sign_at_homogeneous; 
  typedef typename ::boost::mpl::if_<IC_is_real_embeddable,Compare_,Null_functor>::type Compare; 
 


  // This is going to be in PolynomialToolBox 
  struct Coefficient_const_begin                                                  
    : public std::unary_function< Polynomial_d, Coefficient_const_iterator > {       
    Coefficient_const_iterator                                                  
    operator () (const Polynomial_d& p) { return p.begin(); }             
  };                                                                        
  struct Coefficient_const_end                                                    
    : public std::unary_function< Polynomial_d, Coefficient_const_iterator > {       
    Coefficient_const_iterator                                                  
    operator () (const Polynomial_d& p) { return p.end(); }               
  };                                                                        
                                                                              
  struct Innermost_coefficient_const_begin                                        
    : public std::unary_function< Polynomial_d, Innermost_coefficient_const_iterator > {  
    Innermost_coefficient_const_iterator                                             
    operator () (const Polynomial_d& p) {                                      
      return typename Coefficient_const_flattening::Flatten()(p.end(),p.begin());  
    }                                                                          
  };                           
  
  struct Innermost_coefficient_const_end
    : public std::unary_function< Polynomial_d, Innermost_coefficient_const_iterator > {
    Innermost_coefficient_const_iterator
    operator () (const Polynomial_d& p) {
      return typename Coefficient_const_flattening::Flatten()(p.end(),p.end());
    }
  };
  
  struct Is_square_free 
    : public std::unary_function< Polynomial_d, bool >{
    bool operator()( const Polynomial_d& p ) const {
      if( !CGALi::may_have_multiple_factor( p ) )
        return true;
            
      Univariate_content_up_to_constant_factor ucontent_utcf;
      Integral_division_up_to_constant_factor  idiv_utcf;
      Differentiate diff;
            
      Coefficient_type content = ucontent_utcf( p );
      typename PTC::Is_square_free isf;
            
      if( !isf( content ) )
        return false;
            
      Polynomial_d regular_part = idiv_utcf( p, Polynomial_d( content ) ); 
            
      Polynomial_d g = gcd_utcf(regular_part,diff(regular_part));
      return ( g.degree() == 0 );
    }
  };

                   
  struct Make_square_free 
    : public std::unary_function< Polynomial_d, Polynomial_d >{
    Polynomial_d
    operator()(const Polynomial_d& p) const {
      if (CGAL::is_zero(p)) return p;
      Univariate_content_up_to_constant_factor ucontent_utcf;
      Integral_division_up_to_constant_factor  idiv_utcf;
      Differentiate diff;
      typename PTC::Make_square_free msf;
            
      Coefficient_type content = ucontent_utcf(p);
      Polynomial_d result = Polynomial_d(msf(content));
            
      Polynomial_d regular_part = idiv_utcf(p,Polynomial_d(content));
      Polynomial_d g = gcd_utcf(regular_part,diff(regular_part));
            
            
      result *= idiv_utcf(regular_part,g);
      return Canonicalize()(result);
                    
    }
  };

  //       Pseudo_division;
  struct Pseudo_division {
    typedef Polynomial_d        result_type;  
    void
    operator()(
        const Polynomial_d& f, const Polynomial_d& g,
        Polynomial_d& q, Polynomial_d& r, Coefficient_type& D) const {
      Polynomial_d::pseudo_division(f,g,q,r,D);
    }
  };
    
  struct Pseudo_division_quotient
    :public std::binary_function<Polynomial_d, Polynomial_d, Polynomial_d> {
        
    Polynomial_d
    operator()(const Polynomial_d& f, const Polynomial_d& g) const {
      Polynomial_d q,r;
      Coefficient_type D;
      Polynomial_d::pseudo_division(f,g,q,r,D);
      return q;
    }
  };

  struct Pseudo_division_remainder
    :public std::binary_function<Polynomial_d, Polynomial_d, Polynomial_d> {
        
    Polynomial_d
    operator()(const Polynomial_d& f, const Polynomial_d& g) const {
      Polynomial_d q,r;
      Coefficient_type D;
      Polynomial_d::pseudo_division(f,g,q,r,D);
      return r;
    }
  };
    
  struct Gcd_up_to_constant_factor
    :public std::binary_function<Polynomial_d, Polynomial_d, Polynomial_d> {
    Polynomial_d
    operator()(const Polynomial_d& p, const Polynomial_d& q) const {
      if (CGAL::is_zero(p) && CGAL::is_zero(q)){
        return Polynomial_d(0);
      }
      // apply modular filter first
      if (CGALi::may_have_common_factor(p,q)){
        return CGALi::gcd_utcf(p,q);
      }else{
        return Polynomial_d(1);
      }
    }
  };
    
  struct Integral_division_up_to_constant_factor
    :public std::binary_function<Polynomial_d, Polynomial_d, Polynomial_d> {
    Polynomial_d
    operator()(const Polynomial_d& p, const Polynomial_d& q) const {
      typedef Innermost_coefficient_type IC;

      typename PT::Construct_polynomial construct;
      typename PT::Innermost_leading_coefficient ilcoeff;
      typename PT::Innermost_coefficient_const_begin begin;
      typename PT::Innermost_coefficient_const_end end;
      typedef Algebraic_extension_traits<Innermost_coefficient_type> AET;
      typename AET::Denominator_for_algebraic_integers dfai;
      typename AET::Normalization_factor nfac;

            
      IC ilcoeff_q = ilcoeff(q);
      // this factor is needed in case IC is an Algebraic extension
      IC dfai_q = dfai(begin(q), end(q));
      // make dfai_q a 'scalar'
      ilcoeff_q *= dfai_q * nfac(dfai_q);
            
      Polynomial_d result = (p * construct(ilcoeff_q)) / q;

      return Canonicalize()(result);
    }
  };
    
  struct Univariate_content_up_to_constant_factor
    :public std::unary_function<Polynomial_d, Coefficient_type> {
    Coefficient_type
    operator()(const Polynomial_d& p) const {
      typename PTC::Gcd_up_to_constant_factor gcd_utcf;
            
      if(CGAL::is_zero(p)) return Coefficient_type(0);
      if(PT::d == 1) return Coefficient_type(1);

      Coefficient_type result(0);
      for(typename Polynomial_d::const_iterator it = p.begin();
          it != p.end();
          it++){
        result = gcd_utcf(*it,result);
      }
      return result;

    }
  };

  struct Square_free_factorize_up_to_constant_factor {
  private:
    typedef Coefficient_type Coeff;
    typedef Innermost_coefficient_type ICoeff;
        
    // rsqff_utcf computes the sqff recursively for Coeff  
    // end of recursion: ICoeff
    
    template < class OutputIterator >
    OutputIterator rsqff_utcf ( ICoeff , OutputIterator oi) const{ 
      return oi;
    }        
    
    template < class OutputIterator >
    OutputIterator rsqff_utcf (
        typename First_if_different<Coeff,ICoeff>::Type c,
        OutputIterator                                 oi) const {
      
      typename PTC::Square_free_factorize_up_to_constant_factor sqff;
      std::vector<std::pair<Coefficient_type,int> > fac_mul_pairs;
      sqff(c,std::back_inserter(fac_mul_pairs));
      
      for(unsigned int i = 0; i < fac_mul_pairs.size(); i++){
        Polynomial_d factor(fac_mul_pairs[i].first);
        int mult = fac_mul_pairs[i].second;
        *oi++=std::make_pair(factor,mult);
      }
      return oi;
    }

  public:
    template < class OutputIterator>
    OutputIterator 
    operator()(Polynomial_d p, OutputIterator oi) const {            
      if (CGAL::is_zero(p)) return oi;

      Univariate_content_up_to_constant_factor ucontent_utcf;
      Integral_division_up_to_constant_factor idiv_utcf;
      Coefficient_type c = ucontent_utcf(p);
      
      p = idiv_utcf( p , Polynomial_d(c));
      std::vector<Polynomial_d> factors;
      std::vector<int> mults;
      square_free_factorize_utcf(
          p, std::back_inserter(factors), std::back_inserter(mults));
      for(unsigned int i = 0; i < factors.size() ; i++){
         *oi++=std::make_pair(factors[i],mults[i]);
      }
      if (CGAL::total_degree(c) == 0)
        return oi;
      else
        return rsqff_utcf(c,oi);
    }
  };

  struct Shift
    : public std::unary_function< Polynomial_d, Polynomial_d >{
    
    Polynomial_d 
    operator()(const Polynomial_d& p, int e, int i = (d-1)) const {
      Construct_polynomial construct; 
      Get_monom_representation gmr; 
      Monom_rep monom_rep;
      gmr(p,std::back_inserter(monom_rep));
      for(typename Monom_rep::iterator it = monom_rep.begin(); 
          it != monom_rep.end();
          it++){
        it->first[i]+=e;
      }
      return construct(monom_rep.begin(), monom_rep.end());
    }
  };
    
  struct Negate
    : public std::unary_function< Polynomial_d, Polynomial_d >{
        
    Polynomial_d operator()(const Polynomial_d& p, int i = (d-1)) const {
      Construct_polynomial construct; 
      Get_monom_representation gmr; 
      Monom_rep monom_rep;
      gmr(p,std::back_inserter(monom_rep));
      for(typename Monom_rep::iterator it = monom_rep.begin(); 
          it != monom_rep.end();
          it++){
        if (it->first[i] % 2 != 0) 
          it->second = - it->second; 
      }
      return construct(monom_rep.begin(), monom_rep.end());
    }
  };

  struct Invert
    : public std::unary_function< Polynomial_d , Polynomial_d >{
    Polynomial_d operator()(Polynomial_d p, int i = (PT::d-1)) const {
      if (i == (d-1)){
        p.reversal(); 
      }else{
        p =  Swap()(p,i,PT::d-1);
        p.reversal();
        p = Swap()(p,i,PT::d-1);   
      }
      return p ;
    }
  };

  struct Translate
    : public std::binary_function< Polynomial_d , Polynomial_d, 
                                  Innermost_coefficient_type >{
    Polynomial_d
    operator()(
        Polynomial_d p, 
        const Innermost_coefficient_type& c, 
        int i = (d-1)) 
      const {
      if (i == (d-1) ){
        p.translate(Coefficient_type(c)); 
      }else{
        Swap swap;
        p = swap(p,i,d-1);
        p.translate(Coefficient_type(c));
        p = swap(p,i,d-1); 
      }
      return p;
    }
  };

  struct Translate_homogeneous{
    typedef Polynomial_d result_type;
    typedef Polynomial_d first_argument_type;
    typedef Innermost_coefficient_type second_argument_type;
    typedef Innermost_coefficient_type third_argument_type;
        
    Polynomial_d
    operator()(Polynomial_d p, 
        const Innermost_coefficient_type& a, 
        const Innermost_coefficient_type& b,
        int i = (d-1) ) const {
      if (i == (d-1) ){
        p.translate(Coefficient_type(a), Coefficient_type(b) );  
      }else{
        Swap swap;
        p = swap(p,i,d-1);
        p.translate(Coefficient_type(a), Coefficient_type(b));
        p = swap(p,i,d-1);
      }
      return p;
    }
  };

  struct Scale 
    : public 
    std::binary_function< Polynomial_d, Innermost_coefficient_type, Polynomial_d > {
        
    Polynomial_d operator()( Polynomial_d p, const Innermost_coefficient_type& c,
        int i = (PT::d-1) ) {
      typename PT::Scale_homogeneous scale_homogeneous;          
      return scale_homogeneous( p, c, Innermost_coefficient_type(1), i );
    }
        
  };
    
  struct Scale_homogeneous{
    typedef Polynomial_d result_type;
    typedef Polynomial_d first_argument_type;
    typedef Innermost_coefficient_type second_argument_type;
    typedef Innermost_coefficient_type third_argument_type;
        
    Polynomial_d
    operator()(
        Polynomial_d p, 
        const Innermost_coefficient_type& a, 
        const Innermost_coefficient_type& b,
        int i = (d-1) ) const {
      CGAL_precondition( ! CGAL::is_zero(b) );
            
      if (i == (d-1) ) p = Swap()(p,i,d-1);
          
      if(CGAL::is_one(b)) 
        p.scale_up(Coefficient_type(a));
      else 
        if(CGAL::is_one(a)) 
          p.scale_down(Coefficient_type(b));
        else 
          p.scale(Coefficient_type(a), Coefficient_type(b) );  
          
      if (i == (d-1) ) p = Swap()(p,i,d-1);
          
      return p;
    }
  };

  struct Resultant
    : public std::binary_function<Polynomial_d, Polynomial_d, Coefficient_type>{
        
    Coefficient_type
    operator()(
        const Polynomial_d& p, 
        const Polynomial_d& q,
        int i = (d-1) ) const {
      // make i the innermost variabl call CGALi::resultant_
      // CGALi::resultant would eliminate the outermost variable. 
      if(i == 0 )
        return CGALi::resultant_(p,q);
      else
        return CGALi::resultant_(Move()(p,i,0),Move()(q,i,0));
    }  
  };


  //       Polynomial subresultants (aka subresultant polynomials)
  struct Polynomial_subresultants {
        
    template<typename OutputIterator>
    OutputIterator operator()(
        const Polynomial_d& p, 
        const Polynomial_d& q,
        OutputIterator out,
        int i = (d-1) ) const {
      if(i == (d-1) )
        return CGAL::CGALi::polynomial_subresultants(p,q,out);
      else
        return CGAL::CGALi::polynomial_subresultants(Move()(p,i),
            Move()(q,i),
            out);
    }  
  };

  //       principal subresultants (aka scalar subresultants)
  struct Principal_subresultants {
        
    template<typename OutputIterator>
    OutputIterator operator()(
        const Polynomial_d& p, 
        const Polynomial_d& q,
        OutputIterator out,
        int i = (d-1) ) const {
      if(i == (d-1) )
        return CGAL::CGALi::principal_subresultants(p,q,out);
      else
        return CGAL::CGALi::principal_subresultants(Move()(p,i),
            Move()(q,i),
            out);
    }  
  };

  //       Sturm-Habicht sequence (aka signed subresultant sequence)
  struct Sturm_habicht_sequence {
        
    template<typename OutputIterator>
    OutputIterator operator()(
        const Polynomial_d& p, 
        OutputIterator out,
        int i = (d-1) ) const {
      if(i == (d-1) )
        return CGAL::CGALi::sturm_habicht_sequence(p,out);
      else
        return CGAL::CGALi::sturm_habicht_sequence(Move()(p,i),
            out);
    }  
  };

  //       Sturm-Habicht sequence with cofactors
  struct Sturm_habicht_sequence_with_cofactors {
    template<typename OutputIterator1,
                 typename OutputIterator2,
                 typename OutputIterator3>
    OutputIterator1 operator()(
        const Polynomial_d& p, 
        OutputIterator1 out_stha,
        OutputIterator2 out_f,
        OutputIterator3 out_fx,
        int i = (d-1) ) const {
      if(i == (d-1) )
        return CGAL::CGALi::sturm_habicht_sequence_with_cofactors
          (p,out_stha,out_f,out_fx);
      else
        return CGAL::CGALi::sturm_habicht_sequence_with_cofactors
          (Move()(p,i),out_stha,out_f,out_fx);
    }  
  };
    
  //       Principal Sturm-Habicht sequence (formal leading coefficients
  //       of Sturm-Habicht sequence)
  struct Principal_sturm_habicht_sequence {
        
    template<typename OutputIterator>
    OutputIterator operator()(
        const Polynomial_d& p, 
        OutputIterator out,
        int i = (d-1) ) const {
      if(i == (d-1) )
        return CGAL::CGALi::principal_sturm_habicht_sequence(p,out);
      else
        return CGAL::CGALi::principal_sturm_habicht_sequence
          (Move()(p,i),out);
    }  
  };
    
    
  //
  // Functors not mentioned in the reference manual
  //

  struct Get_monom_representation {      
    typedef std::pair< Exponent_vector, Innermost_coefficient_type >
    Exponents_coeff_pair;
    //typedef std::vector< Exponents_coeff_pair > Monom_rep; 
        
    
    template <class OutputIterator>
    void operator()( const Polynomial_d& p, OutputIterator oit ) const {
      if(CGAL::is_zero(p)){
        *oit= make_pair(Exponent_vector(d,0), Innermost_coefficient_type(0));
        oit++;
        return;
      }
     
      typedef Boolean_tag< d == 1 > Is_univariat;
      create_monom_representation( p, oit , Is_univariat());
    }
      
  private:
        
    template <class OutputIterator>
    void
    create_monom_representation 
    ( const Polynomial_d& p, OutputIterator oit, Tag_true ) const{
      for( int exponent = 0; exponent <= p.degree(); ++exponent ) {
        if ( p[exponent] != Coefficient_type(0) ){
          Exponent_vector exp_vec;
          exp_vec.push_back( exponent );
          *oit = Exponents_coeff_pair( exp_vec, p[exponent] ); 
        }
      } 
    } 
    template <class OutputIterator>
    void 
    create_monom_representation 
    ( const Polynomial_d& p, OutputIterator oit, Tag_false ) const { 
      for( int exponent = 0; exponent <= p.degree(); ++exponent ) {
        Monom_rep monom_rep;
        typedef Polynomial_traits_d< Coefficient_type > PT;
        typename PT::Get_monom_representation gmr;
        gmr( p[exponent], std::back_inserter( monom_rep ) );
        for( typename Monom_rep::iterator it = monom_rep.begin();
             it != monom_rep.end(); ++it ) {
          it->first.push_back( exponent );
        }
        copy( monom_rep.begin(), monom_rep.end(), oit );               
      }
    }
  };

  // returns the Exponten_vector of the innermost leading coefficient 
  struct Degree_vector{
    typedef Exponent_vector           result_type;
    typedef Polynomial_d              argument_type;
        
    // returns the exponent vector of inner_most_lcoeff. 
    result_type operator()(const Polynomial_d& polynomial){
      typename PTC::Degree_vector degree_vector;
      Exponent_vector result = degree_vector(polynomial.lcoeff());
      result.push_back(polynomial.degree());
      return result;
    }
  };

    // substitute every variable by its new value in the iterator range
  // begin refers to the innermost/first variable
  struct Substitute{
  public:
    template <class Input_iterator>
    typename CGAL::Coercion_traits<
         typename std::iterator_traits<Input_iterator>::value_type, 
         Innermost_coefficient_type
    >::Type
    operator()(
        const Polynomial_d& p, 
        Input_iterator begin, 
        Input_iterator end){
      typedef typename std::iterator_traits<Input_iterator> ITT;
      typedef typename ITT::iterator_category  Category; 
      return (*this)(p,begin,end,Category()); 
    }
        
    template <class Input_iterator>
    typename CGAL::Coercion_traits< 
         typename std::iterator_traits<Input_iterator>::value_type, 
         Innermost_coefficient_type
    >::Type
    operator()(
        const Polynomial_d& p, 
        Input_iterator begin, 
        Input_iterator end,
        std::forward_iterator_tag){
      typedef typename std::iterator_traits<Input_iterator> ITT;
      std::list<typename ITT::value_type> list(begin,end); 
      return (*this)(p,list.begin(),list.end()); 
    }
        
    template <class Input_iterator>
    typename 
    CGAL::Coercion_traits
    <typename std::iterator_traits<Input_iterator>::value_type, 
         Innermost_coefficient_type>::Type
    operator()(
        const Polynomial_d& p, 
        Input_iterator begin, 
        Input_iterator end,
        std::bidirectional_iterator_tag){
            
      typedef typename std::iterator_traits<Input_iterator>::value_type 
        value_type;
      typedef CGAL::Coercion_traits<Innermost_coefficient_type,value_type> CT;
      typename PTC::Substitute subs; 

      typename CT::Type x = typename CT::Cast()(*(--end));  
            
      int i = Degree()(p);
      typename CT::Type y = 
        subs(Get_coefficient()(p,i),begin,end);
            
      while (--i >= 0){
        y *= x;
        y += subs(Get_coefficient()(p,i),begin,end);
      }
      return y;  
    }        
  };  // substitute every variable by its new value in the iterator range


  
  // begin refers to the innermost/first variable
  struct Substitute_homogeneous{

    template<typename Input_iterator>
    struct Result_type{
      typedef std::iterator_traits<Input_iterator> ITT;
      typedef typename ITT::value_type value_type;
      typedef Coercion_traits<value_type, Innermost_coefficient_type> CT; 
      typedef typename CT::Type Type; 
    };
    
  public:
    
    template <class Input_iterator>
    typename Result_type<Input_iterator>::Type
    operator()( const Polynomial_d& p, Input_iterator begin, Input_iterator end){
      int hdegree = Total_degree()(p);

      typedef std::iterator_traits<Input_iterator> ITT;
      std::list<typename ITT::value_type> list(begin,end); 
      
      // make the homogeneous variable the first in the list
      list.push_front(list.back());
      list.pop_back();
      
      // reverse and begin with the outermost variable 
      return (*this)(p, list.rbegin(), list.rend(), hdegree); 
    }
      
    // this operator is undcoumented and for internal use:
    // the iterator range starts with the outermost variable 
    // and ends with the homogeneous variable 
    template <class Input_iterator>
    typename Result_type<Input_iterator>::Type
    operator()(
        const Polynomial_d& p, 
        Input_iterator begin, 
        Input_iterator end,
        int hdegree){
      
      
      typedef std::iterator_traits<Input_iterator> ITT;
      typedef typename ITT::value_type value_type;
      typedef Coercion_traits<value_type, Innermost_coefficient_type> CT; 
      typedef typename CT::Type Type; 
      
      typename PTC::Substitute_homogeneous subsh; 
      
      typename CT::Type x = typename CT::Cast()(*begin++);  
            

      int i = Degree()(p);
      typename CT::Type y = subsh(Get_coefficient()(p,i),begin,end, hdegree-i);
       
      while (--i >= 0){
        y *= x;
        y += subsh(Get_coefficient()(p,i),begin,end,hdegree-i);
      }
      return y;  
    }        
  };

};

} // namespace CGALi

// Definition of Polynomial_traits_d
//
// In order to determine the algebraic category of the innermost coefficient,
// the Polynomial_traits_d_base class with "Null_tag" is used.

template< class Polynomial >
class Polynomial_traits_d
  : public CGALi::Polynomial_traits_d_base< Polynomial,  
    typename Algebraic_structure_traits<
typename CGALi::Innermost_coefficient_type<Polynomial>::Type >::Algebraic_category,
    typename Algebraic_structure_traits< Polynomial >::Algebraic_category > ,
  public Algebraic_structure_traits<Polynomial>{

//------------ Rebind ----------- 
private:
  template <class T, int d>
  struct Gen_polynomial_type{
    typedef CGAL::Polynomial<typename Gen_polynomial_type<T,d-1>::Type> Type;
  };
  template <class T>
  struct Gen_polynomial_type<T,0>{ typedef T Type; };

public:
  template <class T, int d>
  struct Rebind{
    typedef Polynomial_traits_d<typename Gen_polynomial_type<T,d>::Type> Other;
  };
//------------ Rebind ----------- 
};


// functor adapting functions for some functors (undocumented)
namespace CGALi {

template< class Polynomial >
Polynomial make_square_free( const Polynomial& p ) {
  return typename CGAL::Polynomial_traits_d< Polynomial>::
    Make_square_free()( p );
}

template< class Polynomial > 
bool is_square_free( const Polynomial& p ) {
  return typename CGAL::Polynomial_traits_d< Polynomial>::
    Is_square_free()( p );
}

} // namespace CGALi


CGAL_END_NAMESPACE

#endif // CGAL_POLYNOMIAL_TRAITS_D_H
