#include "stdafx.h"

#pragma once
#include "standard.h"
#include "Vertex.h"
#include "Blob.h"
#include "gstd/src/Pointer.h"
#include "Edge.h"


namespace msii810161816
{
	namespace dnn
	{
		class MSII810161816_DNN_API DNN : public virtual gstd::Base
		{
		public:
			std::vector<gstd::Pointer<Blob> > weights;
			std::vector<gstd::Pointer<Edge> > edges;
			std::vector<gstd::Pointer<Vertex> > vertices;

			std::map<std::string, int> weightLookup;
			std::map<std::string, int> edgeLookup;
			std::map<std::string, int> vertexLookup;

			//core functionality
			void buildLookup();
			void checkGraphIntegrity();
			void copy(DNN* target, std::string copyMode);
			void makeLight();
			bool graphEquals(DNN* val);

			//arithmatic
			void add(DNN* val);
			void scalarMultiply(double val);

			void populate(std::vector<std::string> usedVertices, std::vector<int> indeces);
			void buildExecutionDataStructures(std::vector<std::string> inVertices, std::vector<std::string> outVertices);
			void run();
			void gradient(DNN* valueStore);
			void clearVertices();

		private:
			void checkWeightExists(Blob* weight);
			void checkVertexExists(Vertex* vertex);
			void checkEdgeExists(Edge* edge);
			void addOrCopyBlob(Blob* targetWeights, Blob* sourceWeights);
			void buildActiveSubgraph(std::vector<std::string> seedVertices, bool isForward, std::map<std::string, bool>& activeVertexMap, std::map<std::string, bool>& activeEdgeMap);
			void runVertex(std::string id, bool actuallyRun);
			void gradientVertex(std::string id, DNN* valueStore);

			//executionDataStructures
			std::vector<std::string> inVertices;
			std::vector<std::string> outVertices;
			std::map<std::string, bool> activeEdgeMap;
			std::map<std::string, int> incomingDependencies;
			std::map<std::string, int> outgoingDependencies;
			std::map<std::string, int> incomingDependenciesRemaining; /* for parallelism, create a mutex on this map */
			std::map<std::string, int> outgoingDependenciesRemaining; /* for parallelism, create a mutex on this map */

			//Base Package    
		public:
			virtual gstd::TypeName getTypeName();
			virtual void setInputs();
			virtual bool test();
			virtual std::string toString();
		};

	}
}



namespace msii810161816
{
	namespace gstd
	{
		template<>
		struct TypeNameGetter<dnn::DNN>
		{
			static TypeName get()
			{
				TypeName t;
				t.name = "dnn::DNN";
				return t;
			}
		};
	}
}