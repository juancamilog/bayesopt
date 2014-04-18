/**  \file mcmcm.hpp \brief Markov Chain Monte Carlo algorithms */
/*
-------------------------------------------------------------------------
   This file is part of BayesOpt, an efficient C++ library for 
   Bayesian optimization.

   Copyright (C) 2011-2013 Ruben Martinez-Cantin <rmcantin@unizar.es>
 
   BayesOpt is free software: you can redistribute it and/or modify it 
   under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   BayesOpt is distributed in the hope that it will be useful, but 
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with BayesOpt.  If not, see <http://www.gnu.org/licenses/>.
------------------------------------------------------------------------
*/


#ifndef  _MCMC_HPP_
#define  _MCMC_HPP_

#include <boost/ptr_container/ptr_vector.hpp>
#include "randgen.hpp"
#include "optimizable.hpp"
#include "criteria_functors.hpp"
#include "posteriormodel.hpp"

namespace bayesopt {

  // We plan to add more in the future 
  typedef enum {
    SLICE_MCMC           ///< Slice sampling
  } McmcAlgorithms;


  /**
   * \brief Markov Chain Monte Carlo sampler
   *
   * It generates a set of particles that are distributed according to
   * an arbitrary pdf. IMPORTANT: As it should be a replacement for
   * the optimization (ML or MAP) estimation, it also assumes a
   * NEGATIVE LOG PDF.
   *
   * @see NLOPT_Optimization
   */
  class MCMCSampler
  {
  public:
    /** 
     * \brief Constructor (Note: default constructor is private)
     * 
     * @param rbo point to RBOptimizable type of object with the PDF
     *            to sample from. IMPORTANT: We assume that the 
     *            evaluation of rbo is the NEGATIVE LOG PDF.  
     * @param dim number of input dimensions
     * @param eng random number generation engine (boost)
     */
    MCMCSampler(RBOptimizable* rbo, size_t dim, randEngine& eng);
    virtual ~MCMCSampler();

    /** Sets the sampling algorithm (slice, MH, etc.) */
    void setAlgorithm(McmcAlgorithms newAlg);

    /** Sets the number of particles that are stored */
    void setNParticles(size_t nParticles);

    /**Usually, the initial samples of any MCMC method are biased and
     *	they are discarded. This phase is called the burnout. This
     *	method sets the number of particles to be discarded 
     */
    void setNBurnOut(size_t nParticles);

    /** Compute the set of particles according to the target PDF.
     * @param Xnext input: initial point of the Markov Chain, 
     *              output: last point of the Markov Chain
     */
    void run(vectord &Xnext);

    vectord getParticle(size_t i);

    void printParticles();

  private:
    void randomJump(vectord &x);
    void burnOut(vectord &x);
    void sliceSample(vectord &x);

    boost::scoped_ptr<RBOptimizableWrapper> obj;

    McmcAlgorithms mAlg;
    size_t mDims;
    size_t nBurnOut;
    size_t nSamples;
    bool mStepOut;

    vectord mSigma;
    vecOfvec mParticles;
    randEngine mtRandom;

  private: //Forbidden
    MCMCSampler();
    MCMCSampler(MCMCSampler& copy);
  };

  inline void MCMCSampler::setAlgorithm(McmcAlgorithms newAlg)
  { mAlg = newAlg; };

  inline void MCMCSampler::setNParticles(size_t nParticles)
  { nSamples = nParticles; };

  inline void MCMCSampler::setNBurnOut(size_t nParticles)
  { nBurnOut = nParticles; };

  inline vectord MCMCSampler::getParticle(size_t i)
  { return mParticles[i]; };

  inline void MCMCSampler::printParticles()
  {
    for(size_t i=0; i<mParticles.size(); ++i)
      { 
	FILE_LOG(logDEBUG) << i << "->" << mParticles[i] 
			   << " | Log-lik " << -obj->evaluate(mParticles[i]);
      }
  }



  /**
   * \brief Posterior model of nonparametric processes/criteria based
   * on MCMC samples.
   *
   * For computational reasons we store a copy of each conditional
   * models with the corresponding particle generated by MCMC. That is
   * to avoid costly operations like matrix inversions for every
   * kernel parameter in a GP prediction. Thus, we assume that the
   * number of particles is not very large.
   */
  class MCMCModel: public PosteriorModel
  {
  public:

    typedef boost::ptr_vector<NonParametricProcess>  GPVect;
    typedef boost::ptr_vector<Criteria>  CritVect;

    /** 
     * \brief Constructor (Note: default constructor is private)
     * 
     * @param dim number of input dimensions
     * @param params configuration parameters (see parameters.h)
     * @param eng random number generation engine (boost)
     */
    MCMCModel(size_t dim, bopt_params params, randEngine& eng);

    virtual ~MCMCModel();

    void updateHyperParameters();
    void fitSurrogateModel();
    void updateSurrogateModel();
    double evaluateCriteria(const vectord& query);

    bool criteriaRequiresComparison();
    void setFirstCriterium();
    bool setNextCriterium(const vectord& prevResult);
    std::string getBestCriteria(vectord& best);

    ProbabilityDistribution* getPrediction(const vectord& query);
   
  private:
    void setSurrogateModel(randEngine& eng);    
    void setCriteria(randEngine& eng);

  private:  // Members
    size_t nParticles;
    GPVect mGP;                ///< Pointer to surrogate model
    CritVect mCrit;                    ///< Metacriteria model

    boost::scoped_ptr<MCMCSampler> kSampler;

  private: //Forbidden
    MCMCModel();
    MCMCModel(MCMCModel& copy);
  };

  /**@}*/

  inline void MCMCModel::fitSurrogateModel()
  { 
    for(GPVect::iterator it=mGP.begin(); it != mGP.end(); ++it)
      it->fitSurrogateModel(); 
  };

  inline void MCMCModel::updateSurrogateModel()
  {     
    for(GPVect::iterator it=mGP.begin(); it != mGP.end(); ++it)
      it->updateSurrogateModel(); 
  };

  inline double MCMCModel::evaluateCriteria(const vectord& query)
  { 
    double sum = 0.0;
    for(CritVect::iterator it=mCrit.begin(); it != mCrit.end(); ++it)
      {
	sum += it->evaluate(query); 
      }
    return sum/static_cast<double>(nParticles);
  };

  inline bool MCMCModel::criteriaRequiresComparison()
  {return mCrit[0].requireComparison(); };
    
  inline void MCMCModel::setFirstCriterium()
  { 
    for(CritVect::iterator it=mCrit.begin(); it != mCrit.end(); ++it)
      {
	it->initialCriteria();
      }
  };

  // Although we change the criteria for all MCMC particles, we use
  // only the first element to compute de Hedge algorithm, because it
  // should be based on the average result, thus being common for all
  // the particles.
  inline bool MCMCModel::setNextCriterium(const vectord& prevResult)
  { 
    bool rotated;
    mCrit[0].pushResult(prevResult);
    for(CritVect::iterator it=mCrit.begin(); it != mCrit.end(); ++it)
      {
	rotated = it->rotateCriteria();
      }
    return rotated; 
  };

  inline std::string MCMCModel::getBestCriteria(vectord& best)
  { return mCrit[0].getBestCriteria(best); };

  inline ProbabilityDistribution* MCMCModel::getPrediction(const vectord& query)
  { return mGP[0].prediction(query); };



} //namespace bayesopt


#endif
