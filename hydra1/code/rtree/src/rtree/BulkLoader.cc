/******************************************************************************
 * Project:  libspatialindex - A C++ library for spatial indexing
 * Author:   Marios Hadjieleftheriou, mhadji@gmail.com
 ******************************************************************************
 * Copyright (c) 2002, Marios Hadjieleftheriou
 *
 * All rights reserved.
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
******************************************************************************/

#include <cstring>
#include <stdio.h>
#include <cmath>

#ifndef _MSC_VER
#include <unistd.h>
#endif

#include <spatialindex/SpatialIndex.h>

#include "RTree.h"
#include "Leaf.h"
#include "Index.h"
#include "BulkLoader.h"
#include "../../test/rtree/flags.h"
//#include "../../test/rtree/stats.h"


using namespace SpatialIndex;
using namespace SpatialIndex::RTree;
int paa_from_ts (ts_type *ts_in, ts_type *paa_out, int segments, int ts_values_per_segment) {
    int s, i;
    for (s=0; s<segments; s++) {
        paa_out[s] = 0;
        for (i=0; i<ts_values_per_segment; i++) {
            paa_out[s] += ts_in[(s * ts_values_per_segment)+i];
        }
        paa_out[s] /= ts_values_per_segment;
    }
    return 1;
}

//
// ExternalSorter::Record
//
ExternalSorter::Record::Record()
: m_pData(0)
{
}

ExternalSorter::Record::Record(const Region& r, id_type id, uint32_t len, byte* pData, uint32_t s)
: m_r(r), m_id(id), m_len(len), m_pData(pData), m_s(s)
{
}

ExternalSorter::Record::~Record()
{
	delete[] m_pData;
}

bool ExternalSorter::Record::operator<(const Record& r) const
{
	if (m_s != r.m_s)
		throw Tools::IllegalStateException("ExternalSorter::Record::operator<: Incompatible sorting dimensions.");

	if (m_r.m_pHigh[m_s] + m_r.m_pLow[m_s] < r.m_r.m_pHigh[m_s] + r.m_r.m_pLow[m_s])
		return true;
	else
		return false;
}

void ExternalSorter::Record::storeToFile(Tools::TemporaryFile& f)
{
	f.write(static_cast<uint64_t>(m_id));
	f.write(m_r.m_dimension);
	f.write(m_s);

	for (uint32_t i = 0; i < m_r.m_dimension; ++i)
	{
		f.write(m_r.m_pLow[i]);
		f.write(m_r.m_pHigh[i]);
	}

	f.write(m_len);
	if (m_len > 0) f.write(m_len, m_pData);
}

void ExternalSorter::Record::loadFromFile(Tools::TemporaryFile& f)
{
	m_id = static_cast<id_type>(f.readUInt64());
	uint32_t dim = f.readUInt32();
	m_s = f.readUInt32();

	if (dim != m_r.m_dimension)
	{
		delete[] m_r.m_pLow;
		delete[] m_r.m_pHigh;
		m_r.m_dimension = dim;
		m_r.m_pLow = new ts_type[dim];
		m_r.m_pHigh = new ts_type[dim];
	}

	for (uint32_t i = 0; i < m_r.m_dimension; ++i)
	{
		m_r.m_pLow[i] = f.readFloat();
		m_r.m_pHigh[i] = f.readFloat();	 
		//m_r.m_pLow[i] = f.readDouble();
		//m_r.m_pHigh[i] = f.readDouble();
	}

	m_len = f.readUInt32();
	delete[] m_pData; m_pData = 0;
	if (m_len > 0) f.readBytes(m_len, &m_pData);
}

//
// ExternalSorter
//
ExternalSorter::ExternalSorter(uint32_t u32PageSize, uint32_t u32BufferPages)
: m_bInsertionPhase(true), m_u32PageSize(u32PageSize),
  m_u32BufferPages(u32BufferPages), m_u64TotalEntries(0), m_stI(0)
{
}

ExternalSorter::~ExternalSorter()
{
	for (m_stI = 0; m_stI < m_buffer.size(); ++m_stI) delete m_buffer[m_stI];
}

void ExternalSorter::insert(Record* r)
{
	if (m_bInsertionPhase == false)
		throw Tools::IllegalStateException("ExternalSorter::insert: Input has already been sorted.");

	m_buffer.push_back(r);
	++m_u64TotalEntries;

	// this will create the initial, sorted buckets before the
	// external merge sort.
	if (m_buffer.size() >= m_u32PageSize * m_u32BufferPages)
	{
		std::sort(m_buffer.begin(), m_buffer.end(), Record::SortAscending());
		Tools::TemporaryFile* tf = new Tools::TemporaryFile();
		for (size_t j = 0; j < m_buffer.size(); ++j)
		{
			m_buffer[j]->storeToFile(*tf);
			delete m_buffer[j];
		}
		m_buffer.clear();
		tf->rewindForReading();
		m_runs.push_back(Tools::SmartPointer<Tools::TemporaryFile>(tf));
	}
}

void ExternalSorter::sort()
{
	if (m_bInsertionPhase == false)
		throw Tools::IllegalStateException("ExternalSorter::sort: Input has already been sorted.");

	if (m_runs.empty())
	{
		// The data fits in main memory. No need to store to disk.
		std::sort(m_buffer.begin(), m_buffer.end(), Record::SortAscending());
		m_bInsertionPhase = false;
		return;
	}

	if (m_buffer.size() > 0)
	{
		// Whatever remained in the buffer (if not filled) needs to be stored
		// as the final bucket.
		std::sort(m_buffer.begin(), m_buffer.end(), Record::SortAscending());
		Tools::TemporaryFile* tf = new Tools::TemporaryFile();
		for (size_t j = 0; j < m_buffer.size(); ++j)
		{
			m_buffer[j]->storeToFile(*tf);
			delete m_buffer[j];
		}
		m_buffer.clear();
		tf->rewindForReading();
		m_runs.push_back(Tools::SmartPointer<Tools::TemporaryFile>(tf));
	}

	if (m_runs.size() == 1)
	{
		m_sortedFile = m_runs.front();
	}
	else
	{
		Record* r = 0;

		while (m_runs.size() > 1)
		{
			Tools::SmartPointer<Tools::TemporaryFile> tf(new Tools::TemporaryFile());
			std::vector<Tools::SmartPointer<Tools::TemporaryFile> > buckets;
			std::vector<std::queue<Record*> > buffers;
			std::priority_queue<PQEntry, std::vector<PQEntry>, PQEntry::SortAscending> pq;

			// initialize buffers and priority queue.
			std::list<Tools::SmartPointer<Tools::TemporaryFile> >::iterator it = m_runs.begin();
			for (uint32_t i = 0; i < (std::min)(static_cast<uint32_t>(m_runs.size()), m_u32BufferPages); ++i)
			{
				buckets.push_back(*it);
				buffers.push_back(std::queue<Record*>());

				r = new Record();
				r->loadFromFile(**it);
					// a run cannot be empty initially, so this should never fail.
				pq.push(PQEntry(r, i));

				for (uint32_t j = 0; j < m_u32PageSize - 1; ++j)
				{
					// fill the buffer with the rest of the page of records.
					try
					{
						r = new Record();
						r->loadFromFile(**it);
						buffers.back().push(r);
					}
					catch (Tools::EndOfStreamException)
					{
						delete r;
						break;
					}
				}
				++it;
			}

			// exhaust buckets, buffers, and priority queue.
			while (! pq.empty())
			{
				PQEntry e = pq.top(); pq.pop();
				e.m_r->storeToFile(*tf);
				delete e.m_r;

				if (! buckets[e.m_u32Index]->eof() && buffers[e.m_u32Index].empty())
				{
					for (uint32_t j = 0; j < m_u32PageSize; ++j)
					{
						try
						{
							r = new Record();
							r->loadFromFile(*buckets[e.m_u32Index]);
							buffers[e.m_u32Index].push(r);
						}
						catch (Tools::EndOfStreamException)
						{
							delete r;
							break;
						}
					}
				}

				if (! buffers[e.m_u32Index].empty())
				{
					e.m_r = buffers[e.m_u32Index].front();
					buffers[e.m_u32Index].pop();
					pq.push(e);
				}
			}

			tf->rewindForReading();

			// check if another pass is needed.
			uint32_t u32Count = std::min(static_cast<uint32_t>(m_runs.size()), m_u32BufferPages);
			for (uint32_t i = 0; i < u32Count; ++i)
			{
				m_runs.pop_front();
			}

			if (m_runs.size() == 0)
			{
				m_sortedFile = tf;
				break;
			}
			else
			{
				m_runs.push_back(tf);
			}
		}
	}

	m_bInsertionPhase = false;
}

ExternalSorter::Record* ExternalSorter::getNextRecord()
{
	if (m_bInsertionPhase == true)
		throw Tools::IllegalStateException("ExternalSorter::getNextRecord: Input has not been sorted yet.");

	Record* ret;

	if (m_sortedFile.get() == 0)
	{
		if (m_stI < m_buffer.size())
		{
			ret = m_buffer[m_stI];
			m_buffer[m_stI] = 0;
			++m_stI;
		}
		else
			throw Tools::EndOfStreamException("");
	}
	else
	{
		ret = new Record();
		ret->loadFromFile(*m_sortedFile);
	}

	return ret;
}

inline uint64_t ExternalSorter::getTotalEntries() const
{
	return m_u64TotalEntries;
}

//
// BulkLoader
//

void BulkLoader::bulkLoadUsingSTR(
	SpatialIndex::RTree::RTree* pTree,
	IDataStream& stream,
	uint32_t bindex,
	uint32_t bleaf,
	uint32_t pageSize,
	uint32_t numberOfPages
) {
	if (! stream.hasNext())
		throw Tools::IllegalArgumentException(
			"RTree::BulkLoader::bulkLoadUsingSTR: Empty data stream given."
		);

	NodePtr n = pTree->readNode(pTree->m_rootID);
	pTree->deleteNode(n.get());

	#ifndef NDEBUG
	std::cerr << "RTree::BulkLoader: Sorting data." << std::endl;
	#endif

	Tools::SmartPointer<ExternalSorter> es = Tools::SmartPointer<ExternalSorter>(new ExternalSorter(pageSize, numberOfPages));
	//std::cout << "In BulkLoader.c:343" << std::endl;
	int counter = 0;

        ts_type *ts = new ts_type[TS_SIZE];
        ts_type *paa = new ts_type[SUMMARIZATION_SIZE];
	
	while (stream.hasNext())
	{
	  //pTree->stats_count_partial_seq_input();	  
	  pTree->stats_count_partial_input_time_start();
	    
	  Data* d = reinterpret_cast<Data*>(stream.getNext());
	    
	  pTree->stats_count_partial_input_time_end();
		
	  if (d == 0)
	    throw Tools::IllegalArgumentException(
						  "bulkLoadUsingSTR: RTree bulk load expects SpatialIndex::RTree::Data entries."
						  );

	  es->insert(new ExternalSorter::Record(d->m_region, d->m_id, d->m_dataLength, d->m_pData, 0));
	  d->m_pData = 0;
	  delete d;
	  ++counter;
	  //std::cout <<  "Inserting record " << counter << std::endl;
	}
	es->sort();

	pTree->m_stats.m_u64Data = es->getTotalEntries();
	// create index levels.
	uint32_t level = 0;

	while (true)
	{
		#ifndef NDEBUG
		std::cerr << "RTree::BulkLoader: Building level " << level << std::endl;
		#endif

		pTree->m_stats.m_nodesInLevel.push_back(0);

		Tools::SmartPointer<ExternalSorter> es2 = Tools::SmartPointer<ExternalSorter>(new ExternalSorter(pageSize, numberOfPages));
		createLevel(pTree, es, 0, bleaf, bindex, level++, es2, pageSize, numberOfPages);
		es = es2;

		if (es->getTotalEntries() == 1) break;
		es->sort();
	}

	pTree->m_stats.m_u32TreeHeight = level;
	pTree->storeHeader();
}




void BulkLoader::bulkLoadUsingSTR(
	SpatialIndex::RTree::RTree* pTree,
	//std::ifstream& stream,
	char * filename,
	uint32_t dataset_size,	
	uint32_t bindex,
	uint32_t bleaf,
	uint32_t pageSize,
	uint32_t numberOfPages
) {

       std::ifstream stream(filename, std::ios::in | std::ios::binary);

       if (! stream)
       {
         std::cerr << "Cannot open input data file ." << std::endl;
	 return;
       }
		
	NodePtr n = pTree->readNode(pTree->m_rootID);
	pTree->deleteNode(n.get());

	#ifndef NDEBUG
	std::cerr << "RTree::BulkLoader: Sorting data." << std::endl;
	#endif

	Tools::SmartPointer<ExternalSorter> es = Tools::SmartPointer<ExternalSorter>(new ExternalSorter(pageSize, numberOfPages));
	//std::cout << "In BulkLoader.c:343" << std::endl;
	int counter = 0;

        ts_type *ts = new ts_type[TS_SIZE];
        ts_type *paa = new ts_type[SUMMARIZATION_SIZE];
	size_t count = 0;
	
        Data *d = new SpatialIndex::RTree::Data(sizeof(ts_type) * TS_SIZE);
	  
	while (stream)
	{
	  if (! stream.good()) continue; // skip newlines, etc.
	    //pTree->stats_count_partial_seq_input();	  
	    pTree->stats_count_partial_input_time_start();
	    for(int i=0; i<TS_SIZE; i++) { 
	        float val;
	        stream.read((char *) &val, sizeof(float));
	        ts[i] = (float) val;
	      } 
	    pTree->stats_count_partial_input_time_end();
	    
	    paa_from_ts(ts, paa, SUMMARIZATION_SIZE, TS_SIZE/SUMMARIZATION_SIZE);
	    
	    Point p = Point(paa, SUMMARIZATION_SIZE);	    
	    

      
	    SpatialIndex::RegionPtr mbr = pTree->m_regionPool.acquire();
	    p.getMBR(*mbr); //convert shape into region

	    d->fillData(sizeof(ts_type) * TS_SIZE,  reinterpret_cast<byte*>(ts), *mbr, counter);
	    
	    /*Data *d = new SpatialIndex::RTree::Data(sizeof(ts_type) * TS_SIZE,  reinterpret_cast<byte*>(ts), *mbr, counter);*/
	    
	    if (d == 0)
	      throw Tools::IllegalArgumentException(
						    "bulkLoadUsingSTR: RTree bulk load expects SpatialIndex::RTree::Data entries."
						    );
	    
	    /*es->insert(new ExternalSorter::Record(d->m_region, d->m_id, d->m_dataLength, d->m_pData, 0));*/
	    es->insert(new ExternalSorter::Record(d->m_region, d->m_id, d->m_dataLength, d->m_pData, 0));
	    d->m_pData = 0;
	    /* delete d;*/
	    ++counter;
	    if(counter > dataset_size) break;
	    if ((counter % 100000) == 0)
	      std::cerr << counter << std::endl;
	    //std::cout <<  "Inserting record " << counter << std::endl;
	}
	
	delete d;
	
	stream.close();
	es->sort();

	pTree->m_stats.m_u64Data = es->getTotalEntries();
	// create index levels.
	uint32_t level = 0;

	while (true)
	{
		#ifndef NDEBUG
		std::cerr << "RTree::BulkLoader: Building level " << level << std::endl;
		#endif

		pTree->m_stats.m_nodesInLevel.push_back(0);

		Tools::SmartPointer<ExternalSorter> es2 = Tools::SmartPointer<ExternalSorter>(new ExternalSorter(pageSize, numberOfPages));
		createLevel(pTree, es, 0, bleaf, bindex, level++, es2, pageSize, numberOfPages);
		es = es2;

		if (es->getTotalEntries() == 1) break;
		es->sort();
	}

	pTree->m_stats.m_u32TreeHeight = level;

	pTree->storeHeader();
}

void BulkLoader::createLevel(
	SpatialIndex::RTree::RTree* pTree,
	Tools::SmartPointer<ExternalSorter> es,
	uint32_t dimension,
	uint32_t bleaf,
	uint32_t bindex,
	uint32_t level,
	Tools::SmartPointer<ExternalSorter> es2,
	uint32_t pageSize,
	uint32_t numberOfPages
) {
	uint64_t b = (level == 0) ? bleaf : bindex;
	uint64_t P = static_cast<uint64_t>(std::ceil(static_cast<ts_type>(es->getTotalEntries()) / static_cast<ts_type>(b)));
	uint64_t S = static_cast<uint64_t>(std::ceil(std::sqrt(static_cast<ts_type>(P))));

	if (S == 1 || dimension == pTree->m_dimension - 1 || S * b == es->getTotalEntries())
	{
		std::vector<ExternalSorter::Record*> node;
		ExternalSorter::Record* r;

		while (true)
		{
			try { r = es->getNextRecord(); } catch (Tools::EndOfStreamException) { break; }
			node.push_back(r);

			if (node.size() == b)
			{
				Node* n = createNode(pTree, node, level);
				node.clear();
				pTree->writeNode(n);
				es2->insert(new ExternalSorter::Record(n->m_nodeMBR, n->m_identifier, 0, 0, 0));
				pTree->m_rootID = n->m_identifier;
					// special case when the root has exactly bindex entries.
				delete n;
			}
		}

		if (! node.empty())
		{
			Node* n = createNode(pTree, node, level);
			pTree->writeNode(n);
			es2->insert(new ExternalSorter::Record(n->m_nodeMBR, n->m_identifier, 0, 0, 0));
			pTree->m_rootID = n->m_identifier;
			delete n;
		}
	}
	else
	{
		bool bMore = true;

		while (bMore)
		{
			ExternalSorter::Record* pR;
			Tools::SmartPointer<ExternalSorter> es3 = Tools::SmartPointer<ExternalSorter>(new ExternalSorter(pageSize, numberOfPages));

			for (uint64_t i = 0; i < S * b; ++i)
			{
				try { pR = es->getNextRecord(); }
				catch (Tools::EndOfStreamException) { bMore = false; break; }
				pR->m_s = dimension + 1;
				es3->insert(pR);
			}
			es3->sort();
			createLevel(pTree, es3, dimension + 1, bleaf, bindex, level, es2, pageSize, numberOfPages);
		}
	}
}


Node* BulkLoader::createNode(SpatialIndex::RTree::RTree* pTree, std::vector<ExternalSorter::Record*>& e, uint32_t level)
{
	Node* n;

	if (level == 0) n = new Leaf(pTree, -1);
	else n = new Index(pTree, -1, level);

	for (size_t cChild = 0; cChild < e.size(); ++cChild)
	{
		n->insertEntry(e[cChild]->m_len, e[cChild]->m_pData, e[cChild]->m_r, e[cChild]->m_id);
		e[cChild]->m_pData = 0;
		delete e[cChild];
	}

	return n;
}
