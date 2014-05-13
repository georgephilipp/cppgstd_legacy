/* 
 * File:   SelectionModel.cpp
 * Author: gschoenh
 * 
 * Created on December 5, 2013, 11:57 AM
 */

#include "stdafx.h"

#include "SelectionModel.h"
#include "gstd/src/Map.h"
#include "gstd/src/Vector.h"
#include "gstd/src/Primitives.h"

namespace msii810161816
{   
    namespace ml
    {
        SelectionModel::SelectionModel() 
        {
            selectionCacheProfile.selection = false;
			numFolds = -1;
                        crossValidationDocSweep = false;
        }

        SelectionModel::~SelectionModel() {}
        
        void SelectionModel::cacheSelectionModel()
        {
            SelectionCache cache;
            cache.lambda = lambda;
            if(selectionCacheProfile.selection)
                cache.selection = selection;
            selectionCache.push_back(cache);
        }
        
        void SelectionModel::rank()
        {
            int numLambdas = selectionCache.size();
            int size = parameter.size();
            std::vector<int> scores(size,-numLambdas);
            for(int l=numLambdas-1;l>=0;l--)
                for(int i=0;i<size;i++)
                    if(selectionCache[l].selection.count(i))
                        scores[i] = -l;
            ranks = gstd::vector::rank(scores, false);       
        }

        //Base package
        std::string SelectionModel::toString()
        {
            std::stringstream res;
            res << "This is a ml::SelectionModel" << std::endl;
            res << "lambda is: " << lambda << std::endl;
            res << "Snippet from selection is: " << gstd::Printer::vp(gstd::vector::sub(gstd::map::keys(selection),0,10,true)) << std::endl;
            res << "Snippet from targetSelection is: " << gstd::Printer::vp(gstd::vector::sub(gstd::map::keys(targetSelection),0,10,true)) << std::endl;
            res << "Size of selectionCache is: " << selectionCache.size() << std::endl;
            if(lambdaSweepSchedule.isNull())
                res << "Sweep schedule not set" << std::endl;
            else
                res << lambdaSweepSchedule.get(false)->toString();
            return res.str();
        }    

		//Standard Fns
		void SelectionModel::sweepStd()
		{
			//backup some parameters            
			double cachedLambda = lambda;
			bool cachedDoc = doc;

			//initialize
			lambdaSweepSchedule.get(false)->reset();
			double nextLambda = lambdaSweepSchedule.get(false)->next();
			doc = false;
			int P = 0;
			int cursor = 0;
			std::vector<double> rankscores;

			while (nextLambda >= 0)
			{
				cursor++;
				if (cachedDoc)
					gstd::Printer::c("starting next parameter value. Iteration is " + std::to_string(cursor) + " Lambda is " + std::to_string(nextLambda));

				lambda = nextLambda;
				fit();

				if (cachedDoc)
					gstd::Printer::c("Selection is: " + gstd::Printer::vp(gstd::map::keys(selection)));

				if (rankscores.size() == 0)
				{
					P = parameter.size();
					rankscores = std::vector<double>(P, -1);
				}

				for (int p = 0; p<P; p++)
				if (selection.count(p) == 1 && rankscores[p] < 0)
					rankscores[p] = nextLambda;

				nextLambda = lambdaSweepSchedule.get(false)->next();
			}

			//set outputs
			ranks = gstd::vector::rank(rankscores, false);

			//restore model
			lambda = cachedLambda;
			doc = cachedDoc;

			//print results
			if (cachedDoc)
				sweepDocStd();
		}
		void SelectionModel::fitDocStd()
		{
			gstd::Printer::c("Target model was:");
			gstd::Printer::vc(target);
			gstd::Printer::c("Learnt model is:");
			gstd::Printer::vc(parameter);
			gstd::Printer::c("Target selection is:");
			gstd::Printer::vc(gstd::map::keys(targetSelection));
			gstd::Printer::c("selection is:");
			gstd::Printer::vc(gstd::map::keys(selection));
		}
		void SelectionModel::sweepDocStd()
		{
			std::vector<int> tsVector = gstd::map::keys(targetSelection);
			gstd::Printer::c("Target selection is:");
			gstd::Printer::vc(tsVector);
			gstd::Printer::c("Target parameters of target selection are: ");
			if ((int)target.size() > 0)
			    gstd::Printer::vc(gstd::vector::getmany(target, tsVector));
			gstd::Printer::c("Ranks of targets are:");
			gstd::Printer::vc(gstd::vector::getmany(ranks, tsVector));
		}
    }
}