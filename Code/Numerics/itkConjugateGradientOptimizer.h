/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkConjugateGradientOptimizer.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$


  Copyright (c) 2000 National Library of Medicine
  All rights reserved.

  See COPYRIGHT.txt for copyright details.

=========================================================================*/
#ifndef __itkConjugateGradientOptimizer_h
#define __itkConjugateGradientOptimizer_h

#include "itkSingleValuedNonLinearOptimizer.h"
#include "vnl/algo/vnl_conjugate_gradient.h"

namespace itk
{
  
/** \class ConjugateGradientOptimizer
 * \brief Wrap of the vnl_conjugate_gradient 
 *
 */

  
template <class TCostFunction>
class ITK_EXPORT ConjugateGradientOptimizer : 
        public SingleValuedNonLinearOptimizer< TCostFunction >

{
public:
  /**
   * Standard "Self" typedef.
   */
  typedef ConjugateGradientOptimizer  Self;

  /**
   * Standard "Superclass" typedef.
   */
  typedef   SingleValuedNonLinearOptimizer Superclass;

  /** 
   * Smart pointer typedef support 
   */
  typedef SmartPointer<Self>   Pointer;
  typedef SmartPointer<const Self>  ConstPointer;


  /**
   * Internal Optimizer Type
   */
  typedef   vnl_conjugate_gradient InternalOptimizerType;


 /** 
   * Run-time type information (and related methods).
   */
  itkTypeMacro( ConjugateGradientOptimizer, 
      SingleValuedNonLinearOptimizer );


  /**
   * Method for creation through the object factory.
   */
  itkNewMacro(Self);
  

  /**
   * Method for getting access to the internal optimizer
   */
  vnl_conjugate_gradient & GetOptimizer(void);
 
  /**
   * Start optimization with an initial value
   * (it cannot be const VectorType because vnl_ layer
   * uses a non-const reference
   */
  void StartOptimization( VectorType & );
 
protected:

  ConjugateGradientOptimizer();
  virtual ~ConjugateGradientOptimizer() {};
  ConjugateGradientOptimizer(const Self&) {}
  void operator=(const Self&) {}

private:

  /**
   *  The vnl optimization method for Conjugate Gradient
   */
  InternalOptimizerType     m_ConjugateGradient;


};

} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkConjugateGradientOptimizer.txx"
#endif

#endif



