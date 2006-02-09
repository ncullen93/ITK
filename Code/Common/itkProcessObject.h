/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkProcessObject.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkProcessObject_h
#define __itkProcessObject_h

#include "itkObject.h"
#include "itkDataObject.h"
#include "itkMultiThreader.h"
#include "itkObjectFactory.h"
#include <vector>

namespace itk
{

/** \class ProcessObject
 * \brief ProcessObject is the base class for all process objects (source,
 *        filters, mappers) in the Insight data processing pipeline.
 *
 * ProcessObject is an abstract object that specifies behavior and
 * interface of network process objects (sources, filters,
 * mappers). Source objects are creators of visualization data;
 * filters input, process, and output image data; and mappers
 * transform data into another form (like transforming coordinates or
 * writing data to a file).
 *
 * A major role of ProcessObject is to define the inputs and outputs
 * of a filter. More than one input and/or output may exist for a given
 * filter. Some classes (e.g., source objects or mapper objects) will
 * not use inputs (the source) or outputs (mappers). In this case, the
 * inputs or outputs is just ignored.
 *
 * ProcessObject invokes the following events: 
 * Command::StartEvent, Command::EndEvent
 * These are convenience events you can use for any purpose
 * (e.g., debugging info, highlighting/notifying user interface, etc.) 
 * See Command and LightObject for information on using AddObserver.
 *
 * Another event Command::ProgressEvent can be observed. Some filters invoke
 * this event periodically during their execution (with the progress,
 * parameter, the fraction of work done). The use is similar to that of
 * StartEvent and EndEvent. Filters may also check their
 * AbortGenerateData flag to determine whether to prematurally end their
 * execution.
 *
 * An important feature of subclasses of ProcessObject is that it is
 * possible to control the memory-management model (i.e., retain
 * output versus delete output data). The ReleaseDataFlag enables the
 * deletion of the output data once the downstream process object
 * finishes processing the data (please see text). The
 * ReleaseDataBeforeUpdateFlag enables the deletion of the
 * ProcessObject's output data from a previous update if that output
 * data is slated to be regenerated by the pipeline process.  Setting
 * this flag can control peak memory usage during a subsequent
 * pipeline update.  For a ProcessObject, the ReleaseDataFlag defaults
 * to false and the ReleaseDataBeforeUpdateFlag defaults to true.
 * Some subclasses of ProcessObject, for example ImageSource, use a
 * default setting of false for the ReleaseDataBeforeUpdateFlag.
 *
 * Subclasses of ProcessObject may override 4 of the methods of this class
 * to control how a given filter may interact with the pipeline (dataflow).
 * These methods are: GenerateOutputInformation(),
 * EnlargeOutputRequestedRegion(), GenerateInputRequestedRegion(), and
 * GenerateOutputRequestedRegion(). By overriding these methods, a filter
 * can deviate from the base assumptions of the pipeline execution model.
 *
 * \ingroup ITKSystemObjects
 * \ingroup DataProcessing 
 *       
 */
class ITKCommon_EXPORT ProcessObject : public Object
{
public:
  /** Standard class typedefs. */
  typedef ProcessObject       Self;
  typedef Object  Superclass;
  typedef SmartPointer<Self>  Pointer;
  typedef SmartPointer<const Self>  ConstPointer;
  
  /** Run-time type information (and related methods). */
  itkTypeMacro(ProcessObject,Object);

  /** Smart Pointer type to a DataObject. */
  typedef DataObject::Pointer DataObjectPointer;

  /** STL Array of SmartPointers to DataObjects */
  typedef std::vector<DataObjectPointer> DataObjectPointerArray;

  /** Return an array with all the inputs of this process object.
   * This is useful for tracing back in the pipeline to construct
   * graphs etc.  */
  DataObjectPointerArray& GetInputs() 
    {return m_Inputs;}

  /** Get the size of the input vector.  This is merely the size of
   * the input vector, not the number of inputs that have valid
   * DataObject's assigned. Use GetNumberOfValidRequiredInputs() to
   * determine how many inputs are non-null. */
  std::vector<DataObjectPointer>::size_type GetNumberOfInputs() const
    {return m_Inputs.size();}

  /** Get the number of valid inputs.  This is the number of non-null
   * entries in the input vector in the first NumberOfRequiredInputs
   * slots. This method is used to determine whether the necessary
   * required inputs have been set. Subclasses of ProcessObject may
   * override this implementation if the required inputs are not
   * the first slots in input vector.
   */
  virtual std::vector<DataObjectPointer>::size_type GetNumberOfValidRequiredInputs() const;
  
  /** Return an array with all the outputs of this process object.
   * This is useful for tracing forward in the pipeline to contruct
   * graphs etc.  */
  DataObjectPointerArray& GetOutputs()
    { return m_Outputs; }
  std::vector<DataObjectPointer>::size_type GetNumberOfOutputs() const
    {return m_Outputs.size();}
      
  /** Set the AbortGenerateData flag for the process object. Process objects
   *  may handle premature termination of execution in different ways.  */
  itkSetMacro(AbortGenerateData,bool);

  /** Get the AbortGenerateData flag for the process object. Process objects
   *  may handle premature termination of execution in different ways.  */
  itkGetConstReferenceMacro(AbortGenerateData,bool);
  
  /** Turn on and off the AbortGenerateData flag. */
  itkBooleanMacro(AbortGenerateData); 
  
  /** Set the execution progress of a process object. The progress is
   * a floating number in [0,1] with 0 meaning no progress and 1 meaning
   * the filter has completed execution.  The ProgressEvent is NOT
   * invoked. */
  itkSetClampMacro(Progress,float,0.0,1.0);

  /** Get the execution progress of a process object. The progress is
   * a floating number in [0,1] with 0 meaning no progress and 1 meaning
   * the filter has completed execution. */
  itkGetConstReferenceMacro(Progress,float);

  /** Update the progress of the process object.
   *
   * Sets the Progress ivar to amount and invokes any observers for
   * the ProgressEvent. The parameter amount should be in [0,1] and is
   * the cumulative (not incremental) progress. */
  void UpdateProgress(float amount);
  
  /** Bring this filter up-to-date. Update() checks modified times against
   * last execution times, and re-executes objects if necessary. A side
   * effect of this method is that the whole pipeline may execute
   * in order to bring this filter up-to-date. This method updates the
   * currently prescribed requested region.  If no requested region has
   * been set on the output, then the requested region will be set to the
   * largest possible region. Once the requested region is set, Update()
   * will make sure the specified requested region is up-to-date. This
   * is a confusing side effect to users who are just calling Update() on
   * a filter.  A first call to Update() will cause the largest possible
   * region to be updated.  A second call to Update() will update that
   * same region.  If a modification to the upstream pipeline cause a
   * filter to have a different largest possible region, this second
   * call to Update() will not cause the output requested region to be
   * reset to the new largest possible region.  Instead, the output requested
   * region will be the same as the last time Update() was called. To have
   * a filter always to produce its largest possible region, users should
   * call UpdateLargestPossibleRegion() instead. */
  virtual void Update();

  /** Like Update(), but sets the output requested region to the
   * largest possible region for the output.  This is the method users
   * should call if they want the entire dataset to be processed.  If
   * a user wants to update the same output region as a previous call
   * to Update() or a previous call to UpdateLargestPossibleRegion(), 
   * then they should call the method Update(). */
  virtual void UpdateLargestPossibleRegion();

  /** Update the information decribing the output data. This method
   * transverses up the pipeline gathering modified time information.
   * On the way back down the pipeline, this method calls
   * GenerateOutputInformation() to set any necessary information
   * about the output data objects.  For instance, a filter that
   * shrinks an image will need to provide an implementation for
   * GenerateOutputInformation() that changes the spacing of the
   * pixels. Such filters should call their superclass' implementation
   * of GenerateOutputInformation prior to changing the information
   * values they need (i.e. GenerateOutputInformation() should call
   * Superclass::GenerateOutputInformation() prior to changing the
   * information. */
  virtual void UpdateOutputInformation();

  /** Send the requested region information back up the pipeline (to the
   * filters that preceed this one). */
  virtual void PropagateRequestedRegion(DataObject *output);

  /** Actually generate new output  */
  virtual void UpdateOutputData(DataObject *output);


  /** Give the process object a chance to indictate that it will produce more
   * output than it was requested to produce. For example, many imaging
   * filters must compute the entire output at once or can only produce output
   * in complete slices. Such filters cannot handle smaller requested regions.
   * These filters must provide an implementation of this method, setting
   * the output requested region to the size they will produce.  By default,
   * a process object does not modify the size of the output requested region. */
  virtual void EnlargeOutputRequestedRegion(DataObject *itkNotUsed(output)){};
  

  /** Reset the pipeline. If an exception is thrown during an Update(),
   * the pipeline may be in an inconsistent state.  This method clears
   * the internal state of the pipeline so Update() can be called. */
  virtual void ResetPipeline();

  /** Make a DataObject of the correct type to used as the specified
   * output.  Every ProcessObject subclass must be able to create a
   * DataObject that can be used as a specified output. This method
   * is automatically called when DataObject::DisconnectPipeline() is
   * called.  DataObject::DisconnectPipeline, disconnects a data object
   * from being an output of its current source.  When the data object
   * is disconnected, the ProcessObject needs to construct a replacement
   * output data object so that the ProcessObject is in a valid state.
   * So DataObject::DisconnectPipeline eventually calls
   * ProcessObject::MakeOutput. Note that MakeOutput always returns a
   * itkSmartPointer to a DataObject. ImageSource and MeshSource override
   * this method to create the correct type of image and mesh respectively.
   * If a filter has multiple outputs of different types, then that
   * filter must provide an implementation of MakeOutput(). */
  virtual DataObjectPointer MakeOutput(unsigned int idx);
  
  /** Turn on/off the flags to control whether the bulk data belonging
   * to the outputs of this ProcessObject are released after being
   * used by a downstream ProcessObject. Default value is off. Another
   * options for controlling memory utilization is the
   * ReleaseDataBeforeUpdateFlag. */
  virtual void SetReleaseDataFlag(bool flag);
  virtual bool GetReleaseDataFlag() const;
  void ReleaseDataFlagOn() {this->SetReleaseDataFlag(true);}
  void ReleaseDataFlagOff() {this->SetReleaseDataFlag(false);}

  /** Turn on/off the flags to control whether the bulk data belonging
   * to the outputs of this ProcessObject are released/reallocated
   * during an Update().  In limited memory scenarios, a user may want
   * to force the elements of a pipeline to release any bulk data that
   * is going to be regenerated anyway during an Update() in order to
   * control peak memory allocation. Note that this flag is different
   * from the ReleaseDataFlag. ReleaseDataFlag manages the
   * deallocation of a ProcessObject's bulk output data once that data
   * has been consumed by a downstream ProcessObject.  The
   * ReleaseDataBeforeUpdateFlag manages the deallocation/reallocation
   * of bulk data during a pipeline update to control peak memory
   * utilization. Default value is on. */
  itkSetMacro(ReleaseDataBeforeUpdateFlag, bool);
  itkGetConstReferenceMacro(ReleaseDataBeforeUpdateFlag, bool);
  itkBooleanMacro(ReleaseDataBeforeUpdateFlag);
   
  
  /** Get/Set the number of threads to create when executing. */
  itkSetClampMacro( NumberOfThreads, int, 1, ITK_MAX_THREADS );
  itkGetConstReferenceMacro( NumberOfThreads, int );
  
  /** Return the multithreader used by this class. */
  MultiThreader * GetMultiThreader()
    {return m_Threader;}

  /** An opportunity to deallocate a ProcessObject's bulk data
   *  storage. Some filters may wish to reuse existing bulk data
   *  storage to avoid unnecessary deallocation/allocation
   *  sequences. The default implementation calls Initialize() on each
   *  output. DataObject::Initialize() frees its bulk data by default.
   */
  virtual void PrepareOutputs();

protected:
  ProcessObject();
  ~ProcessObject();
  void PrintSelf(std::ostream& os, Indent indent) const;
  
  /** Protected methods for setting inputs.
   * Subclasses make use of them for setting input. */
  virtual void SetNthInput(unsigned int num, DataObject *input);
  virtual void AddInput(DataObject *input);
  virtual void RemoveInput(DataObject *input);
  itkSetMacro(NumberOfRequiredInputs,unsigned int);
  itkGetConstReferenceMacro(NumberOfRequiredInputs,unsigned int);

  /** Push/Pop an input of this process object. These methods allow a
   * filter to model its input vector as a queue or stack.  These
   * routines may not be appropriate for all filters, especially
   * filters with different types of inputs.  These routines follow
   * the semantics of STL. */
  virtual void PushBackInput(const DataObject *input);
  virtual void PopBackInput();
  virtual void PushFrontInput(const DataObject *input);
  virtual void PopFrontInput();
  
  
  /** Called to allocate the input array. Copies old inputs. */
  void SetNumberOfInputs(unsigned int num);

  /** Method used internally for getting an input. */
  DataObject * GetInput(unsigned int idx);
  const DataObject * GetInput(unsigned int idx) const;

  /** Protected methods for setting outputs.
   * Subclasses make use of them for getting output. */
  virtual void SetNthOutput(unsigned int num, DataObject *output);
  virtual void AddOutput(DataObject *output);
  virtual void RemoveOutput(DataObject *output);
  itkSetMacro(NumberOfRequiredOutputs,unsigned int);
  itkGetConstReferenceMacro(NumberOfRequiredOutputs,unsigned int);

  /** Called to allocate the output array.  Copies old outputs. */
  void SetNumberOfOutputs(unsigned int num);

  /** Method used internally for getting an output. */
  DataObject * GetOutput(unsigned int idx);
  const DataObject * GetOutput(unsigned int idx) const;

  /** What is the input requested region that is required to produce the
   * output requested region? By default, the largest possible region is
   * always required but this is overridden in many subclasses. For instance,
   * for an image processing filter where an output pixel is a simple function
   * of an input pixel, the input requested region will be set to the output
   * requested region.  For an image processing filter where an output pixel
   * is a function of the pixels in a neighborhood of an input pixel, then
   * the input requested region will need to be larger than the output
   * requested region (to avoid introducing artificial boundary conditions).
   * This function should never request an input region that is outside the
   * the input largest possible region (i.e. implementations of this method
   * should crop the input requested region at the boundaries of the input
   * largest possible region). */
  virtual void GenerateInputRequestedRegion();
  
  /** Given one output whose requested region has been set, how should
   * the requested regions for the remaining outputs of the process object
   * be set?  By default, all the outputs are set to the same requested
   * region.  If a filter needs to produce different requested regions
   * for each output, for instance an image processing filter producing
   * several outputs at different resolutions, then that filter may
   * override this method and set the requested regions appropriatedly.
   *
   * Note that a filter producing multiple outputs of different types is
   * required to override this method.  The default implementation
   * can only correctly handle multiple outputs of the same type. */
  virtual void GenerateOutputRequestedRegion(DataObject *output);

  /** Generate the information decribing the output data. The default 
   * implementation of this method will copy information from the input to
   * the output.  A filter may override this method if its output will have
   * different information than its input.  For instance, a filter that 
   * shrinks an image will need to provide an implementation for this 
   * method that changes the spacing of the pixels. Such filters should call
   * their superclass' implementation of this method prior to changing the
   * information values they need (i.e. GenerateOutputInformation() should
   * call Superclass::GenerateOutputInformation() prior to changing the
   * information. */
  virtual void GenerateOutputInformation();
  
  /** This method causes the filter to generate its output. */
  virtual void GenerateData() {}

  /** Called to allocate the input array.  Copies old inputs. */
  /** Propagate a call to ResetPipeline() up the pipeline. Called only from
   * DataObject. */
  virtual void PropagateResetPipeline();

  /** A filter may need to release its input's bulk data after it has
   * finished calculating a new output. The filter may need to release
   * the inputs because the user has turned on the ReleaseDataFlag or
   * it may need to release the inputs because the filter is an "in
   * place" filter and it has overwritten its input with its output
   * data.  The implementation here simply checks the ReleaseDataFlag
   * of the inputs.  InPlaceImageFilter overrides this method so
   * release the input it has overwritten.
   *
   * \sa InPlaceImageFilter::ReleaseInputs()
   */
  virtual void ReleaseInputs();

  /**
   * Cache the state of any ReleaseDataFlag's on the inputs. While the
   * filter is executing, we need to set the ReleaseDataFlag's on the
   * inputs to false in case the current filter is implemented using a
   * mini-pipeline (which will try to release the inputs).  After the
   * filter finishes, we restore the state of the ReleaseDataFlag's
   * before the call to ReleaseInputs().
   */
  virtual void CacheInputReleaseDataFlags();

  /**
   * Restore the cached input ReleaseDataFlags.
   */
  virtual void RestoreInputReleaseDataFlags();
  
  /** These ivars are made protected so filters like itkStreamingImageFilter
   * can access them directly. */
  
  /** This flag indicates when the pipeline is executing.
   * It prevents infinite recursion when pipelines have loops. */
  bool m_Updating;

  /** Time when GenerateOutputInformation was last called. */
  TimeStamp m_OutputInformationMTime;

private:
  ProcessObject(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  /** An array of the inputs to the filter. */
  std::vector<DataObjectPointer> m_Inputs;
  unsigned int m_NumberOfRequiredInputs;

  /** An array that caches the ReleaseDataFlags of the inputs */
  std::vector<bool> m_CachedInputReleaseDataFlags;
  
  /** An array of the outputs to the filter. */
  std::vector<DataObjectPointer> m_Outputs;
  unsigned int m_NumberOfRequiredOutputs;
  
  /** These support the progress method and aborting filter execution. */
  bool  m_AbortGenerateData;
  float m_Progress;
  
  /** Support processing data in multiple threads. Used by subclasses
   * (e.g., ImageSource). */
  MultiThreader::Pointer m_Threader;
  int m_NumberOfThreads;

  /** Memory management ivars */
  bool m_ReleaseDataBeforeUpdateFlag;
  
  /** Friends of ProcessObject */
  friend class DataObject;
};

} // end namespace itk

#endif

