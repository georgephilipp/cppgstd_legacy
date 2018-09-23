#include "stdafx.h"

#include "DNN.h"
#include "gstd/src/Map.h"
#include <set>
#include "gstd/src/Primitives.h"


namespace msii810161816
{
	namespace dnn
	{
		void DNN::buildLookup()
		{
			weightLookup.clear();
			edgeLookup.clear();
			vertexLookup.clear();

			for (int i = 0; i < (int)edges.size(); i++)
			{
				std::string id = weights[i].get(false)->id;
				gstd::check(edgeLookup.count(id) == 0, "duplicate weight id");
				weightLookup[id] = i;
			}
			for (int i = 0; i < (int)edges.size(); i++)
			{
				std::string id = edges[i].get(false)->id;
				gstd::check(edgeLookup.count(id) == 0, "duplicate edge id");
				edgeLookup[id] = i;
			}
			for (int i = 0; i < (int)vertices.size(); i++)
			{
				std::string id = vertices[i].get(false)->id;
				gstd::check(vertexLookup.count(id) == 0, "duplicate vertex id");
				vertexLookup[id] = i;
			}
		}

		void DNN::checkGraphIntegrity()
		{
			int numWeights = weights.size();
			int numEdges = edges.size();
			int numVertices = vertices.size();

			//check that all objects in the object store are assigned
			for (int i = 0; i < numWeights; i++)
			{
				gstd::check(!weights[i].isNull(), "found null weight");
			}
			for (int i = 0; i < numEdges; i++)
			{
				gstd::check(!edges[i].isNull(), "found null edge");
			}
			for (int i = 0; i < numVertices; i++)
			{
				gstd::check(!vertices[i].isNull(), "found null vertex");
			}

			buildLookup();

			//check blob ids are unique
			std::set<std::string> blobIds;
			for (int i = 0; i < numWeights; i++)
			{
				std::string blobId = weights[i].get(false)->id;
				gstd::check(blobIds.count(blobId) == 0, "found duplicate blob id");
				blobIds.insert(blobId);
			}
			for (int i = 0; i < numEdges; i++)
			{
				std::string blobId = edges[i].get(false)->weightsHeavy.get(false)->id;
				gstd::check(blobIds.count(blobId) == 0, "found duplicate blob id");
				blobIds.insert(blobId);
			}
			for (int i = 0; i < numVertices; i++)
			{
				std::string blobId = vertices[i].get(false)->content.get(false)->id;
				gstd::check(blobIds.count(blobId) == 0, "found duplicate blob id");
				blobIds.insert(blobId);
			}
			
			//check all neighbours are proper
			for (int i = 0; i<numEdges; i++)
			{
				Edge* edge = edges[i].get(false);
				checkVertexExists(edge->parent);
				checkVertexExists(edge->child);
				checkWeightExists(edge->weightsHeavy.get(false, false));
			}
			for (int i = 0; i<numVertices; i++)
			{
				Vertex* vertex = vertices[i].get(false);
				for (int j = 0; j < (int)vertex->incomingEdges.size(); j++)
					checkEdgeExists(vertex->incomingEdges[j]);
				for (int j = 0; j < (int)vertex->outgoingEdges.size(); j++)
					checkEdgeExists(vertex->outgoingEdges[j]);
				checkWeightExists(vertex->content.get(false, false));
			}

			//check neighbour reflexivity
			int totalIncomingEdges = 0;
			int totalOutgoingEdges = 0;
			for (int i = 0; i<numVertices; i++)
			{
				Vertex* vertex = vertices[i].get(false);
				std::set<std::string> incomingIds, outgoingIds;
				for (int j = 0; j < (int)vertex->incomingEdges.size(); j++)
				{
					Edge* incomingEdge = vertex->incomingEdges[j];
					gstd::check(incomingIds.count(incomingEdge->id) == 0, "same incoming edge is registered twice");
					incomingIds.insert(incomingEdge->id);
					gstd::check(incomingEdge->parent == vertex, "incoming edges parent is not the vertex");
					totalIncomingEdges++;
				}
				for (int j = 0; j < (int)vertex->outgoingEdges.size(); j++)
				{
					Edge* outgoingEdge = vertex->outgoingEdges[j];
					gstd::check(outgoingIds.count(outgoingEdge->id) == 0, "same incoming edge is registered twice");
					outgoingIds.insert(outgoingEdge->id);
					gstd::check(outgoingEdge->parent == vertex, "incoming edges parent is not the vertex");
					totalOutgoingEdges++;
				}
				gstd::check(totalIncomingEdges == numEdges, "some edge is not registered as an incoming edge");
				gstd::check(totalOutgoingEdges == numEdges, "some edge is not registered as an outgoing edge");
			}
		}

		void DNN::copy(DNN* target, std::string copyMode)
		{
			int numWeights = weights.size();
			int numVertices = vertices.size();
			int numEdges = edges.size();

			gstd::check((int)target->weights.size() == 0, "cannot copy into non-empty DNN object");
			gstd::check((int)target->vertices.size() == 0, "cannot copy into non-empty DNN object");
			gstd::check((int)target->edges.size() == 0, "cannot copy into non-empty DNN object");

			//copy objects
			target->weights.resize(numWeights);
			target->vertices.resize(numVertices);
			target->edges.resize(numEdges);

			for (int i = 0; i < numWeights; i++)
			{
				target->weights[i].set(weights[i].get(false)->construct(), true);
				weights[i].get(false)->copy(target->weights[i].get(false), copyMode);
			}

			for (int i = 0; i < numEdges; i++)
			{
				target->edges[i].set(edges[i].get(false)->construct(), true);
				target->edges[i].get(false)->weightsHeavy.set(edges[i].get(false)->weightsHeavy.get(false)->construct(), true);
				edges[i].get(false)->copy(target->edges[i].get(false), copyMode);
			}

			for (int i = 0; i < numVertices; i++)
			{
				target->vertices[i].set(vertices[i].get(false)->construct(), true);
				target->vertices[i].get(false)->content.set(vertices[i].get(false)->content.get(false)->construct(), true);
				vertices[i].get(false)->copy(target->vertices[i].get(false), copyMode);
			}

			//copy the graph
			buildLookup();
			for (int i = 0; i < numEdges; i++)
			{
				Edge* edge = target->edges[i].get(false);
				edge->child = target->vertices[vertexLookup.at(edge->child->id)].get(false);
				edge->parent = target->vertices[vertexLookup.at(edge->parent->id)].get(false);
				edge->weightsLight = target->weights[weightLookup.at(edge->weightsLight->id)].get(false);
			}
			for (int i = 0; i < numVertices; i++)
			{
				Vertex* vertex = target->vertices[i].get(false);
				int numIncomingEdges = vertex->incomingEdges.size();
				int numOutgoingEdges = vertex->outgoingEdges.size();
				for (int j = 0; j < numIncomingEdges; j++)
				{
					std::string incomingEdgeId = vertex->incomingEdges[j]->id;
					vertex->incomingEdges[j] = target->edges[edgeLookup.at(incomingEdgeId)].get(false);
				}
				for (int j = 0; j < numOutgoingEdges; j++)
				{
					std::string outgoingEdgeId = vertex->outgoingEdges[j]->id;
					vertex->outgoingEdges[j] = target->edges[edgeLookup.at(outgoingEdgeId)].get(false);
				}
			}

			//copy run data structures
			target->inVertices = inVertices;
			target->outVertices = outVertices;
			target->activeEdgeMap = activeEdgeMap;
			target->incomingDependencies = incomingDependencies;
			target->outgoingDependencies = outgoingDependencies;
			target->incomingDependenciesRemaining = incomingDependenciesRemaining;
			target->outgoingDependenciesRemaining = outgoingDependenciesRemaining;
		}

		void DNN::makeLight()
		{
			int numEdges = edges.size();
			for (int i = 0; i < numEdges; i++)
			{
				Edge* edge = edges[i].get(false);
				Blob* heavyWeights = edge->weightsHeavy.get(false);
				if (!heavyWeights->isEmpty())
				{
					edge->weightsLight->addOrCopy(heavyWeights);
					heavyWeights->clear();
				}
			}
		}

		bool DNN::graphEquals(DNN* val)
		{
			int numWeights = weights.size();
			int numEdges = edges.size();
			int numVertices = vertices.size();

			if (numWeights != (int)val->weights.size() || numEdges != (int)val->edges.size() || numVertices != (int)val->vertices.size())
				return false;

			buildLookup();
			val->buildLookup();

			for (int i = 0; i < numVertices; i++)
			{
				Vertex* vertex = vertices[i].get(false);
				std::string id = vertex->id;
				if (val->vertexLookup.count(id) == 0)
					return false;
				Vertex* match = val->vertices[val->vertexLookup.at(id)].get(false);
				int numIncomingEdges = vertex->incomingEdges.size();
				int numOutgoingEdges = vertex->outgoingEdges.size();
				if (numIncomingEdges != (int)match->incomingEdges.size())
					return false;
				if (numOutgoingEdges != (int)match->outgoingEdges.size())
					return false;
				for (int i = 0; i < numIncomingEdges; i++)
				{
					if (vertex->incomingEdges[i]->id != match->incomingEdges[i]->id)
						return false;
					if (vertex->outgoingEdges[i]->id != match->outgoingEdges[i]->id)
						return false;
				}
				if (vertex->content.get(false)->id != match->content.get(false)->id)
					return false;
			}

			for (int i = 0; i < numEdges; i++)
			{
				Edge* edge = edges[i].get(false);
				Edge* match = val->edges[val->edgeLookup.at(edge->id)].get(false);
				if (edge->weightsHeavy.get(false)->id != match->weightsHeavy.get(false)->id)
					return false;
				if (edge->weightsLight->id != match->weightsLight->id)
					return false;
			}

			return true;
		}

		void DNN::add(DNN* val)
		{
			buildLookup();
			val->buildLookup();

			int numWeights = val->weights.size();
			int numEdges = val->edges.size();

			for (int i = 0; i < numWeights; i++)
			{
				std::string id = val->weights[i].get(false)->id;
				gstd::check(weightLookup.count(id) == 1, "cannot find weight blob to add to");
				weights[weightLookup.at(id)].get(false)->addOrCopy(val->weights[i].get(false));
			}

			for (int i = 0; i < numEdges; i++)
			{
				std::string id = val->edges[i].get(false)->id;
				gstd::check(edgeLookup.count(id) == 1, "cannot find edge to add to");
				edges[edgeLookup.at(id)].get(false)->weightsHeavy.get(false)->addOrCopy(val->edges[i].get(false)->weightsHeavy.get(false));
			}
		}

		void DNN::scalarMultiply(double val)
		{
			int numWeights = weights.size();
			int numEdges = edges.size();

			for (int i = 0; i<numWeights; i++)
				weights[i].get(false)->scalarMultiply(val);

			for (int i = 0; i<numEdges; i++)
				edges[i].get(false)->weightsHeavy.get(false)->scalarMultiply(i);
		}

		void DNN::populate(std::vector<std::string> inVertices, std::vector<int> indeces)
		{
			gstd::check((int)indeces.size() > 0, "cannot populate with zero data points");
			int firstIndex = indeces[0];
			int numIndeces = indeces.size();

			for (int i = 0; i < (int)inVertices.size(); i++) /*loop is parallelizable*/
			{
				Vertex* vertex = vertices[vertexLookup.at(inVertices[i])].get(false);
				Blob* blob = vertex->content.get(false);
				gstd::trial<data::DpTensor<float> > firstDataPoint = vertex->dataClient.get(false)->get(firstIndex);
				gstd::check(firstDataPoint.success, "getting data point failed");
				blob->resetN(numIndeces, firstDataPoint.result);

				for (int j = 0; j < numIndeces; j++) /*loop is parallelizable*/
				{
					gstd::trial<data::DpTensor<float> > dataPoint = vertex->dataClient.get(false)->get(indeces[i]);
					gstd::check(dataPoint.success, "getting data point failed");
					blob->setn(j, dataPoint.result);
				}
			}
		}

		void DNN::buildExecutionDataStructures(std::vector<std::string> _inVertices, std::vector<std::string> _outVertices)
		{
			buildLookup();

			inVertices = _inVertices;
			outVertices = _outVertices;

			std::vector<std::string> vertexList = gstd::map::keys(vertexLookup);
			std::vector<std::string> edgeList = gstd::map::keys(edgeLookup);
			int numVertices = (int)vertexList.size();
			int numEdges = (int)edgeList.size();

			std::map<std::string, bool> activeVertexMapForward;
			std::map<std::string, bool> activeEdgeMapForward;
			buildActiveSubgraph(inVertices, true, activeVertexMapForward, activeEdgeMapForward);
			std::map<std::string, bool> activeVertexMapBackward;
			std::map<std::string, bool> activeEdgeMapBackward;
			buildActiveSubgraph(inVertices, true, activeVertexMapBackward, activeEdgeMapBackward);

			activeEdgeMap = activeEdgeMapForward;
			for (int i = 0; i < numEdges; i++)
			{
				if (!activeEdgeMapBackward[edgeList[i]])
					activeEdgeMap[edgeList[i]] = false;
			}

			incomingDependencies = gstd::map::create(vertexList, std::vector<int>(numVertices, 0));
			for (int i = 0; i < numEdges; i++)
			{
				std::string currentId = edges[i].get(false)->id;
				if (activeEdgeMap[currentId])
					incomingDependencies[edges[i].get(false)->child->id]++;
				if (activeEdgeMap[currentId])
					outgoingDependencies[edges[i].get(false)->parent->id]++;
			}
		}

		void DNN::run()
		{
			incomingDependenciesRemaining = incomingDependencies;

			/* for parallelism, launch each loop in a different thread */
			for (int i = 0; i < (int)inVertices.size(); i++)
			{
				runVertex(inVertices[i], false);
			}
		}

		void DNN::gradient(DNN* valueStore)
		{
			outgoingDependenciesRemaining = outgoingDependencies;

			/* for parallelism, launch each loop in a different thread */
			for (int i = 0; i < (int)outVertices.size(); i++)
			{
				std::string outVertexId = outVertices[i];
				if (outgoingDependencies.count(outVertexId) == 0 && incomingDependencies.count(outVertexId) == 1)
					gradientVertex(outVertexId, valueStore);
			}
		}

		void DNN::clearVertices()
		{
			for (int i = 0; i < (int)vertices.size(); i++)
			{
				if (!vertices[i].get(false)->content.isNull())
					vertices[i].get(false)->content.get(false)->clear();
			}
		}

		//private functions
		void DNN::checkWeightExists(Blob* weight)
		{
			gstd::check(weight != 0, "found zero weight");
			gstd::check(weightLookup.count(weight->id) == 1, "found weight with unregistered id");
		}

		void DNN::checkEdgeExists(Edge* edge)
		{
			gstd::check(edge != 0, "found zero edge");
			gstd::check(edgeLookup.count(edge->id) == 1, "found edge with unregistered id");
		}

		void DNN::checkVertexExists(Vertex* vertex)
		{
			gstd::check(vertex != 0, "found nonexistent vertex");
			gstd::check(vertexLookup.count(vertex->id) == 1, "found vertex with unregistered id");
		}

		void DNN::buildActiveSubgraph(std::vector<std::string> seedVertices, bool isForward, std::map<std::string, bool>& activeVertexMap, std::map<std::string, bool>& _activeEdgeMap)
		{
			activeVertexMap = gstd::map::create(gstd::map::keys(vertexLookup), std::vector<bool>(vertexLookup.size(), false));
			_activeEdgeMap = gstd::map::create(gstd::map::keys(edgeLookup), std::vector<bool>(edgeLookup.size(), false));
			int cursor = 0;
			while (cursor < (int)seedVertices.size())
			{
				std::string currentVertex = seedVertices[cursor];
				std::vector<Edge*> currentEdges;
				if (isForward)
					currentEdges = vertices[vertexLookup.at(currentVertex)].get(false)->outgoingEdges;
				else
					currentEdges = vertices[vertexLookup.at(currentVertex)].get(false)->incomingEdges;
				for (int i = 0; i < (int)currentEdges.size(); i++)
				{
					Vertex* relative;
					if (isForward)
						relative = currentEdges[i]->child;
					else
						relative = currentEdges[i]->parent;
					if (!relative->content.isNull() && relative->content.get(false)->isEmpty())
					{
						_activeEdgeMap[currentEdges[i]->id] = true;
						if (!activeVertexMap.at(relative->id))
						{
							activeVertexMap[relative->id] = true;
							seedVertices.push_back(relative->id);
						}
					}
				}
				cursor++;
			}
		}

		void DNN::runVertex(std::string id, bool actuallyRun)
		{
			Vertex* vertex = vertices[vertexLookup.at(id)].get(false);

			if (actuallyRun)
				vertex->run();

			/*for parallelism, insert mutex here*/
			int numOutEdges = vertex->outgoingEdges.size();
			for (int i = 0; i < numOutEdges; i++)
			{
				std::string thisEdgeId = vertex->outgoingEdges[i]->id;
				if (activeEdgeMap[thisEdgeId])
				{
					std::string thisVertexId = vertex->outgoingEdges[i]->child->id;
					incomingDependenciesRemaining[thisVertexId]--;
					if (incomingDependenciesRemaining[thisVertexId] == 0)
						runVertex(thisVertexId, true); /*for parallelism, launch this in a new thread*/
				}
			}
		}

		void DNN::gradientVertex(std::string id, DNN* valueStore)
		{
			Vertex* vertex = vertices[vertexLookup.at(id)].get(false);
			Vertex* vsVertex = valueStore->vertices[valueStore->vertexLookup.at(id)].get(false);

			vertex->runGradient(vsVertex);

			/*for parallelism, insert mutex here*/
			int numInEdges = vertex->incomingEdges.size();
			for (int i = 0; i < numInEdges; i++)
			{
				std::string thisEdgeId = vertex->incomingEdges[i]->id;
				if (activeEdgeMap[thisEdgeId])
				{
					std::string thisVertexId = vertex->incomingEdges[i]->child->id;
					outgoingDependenciesRemaining[thisVertexId]--;
					if (outgoingDependenciesRemaining[thisVertexId] == 0 && incomingDependencies.count(thisVertexId) == 1)
					{
						gradientVertex(thisVertexId, valueStore); /*for parallelism, launch this in a new thread*/
					}						
				}
			}
		}

		//Base package
		gstd::TypeName DNN::getTypeName()
		{
			return gstd::TypeNameGetter<DNN>::get();
		}
		void DNN::setInputs()
		{

		}
		bool DNN::test()
		{
			return true;
		}
		std::string DNN::toString()
		{
			std::stringstream res;
			res << "This is dnn::DNN " << std::endl;
			return res.str();
		}


	}
}