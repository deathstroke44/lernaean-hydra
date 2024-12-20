/******************************************************************************
 * Project:  libsidx - A C API wrapper around libspatialindex
 * Purpose:	 C API wrapper implementation
 * Author:   Howard Butler, hobu.inc@gmail.com
 ******************************************************************************
 * Copyright (c) 2009, Howard Butler
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

#include <cmath>
#include <limits>
#include <spatialindex/capi/sidx_impl.h>

static std::stack<Error> errors;


#ifdef _WIN32
#  pragma warning(push)
#  pragma warning(disable: 4127)  // assignment operator could not be generated
#endif

#define VALIDATE_POINTER0(ptr, func) \
   do { if( NULL == ptr ) { \
		RTError const ret = RT_Failure; \
		std::ostringstream msg; \
		msg << "Pointer \'" << #ptr << "\' is NULL in \'" << (func) <<"\'."; \
		std::string message(msg.str()); \
		Error_PushError( ret, message.c_str(), (func)); \
		return; \
   }} while(0)

#define VALIDATE_POINTER1(ptr, func, rc) \
   do { if( NULL == ptr ) { \
		RTError const ret = RT_Failure; \
		std::ostringstream msg; \
		msg << "Pointer \'" << #ptr << "\' is NULL in \'" << (func) <<"\'."; \
		std::string message(msg.str()); \
		Error_PushError( ret, message.c_str(), (func)); \
		return (rc); \
   }} while(0)

IDX_C_START

SIDX_C_DLL void Error_Reset(void) {
	if (errors.empty()) return;
	for (std::size_t i=0;i<errors.size();i++) errors.pop();
}

SIDX_C_DLL void Error_Pop(void) {
	if (errors.empty()) return;
	errors.pop();
}

SIDX_C_DLL int Error_GetLastErrorNum(void){
	if (errors.empty())
		return 0;
	else {
		Error err = errors.top();
		return err.GetCode();
	}
}

SIDX_C_DLL char* Error_GetLastErrorMsg(void){
	if (errors.empty())
		return NULL;
	else {
		Error err = errors.top();
		return STRDUP(err.GetMessage());
	}
}

SIDX_C_DLL char* Error_GetLastErrorMethod(void){
	if (errors.empty())
		return NULL;
	else {
		Error err = errors.top();
		return STRDUP(err.GetMethod());
	}
}

SIDX_C_DLL void Error_PushError(int code, const char *message, const char *method) {
	Error err = Error(code, std::string(message), std::string(method));
	errors.push(err);
}

SIDX_C_DLL int Error_GetErrorCount(void) {
	return static_cast<int>(errors.size());
}

SIDX_C_DLL IndexH Index_Create(IndexPropertyH hProp)
{
	VALIDATE_POINTER1(hProp, "Index_Create", NULL);
	Tools::PropertySet* prop = static_cast<Tools::PropertySet*>(hProp);

	try {
		return (IndexH) new Index(*prop);
	} catch (Tools::Exception& e)
	{
		Error_PushError(RT_Failure,
						e.what().c_str(),
						"Index_Create");
		return NULL;
	} catch (std::exception const& e)
	{
		Error_PushError(RT_Failure,
						e.what(),
						"Index_Create");
		return NULL;
	} catch (...) {
		Error_PushError(RT_Failure,
						"Unknown Error",
						"Index_Create");
		return NULL;
	}
	return NULL;
}

SIDX_C_DLL IndexH Index_CreateWithStream( IndexPropertyH hProp,
										int (*readNext)(SpatialIndex::id_type *id, ts_type **pMin, ts_type **pMax, uint32_t *nDimension, const uint8_t **pData, uint32_t *nDataLength)
									   )
{
	VALIDATE_POINTER1(hProp, "Index_CreateWithStream", NULL);
	Tools::PropertySet* prop = static_cast<Tools::PropertySet*>(hProp);


	try {
		return (IndexH) new Index(*prop, readNext);
	} catch (Tools::Exception& e)
	{
		Error_PushError(RT_Failure,
						e.what().c_str(),
						"Index_CreateWithStream");
		return NULL;
	} catch (std::exception const& e)
	{
		Error_PushError(RT_Failure,
						e.what(),
						"Index_CreateWithStream");
		return NULL;
	} catch (...) {
		Error_PushError(RT_Failure,
						"Unknown Error",
						"Index_CreateWithStream");
		return NULL;
	}
	return NULL;
}

SIDX_C_DLL void Index_Destroy(IndexH index)
{
	VALIDATE_POINTER0(index, "Index_Destroy");
	Index* idx = (Index*) index;
	if (idx) delete idx;
}

SIDX_C_DLL void Index_Flush(IndexH index)
{
	VALIDATE_POINTER0(index, "Index_Flush");
	Index* idx = (Index*) index;
	if (idx)
	{
		idx->flush();
	}
}

SIDX_C_DLL RTError Index_DeleteTPData( IndexH index,
  int64_t id,
  ts_type* pdMin,
  ts_type* pdMax,
  ts_type* pdVMin,
  ts_type* pdVMax,
  ts_type tStart,
  ts_type tEnd,
  uint32_t nDimension
  )
{
  VALIDATE_POINTER1(index, "Index_DeleteTPData", RT_Failure);

  Index* idx = static_cast<Index*>(index);

  try {
    idx->index().deleteData(SpatialIndex::MovingRegion(pdMin, pdMax, pdVMin, pdVMax, tStart, tEnd, nDimension), id);
    return RT_None;
  } catch (Tools::Exception& e)
  {
    Error_PushError(RT_Failure,
            e.what().c_str(),
            "Index_DeleteTPData");
    return RT_Failure;
  } catch (std::exception const& e)
  {
    Error_PushError(RT_Failure,
            e.what(),
            "Index_DeleteTPData");
    return RT_Failure;
  } catch (...) {
    Error_PushError(RT_Failure,
            "Unknown Error",
            "Index_DeleteTPData");
    return RT_Failure;
  }
  return RT_None;
}

SIDX_C_DLL RTError Index_DeleteData(  IndexH index,
									int64_t id,
									ts_type* pdMin,
									ts_type* pdMax,
									uint32_t nDimension)
{
	VALIDATE_POINTER1(index, "Index_DeleteData", RT_Failure);

	Index* idx = static_cast<Index*>(index);

	try {
		idx->index().deleteData(SpatialIndex::Region(pdMin, pdMax, nDimension), id);
		return RT_None;
	} catch (Tools::Exception& e)
	{
		Error_PushError(RT_Failure,
						e.what().c_str(),
						"Index_DeleteData");
		return RT_Failure;
	} catch (std::exception const& e)
	{
		Error_PushError(RT_Failure,
						e.what(),
						"Index_DeleteData");
		return RT_Failure;
	} catch (...) {
		Error_PushError(RT_Failure,
						"Unknown Error",
						"Index_DeleteData");
		return RT_Failure;
	}
	return RT_None;
}

SIDX_C_DLL RTError Index_InsertTPData( IndexH index,
  int64_t id,
  ts_type* pdMin,
  ts_type* pdMax,
  ts_type* pdVMin,
  ts_type* pdVMax,
  ts_type tStart,
  ts_type tEnd,
  uint32_t nDimension,
  const uint8_t* pData,
  uint32_t nDataLength
  )
{
  VALIDATE_POINTER1(index, "Index_InsertTPData", RT_Failure);

  Index* idx = static_cast<Index*>(index);

  // Test the data and check for the case when mins equal maxs (x,y,z,v)
  // In that case, we will insert a SpatialIndex::MovingPoint
  // instead of a SpatialIndex::MovingRegion

  bool isPoint = false;
  SpatialIndex::IShape* shape = 0;
  ts_type const epsilon = std::numeric_limits<ts_type>::epsilon();

  ts_type length(0), vlength(0);
  for (uint32_t i = 0; i < nDimension; ++i) {
    ts_type delta = pdMin[i] - pdMax[i];
    length += std::fabs(delta);

    ts_type vDelta = pdVMin[i] - pdVMax[i];
    vlength += std::fabs(vDelta);
  }

  if ((length <= epsilon) && (vlength <= epsilon)){
    isPoint = true;
  }

  if (isPoint == true) {
    shape = new SpatialIndex::MovingPoint(pdMin, pdVMin, tStart, tEnd, nDimension);
  } else {
    shape = new SpatialIndex::MovingRegion(pdMin, pdMax, pdVMin, pdVMax, tStart, tEnd, nDimension);
  }
  try {
    idx->index().insertData(nDataLength,
                pData,
                *shape,
                id);

    delete shape;
    return RT_None;

  } catch (Tools::Exception& e)
  {
    Error_PushError(RT_Failure,
            e.what().c_str(),
            "Index_InsertTPData");
    delete shape;
    return RT_Failure;
  } catch (std::exception const& e)
  {
    Error_PushError(RT_Failure,
            e.what(),
            "Index_InsertTPData");
    delete shape;
    return RT_Failure;
  } catch (...) {
    Error_PushError(RT_Failure,
            "Unknown Error",
            "Index_InsertTPData");
    delete shape;
    return RT_Failure;
  }
  return RT_None;

}



SIDX_C_DLL RTError Index_InsertData(  IndexH index,
									int64_t id,
									ts_type* pdMin,
									ts_type* pdMax,
									uint32_t nDimension,
									const uint8_t* pData,
									uint32_t nDataLength)
{
	VALIDATE_POINTER1(index, "Index_InsertData", RT_Failure);

	Index* idx = static_cast<Index*>(index);

	// Test the data and check for the case when minx == maxx, miny == maxy
	// and minz == maxz.  In that case, we will insert a SpatialIndex::Point
	// instead of a SpatialIndex::Region

	bool isPoint = false;
	SpatialIndex::IShape* shape = 0;
	ts_type const epsilon = std::numeric_limits<ts_type>::epsilon();

	ts_type length(0);
	for (uint32_t i = 0; i < nDimension; ++i) {
		ts_type delta = pdMin[i] - pdMax[i];
		length += std::fabs(delta);
	}

	if (length <= epsilon) {
		isPoint = true;
	}

	if (isPoint == true) {
		shape = new SpatialIndex::Point(pdMin, nDimension);
	} else {
		shape = new SpatialIndex::Region(pdMin, pdMax, nDimension);
	}
	try {
		idx->index().insertData(nDataLength,
								pData,
								*shape,
								id);

		delete shape;
		return RT_None;

	} catch (Tools::Exception& e)
	{
		Error_PushError(RT_Failure,
						e.what().c_str(),
						"Index_InsertData");
		delete shape;
		return RT_Failure;
	} catch (std::exception const& e)
	{
		Error_PushError(RT_Failure,
						e.what(),
						"Index_InsertData");
		delete shape;
		return RT_Failure;
	} catch (...) {
		Error_PushError(RT_Failure,
						"Unknown Error",
						"Index_InsertData");
		delete shape;
		return RT_Failure;
	}
	return RT_None;
}

SIDX_C_DLL RTError Index_TPIntersects_obj(  IndexH index,
                    ts_type* pdMin,
                    ts_type* pdMax,
                    ts_type* pdVMin,
                    ts_type* pdVMax,
                    ts_type tStart,
                    ts_type tEnd,
                    uint32_t nDimension,
                    IndexItemH** items,
                    uint64_t* nResults)
{
  VALIDATE_POINTER1(index, "Index_TPIntersects_obj", RT_Failure);
  Index* idx = static_cast<Index*>(index);
  int64_t nResultLimit, nStart;

  nResultLimit = idx->GetResultSetLimit();
  nStart = idx->GetResultSetOffset();

  ObjVisitor* visitor = new ObjVisitor;
  try {
    SpatialIndex::MovingRegion* r = new SpatialIndex::MovingRegion(pdMin, pdMax, pdVMin, pdVMax, tStart, tEnd, nDimension);
    idx->index().intersectsWithQuery(	*r,
                      *visitor);

    Page_ResultSet_Obj(*visitor, items, nStart, nResultLimit, nResults);

    delete r;
    delete visitor;

  } catch (Tools::Exception& e)
  {
    Error_PushError(RT_Failure,
            e.what().c_str(),
            "Index_TPIntersects_obj");
    delete visitor;
    return RT_Failure;
  } catch (std::exception const& e)
  {
    Error_PushError(RT_Failure,
            e.what(),
            "Index_TPIntersects_obj");
    delete visitor;
    return RT_Failure;
  } catch (...) {
    Error_PushError(RT_Failure,
            "Unknown Error",
            "Index_TPIntersects_obj");
    delete visitor;
    return RT_Failure;
  }
  return RT_None;
}


SIDX_C_DLL RTError Index_Intersects_obj(  IndexH index,
										ts_type* pdMin,
										ts_type* pdMax,
										uint32_t nDimension,
										IndexItemH** items,
										uint64_t* nResults)
{
	VALIDATE_POINTER1(index, "Index_Intersects_obj", RT_Failure);
	Index* idx = static_cast<Index*>(index);
	int64_t nResultLimit, nStart;

	nResultLimit = idx->GetResultSetLimit();
	nStart = idx->GetResultSetOffset();

	ObjVisitor* visitor = new ObjVisitor;
	try {
    SpatialIndex::Region* r = new SpatialIndex::Region(pdMin, pdMax, nDimension);
		idx->index().intersectsWithQuery(	*r,
											*visitor);

    Page_ResultSet_Obj(*visitor, items, nStart, nResultLimit, nResults);

    delete r;
		delete visitor;

	} catch (Tools::Exception& e)
	{
		Error_PushError(RT_Failure,
						e.what().c_str(),
						"Index_Intersects_obj");
		delete visitor;
		return RT_Failure;
	} catch (std::exception const& e)
	{
		Error_PushError(RT_Failure,
						e.what(),
						"Index_Intersects_obj");
		delete visitor;
		return RT_Failure;
	} catch (...) {
		Error_PushError(RT_Failure,
						"Unknown Error",
						"Index_Intersects_obj");
		delete visitor;
		return RT_Failure;
	}
	return RT_None;
}

SIDX_C_DLL RTError Index_TPIntersects_id(  IndexH index,
                    ts_type* pdMin,
                    ts_type* pdMax,
                    ts_type* pdVMin,
                    ts_type* pdVMax,
                    ts_type tStart,
                    ts_type tEnd,
                    uint32_t nDimension,
                    int64_t** ids,
                    uint64_t* nResults)
{
  VALIDATE_POINTER1(index, "Index_TPIntersects_id", RT_Failure);
  Index* idx = static_cast<Index*>(index);

  int64_t nResultLimit, nStart;

  nResultLimit = idx->GetResultSetLimit();
  nStart = idx->GetResultSetOffset();

  IdVisitor* visitor = new IdVisitor;
  try {
    SpatialIndex::MovingRegion* r = new SpatialIndex::MovingRegion(pdMin, pdMax, pdVMin, pdVMax, tStart, tEnd, nDimension);
    idx->index().intersectsWithQuery(	*r,
                      *visitor);

    Page_ResultSet_Ids(*visitor, ids, nStart, nResultLimit, nResults);

    delete r;
    delete visitor;

  } catch (Tools::Exception& e)
  {
    Error_PushError(RT_Failure,
            e.what().c_str(),
            "Index_TPIntersects_id");
    delete visitor;
    return RT_Failure;
  } catch (std::exception const& e)
  {
    Error_PushError(RT_Failure,
            e.what(),
            "Index_TPIntersects_id");
    delete visitor;
    return RT_Failure;
  } catch (...) {
    Error_PushError(RT_Failure,
            "Unknown Error",
            "Index_TPIntersects_id");
    delete visitor;
    return RT_Failure;
  }
  return RT_None;
}


SIDX_C_DLL RTError Index_Intersects_id(	  IndexH index,
										ts_type* pdMin,
										ts_type* pdMax,
										uint32_t nDimension,
										int64_t** ids,
										uint64_t* nResults)
{
	VALIDATE_POINTER1(index, "Index_Intersects_id", RT_Failure);
	Index* idx = static_cast<Index*>(index);

	int64_t nResultLimit, nStart;

	nResultLimit = idx->GetResultSetLimit();
  nStart = idx->GetResultSetOffset();

	IdVisitor* visitor = new IdVisitor;
	try {
    SpatialIndex::Region* r = new SpatialIndex::Region(pdMin, pdMax, nDimension);
		idx->index().intersectsWithQuery(	*r,
											*visitor);

    Page_ResultSet_Ids(*visitor, ids, nStart, nResultLimit, nResults);

    delete r;
		delete visitor;

	} catch (Tools::Exception& e)
	{
		Error_PushError(RT_Failure,
						e.what().c_str(),
						"Index_Intersects_id");
		delete visitor;
		return RT_Failure;
	} catch (std::exception const& e)
	{
		Error_PushError(RT_Failure,
						e.what(),
						"Index_Intersects_id");
		delete visitor;
		return RT_Failure;
	} catch (...) {
		Error_PushError(RT_Failure,
						"Unknown Error",
						"Index_Intersects_id");
		delete visitor;
		return RT_Failure;
	}
	return RT_None;
}

SIDX_C_DLL RTError Index_TPIntersects_count(	  IndexH index,
                    ts_type* pdMin,
                    ts_type* pdMax,
                    ts_type* pdVMin,
                    ts_type* pdVMax,
                    ts_type tStart,
                    ts_type tEnd,
                    uint32_t nDimension,
                    uint64_t* nResults)
{
  VALIDATE_POINTER1(index, "Index_TPIntersects_count", RT_Failure);
  Index* idx = static_cast<Index*>(index);

  CountVisitor* visitor = new CountVisitor;
  try {
    SpatialIndex::MovingRegion* r = new SpatialIndex::MovingRegion(pdMin, pdMax, pdVMin, pdVMax, tStart, tEnd, nDimension);
    idx->index().intersectsWithQuery(	*r,
                      *visitor);

    *nResults = visitor->GetResultCount();

    delete r;
    delete visitor;

  } catch (Tools::Exception& e)
  {
    Error_PushError(RT_Failure,
            e.what().c_str(),
            "Index_TPIntersects_count");
    delete visitor;
    return RT_Failure;
  } catch (std::exception const& e)
  {
    Error_PushError(RT_Failure,
            e.what(),
            "Index_TPIntersects_count");
    delete visitor;
    return RT_Failure;
  } catch (...) {
    Error_PushError(RT_Failure,
            "Unknown Error",
            "Index_TPIntersects_count");
    delete visitor;
    return RT_Failure;
  }
  return RT_None;
}


SIDX_C_DLL RTError Index_Intersects_count(	  IndexH index,
										ts_type* pdMin,
										ts_type* pdMax,
										uint32_t nDimension,
										uint64_t* nResults)
{
	VALIDATE_POINTER1(index, "Index_Intersects_count", RT_Failure);
	Index* idx = static_cast<Index*>(index);

	CountVisitor* visitor = new CountVisitor;
	try {
    SpatialIndex::Region* r = new SpatialIndex::Region(pdMin, pdMax, nDimension);
		idx->index().intersectsWithQuery(	*r,
											*visitor);

		*nResults = visitor->GetResultCount();

		delete r;
		delete visitor;

	} catch (Tools::Exception& e)
	{
		Error_PushError(RT_Failure,
						e.what().c_str(),
						"Index_Intersects_count");
		delete visitor;
		return RT_Failure;
	} catch (std::exception const& e)
	{
		Error_PushError(RT_Failure,
						e.what(),
						"Index_Intersects_count");
		delete visitor;
		return RT_Failure;
	} catch (...) {
		Error_PushError(RT_Failure,
						"Unknown Error",
						"Index_Intersects_count");
		delete visitor;
		return RT_Failure;
	}
	return RT_None;
}

SIDX_C_DLL RTError Index_SegmentIntersects_obj(  IndexH index,
										ts_type* pdStartPoint,
										ts_type* pdEndPoint,
										uint32_t nDimension,
										IndexItemH** items,
										uint64_t* nResults)
{
	VALIDATE_POINTER1(index, "Index_Intersects_obj", RT_Failure);
	Index* idx = static_cast<Index*>(index);

  int64_t nResultLimit, nStart;

  nResultLimit = idx->GetResultSetLimit();
  nStart = idx->GetResultSetOffset();

	ObjVisitor* visitor = new ObjVisitor;
	try {
    SpatialIndex::LineSegment* l = new SpatialIndex::LineSegment(pdStartPoint, pdEndPoint, nDimension);
		idx->index().intersectsWithQuery(	*l,
											*visitor);

    Page_ResultSet_Obj(*visitor, items, nStart, nResultLimit, nResults);

    delete l;
		delete visitor;

	} catch (Tools::Exception& e)
	{
		Error_PushError(RT_Failure,
						e.what().c_str(),
						"Index_Intersects_obj");
		delete visitor;
		return RT_Failure;
	} catch (std::exception const& e)
	{
		Error_PushError(RT_Failure,
						e.what(),
						"Index_Intersects_obj");
		delete visitor;
		return RT_Failure;
	} catch (...) {
		Error_PushError(RT_Failure,
						"Unknown Error",
						"Index_Intersects_obj");
		delete visitor;
		return RT_Failure;
	}
	return RT_None;
}

SIDX_C_DLL RTError Index_SegmentIntersects_id(	  IndexH index,
										ts_type* pdStartPoint,
										ts_type* pdEndPoint,
										uint32_t nDimension,
										int64_t** ids,
										uint64_t* nResults)
{
	VALIDATE_POINTER1(index, "Index_Intersects_id", RT_Failure);
	Index* idx = static_cast<Index*>(index);
  int64_t nResultLimit, nStart;

  nResultLimit = idx->GetResultSetLimit();
  nStart = idx->GetResultSetOffset();

	IdVisitor* visitor = new IdVisitor;
	try {
    SpatialIndex::LineSegment* l = new SpatialIndex::LineSegment(pdStartPoint, pdEndPoint, nDimension);
		idx->index().intersectsWithQuery(	*l,
											*visitor);

    Page_ResultSet_Ids(*visitor, ids, nStart, nResultLimit, nResults);

    delete l;
		delete visitor;

	} catch (Tools::Exception& e)
	{
		Error_PushError(RT_Failure,
						e.what().c_str(),
						"Index_Intersects_id");
		delete visitor;
		return RT_Failure;
	} catch (std::exception const& e)
	{
		Error_PushError(RT_Failure,
						e.what(),
						"Index_Intersects_id");
		delete visitor;
		return RT_Failure;
	} catch (...) {
		Error_PushError(RT_Failure,
						"Unknown Error",
						"Index_Intersects_id");
		delete visitor;
		return RT_Failure;
	}
	return RT_None;
}

SIDX_C_DLL RTError Index_SegmentIntersects_count(	  IndexH index,
										ts_type* pdStartPoint,
										ts_type* pdEndPoint,
										uint32_t nDimension,
										uint64_t* nResults)
{
	VALIDATE_POINTER1(index, "Index_Intersects_count", RT_Failure);
	Index* idx = static_cast<Index*>(index);

	CountVisitor* visitor = new CountVisitor;
	try {
        SpatialIndex::LineSegment* l = new SpatialIndex::LineSegment(pdStartPoint, pdEndPoint, nDimension);
		idx->index().intersectsWithQuery(	*l,
											*visitor);

		*nResults = visitor->GetResultCount();

		delete l;
		delete visitor;

	} catch (Tools::Exception& e)
	{
		Error_PushError(RT_Failure,
						e.what().c_str(),
						"Index_Intersects_count");
		delete visitor;
		return RT_Failure;
	} catch (std::exception const& e)
	{
		Error_PushError(RT_Failure,
						e.what(),
						"Index_Intersects_count");
		delete visitor;
		return RT_Failure;
	} catch (...) {
		Error_PushError(RT_Failure,
						"Unknown Error",
						"Index_Intersects_count");
		delete visitor;
		return RT_Failure;
	}
	return RT_None;
}

SIDX_C_DLL RTError Index_TPNearestNeighbors_id(IndexH index,
                      ts_type* pdMin,
                      ts_type* pdMax,
                      ts_type* pdVMin,
                      ts_type* pdVMax,
                      ts_type tStart,
                      ts_type tEnd,
                      uint32_t nDimension,
                      int64_t** ids,
                      uint64_t* nResults)
{
  VALIDATE_POINTER1(index, "Index_TPNearestNeighbors_id", RT_Failure);
  Index* idx = static_cast<Index*>(index);
  int64_t nResultLimit, nStart;

  nResultLimit = idx->GetResultSetLimit();
  nStart = idx->GetResultSetOffset();

  IdVisitor* visitor = new IdVisitor;

  try {
    SpatialIndex::MovingRegion* r = new SpatialIndex::MovingRegion(pdMin, pdMax, pdVMin, pdVMax, tStart, tEnd, nDimension);
    idx->index().nearestNeighborQuery(	static_cast<uint32_t>(*nResults),
                      *r,
                      *visitor);

    Page_ResultSet_Ids(*visitor, ids, nStart, nResultLimit, nResults);

    delete r;
    delete visitor;

  } catch (Tools::Exception& e)
  {
    Error_PushError(RT_Failure,
            e.what().c_str(),
            "Index_TPNearestNeighbors_id");
    delete visitor;
    return RT_Failure;
  } catch (std::exception const& e)
  {
    Error_PushError(RT_Failure,
            e.what(),
            "Index_TPNearestNeighbors_id");
    delete visitor;
    return RT_Failure;
  } catch (...) {
    Error_PushError(RT_Failure,
            "Unknown Error",
            "Index_TPNearestNeighbors_id");
    delete visitor;
    return RT_Failure;
  }
  return RT_None;
}


SIDX_C_DLL RTError Index_NearestNeighbors_id(IndexH index,
											ts_type* pdMin,
											ts_type* pdMax,
											uint32_t nDimension,
											int64_t** ids,
											uint64_t* nResults)
{
	VALIDATE_POINTER1(index, "Index_NearestNeighbors_id", RT_Failure);
	Index* idx = static_cast<Index*>(index);
  int64_t nResultLimit, nStart;

  nResultLimit = idx->GetResultSetLimit();
  nStart = idx->GetResultSetOffset();

	IdVisitor* visitor = new IdVisitor;

	try {
    SpatialIndex::Region* r = new SpatialIndex::Region(pdMin, pdMax, nDimension);

		idx->index().nearestNeighborQuery(	static_cast<uint32_t>(*nResults),
											*r,
											*visitor);

		Page_ResultSet_Ids(*visitor, ids, nStart, nResultLimit, nResults);

    delete r;
		delete visitor;

	} catch (Tools::Exception& e)
	{
		Error_PushError(RT_Failure,
						e.what().c_str(),
						"Index_NearestNeighbors_id");
		delete visitor;
		return RT_Failure;
	} catch (std::exception const& e)
	{
		Error_PushError(RT_Failure,
						e.what(),
						"Index_NearestNeighbors_id");
		delete visitor;
		return RT_Failure;
	} catch (...) {
		Error_PushError(RT_Failure,
						"Unknown Error",
						"Index_NearestNeighbors_id");
		delete visitor;
		return RT_Failure;
	}
	return RT_None;
}

SIDX_C_DLL RTError Index_TPNearestNeighbors_obj(IndexH index,
                      ts_type* pdMin,
                      ts_type* pdMax,
                      ts_type* pdVMin,
                      ts_type* pdVMax,
                      ts_type tStart,
                      ts_type tEnd,
                      uint32_t nDimension,
                      IndexItemH** items,
                      uint64_t* nResults)
{
  VALIDATE_POINTER1(index, "Index_TPNearestNeighbors_obj", RT_Failure);
  Index* idx = static_cast<Index*>(index);

  int64_t nResultLimit, nStart;

  nResultLimit = idx->GetResultSetLimit();
  nStart = idx->GetResultSetOffset();

  ObjVisitor* visitor = new ObjVisitor;
  try {
    SpatialIndex::MovingRegion* r = new SpatialIndex::MovingRegion(pdMin, pdMax, pdVMin, pdVMax, tStart, tEnd, nDimension);

    idx->index().nearestNeighborQuery(	static_cast<uint32_t>(*nResults),
                      *r,
                      *visitor);

    Page_ResultSet_Obj(*visitor, items, nStart, nResultLimit, nResults);

    delete r;
    delete visitor;

  } catch (Tools::Exception& e)
  {
    Error_PushError(RT_Failure,
            e.what().c_str(),
            "Index_TPNearestNeighbors_obj");
    delete visitor;
    return RT_Failure;
  } catch (std::exception const& e)
  {
    Error_PushError(RT_Failure,
            e.what(),
            "Index_TPNearestNeighbors_obj");
    delete visitor;
    return RT_Failure;
  } catch (...) {
    Error_PushError(RT_Failure,
            "Unknown Error",
            "Index_NearestNeighbors_obj");
    delete visitor;
    return RT_Failure;
  }
  return RT_None;
}


SIDX_C_DLL RTError Index_NearestNeighbors_obj(IndexH index,
											ts_type* pdMin,
											ts_type* pdMax,
											uint32_t nDimension,
											IndexItemH** items,
											uint64_t* nResults)
{
	VALIDATE_POINTER1(index, "Index_NearestNeighbors_obj", RT_Failure);
	Index* idx = static_cast<Index*>(index);

  int64_t nResultLimit, nStart;

  nResultLimit = idx->GetResultSetLimit();
  nStart = idx->GetResultSetOffset();

	ObjVisitor* visitor = new ObjVisitor;
	try {
    SpatialIndex::Region* r = new SpatialIndex::Region(pdMin, pdMax, nDimension);

		idx->index().nearestNeighborQuery(	static_cast<uint32_t>(*nResults),
											*r,
											*visitor);

    Page_ResultSet_Obj(*visitor, items, nStart, nResultLimit, nResults);

    delete r;
		delete visitor;

	} catch (Tools::Exception& e)
	{
		Error_PushError(RT_Failure,
						e.what().c_str(),
						"Index_NearestNeighbors_obj");
		delete visitor;
		return RT_Failure;
	} catch (std::exception const& e)
	{
		Error_PushError(RT_Failure,
						e.what(),
						"Index_NearestNeighbors_obj");
		delete visitor;
		return RT_Failure;
	} catch (...) {
		Error_PushError(RT_Failure,
						"Unknown Error",
						"Index_NearestNeighbors_obj");
		delete visitor;
		return RT_Failure;
	}
	return RT_None;
}

SIDX_C_DLL RTError Index_GetBounds(	  IndexH index,
									ts_type** ppdMin,
									ts_type** ppdMax,
									uint32_t* nDimension)
{
	VALIDATE_POINTER1(index, "Index_GetBounds", RT_Failure);
	Index* idx = static_cast<Index*>(index);

	BoundsQuery* query = new BoundsQuery;

	try {
		idx->index().queryStrategy( *query);

		const SpatialIndex::Region* bounds = query->GetBounds();
		if (bounds == 0) {
			*nDimension = 0;
			delete query;
			return RT_None;
		}

		*nDimension =bounds->getDimension();

		*ppdMin = (ts_type*) malloc (*nDimension * sizeof(ts_type));
		*ppdMax = (ts_type*) malloc (*nDimension * sizeof(ts_type));

		for (uint32_t i=0; i< *nDimension; ++i) {
			(*ppdMin)[i] = bounds->getLow(i);
			(*ppdMax)[i] = bounds->getHigh(i);
		}

		delete query;

	} catch (Tools::Exception& e)
	{
		Error_PushError(RT_Failure,
						e.what().c_str(),
						"Index_GetBounds");
		delete query;
		return RT_Failure;
	} catch (std::exception const& e)
	{
		Error_PushError(RT_Failure,
						e.what(),
						"Index_GetBounds");
		delete query;
		return RT_Failure;
	} catch (...) {
		Error_PushError(RT_Failure,
						"Unknown Error",
						"Index_GetBounds");
		delete query;
		return RT_Failure;
	}
	return RT_None;
}

SIDX_DLL RTError Index_SetResultSetOffset(IndexH index, int64_t value)
{
	try
	{
		VALIDATE_POINTER1(index, "Index_SetResultSetOffset", RT_Failure);
		Index* idx = static_cast<Index*>(index);
		idx->SetResultSetOffset(value);
	}
	catch (...) {
		Error_PushError(RT_Failure,
						"Unknown Error",
						"Index_SetResultSetOffset");
		return RT_Failure;
	}
	return RT_None;
}

SIDX_DLL int64_t Index_GetResultSetOffset(IndexH index)
{
	VALIDATE_POINTER1(index, "Index_GetResultSetOffset", 0);
	Index* idx = static_cast<Index*>(index);
	return idx->GetResultSetOffset();
}

SIDX_DLL RTError Index_SetResultSetLimit(IndexH index, int64_t value)
{
	try
	{
		VALIDATE_POINTER1(index, "Index_SetResultSetLimit", RT_Failure);
		Index* idx = static_cast<Index*>(index);
		idx->SetResultSetLimit(value);
	}
	catch (...) {
		Error_PushError(RT_Failure,
						"Unknown Error",
						"Index_SetResultSetLimit");
		return RT_Failure;
	}
	return RT_None;
}

SIDX_DLL int64_t Index_GetResultSetLimit(IndexH index)
{
	VALIDATE_POINTER1(index, "Index_GetResultSetLimit", 0);
	Index* idx = static_cast<Index*>(index);
	return idx->GetResultSetLimit();
}

SIDX_C_DLL uint32_t Index_IsValid(IndexH index)
{
	VALIDATE_POINTER1(index, "Index_IsValid", 0);
	Index* idx = static_cast<Index*>(index);
	return static_cast<uint32_t>(idx->index().isIndexValid());
}

SIDX_C_DLL IndexPropertyH Index_GetProperties(IndexH index)
{
	VALIDATE_POINTER1(index, "Index_GetProperties", 0);
	Index* idx = static_cast<Index*>(index);
	Tools::PropertySet* ps = new Tools::PropertySet;

	idx->index().getIndexProperties(*ps);
	return (IndexPropertyH)ps;
}

SIDX_C_DLL void Index_ClearBuffer(IndexH index)
{
	VALIDATE_POINTER0(index, "Index_ClearBuffer");
	Index* idx = static_cast<Index*>(index);
	idx->buffer().clear();
}

SIDX_C_DLL void Index_DestroyObjResults(IndexItemH* results, uint32_t nResults)
{
	VALIDATE_POINTER0(results, "Index_DestroyObjResults");
	SpatialIndex::IData* it;
	for (uint32_t i=0; i< nResults; ++i) {
		if (results[i] != NULL) {
			it = static_cast<SpatialIndex::IData*>(results[i]);
			if (it != 0)
				delete it;
		}
	}

	std::free(results);
}


SIDX_C_DLL void Index_Free(void* results)
{
	VALIDATE_POINTER0(results, "Index_Free");
	if (results != 0)
	    std::free(results);
}

SIDX_C_DLL RTError Index_GetLeaves(	IndexH index,
									uint32_t* nNumLeafNodes,
									uint32_t** nLeafSizes,
									int64_t** nLeafIDs,
									int64_t*** nLeafChildIDs,
									ts_type*** pppdMin,
									ts_type*** pppdMax,
									uint32_t* nDimension)
{
	VALIDATE_POINTER1(index, "Index_GetLeaves", RT_Failure);
	Index* idx = static_cast<Index*>(index);

	std::vector<LeafQueryResult>::const_iterator i;
	LeafQuery* query = new LeafQuery;

	// Fetch the dimensionality of the index
	Tools::PropertySet ps;
	idx->index().getIndexProperties(ps);

	Tools::Variant var;
	var = ps.getProperty("Dimension");

	if (var.m_varType != Tools::VT_EMPTY)
	{
		if (var.m_varType != Tools::VT_ULONG) {
			Error_PushError(RT_Failure,
							"Property Dimension must be Tools::VT_ULONG",
							"Index_GetLeaves");
			return RT_Failure;
		}
	}

	*nDimension = var.m_val.ulVal;

	try {
		idx->index().queryStrategy( *query);

		const std::vector<LeafQueryResult>& results = query->GetResults();

		*nNumLeafNodes = (uint32_t)results.size();

		*nLeafSizes = (uint32_t*) malloc (*nNumLeafNodes * sizeof(uint32_t));
		*nLeafIDs = (int64_t*) malloc (*nNumLeafNodes * sizeof(int64_t));

		*nLeafChildIDs = (int64_t**) malloc(*nNumLeafNodes * sizeof(int64_t*));
		*pppdMin = (ts_type**) malloc (*nNumLeafNodes * sizeof(ts_type*));
		*pppdMax = (ts_type**) malloc (*nNumLeafNodes * sizeof(ts_type*));

		uint32_t k=0;
		for (i = results.begin(); i != results.end(); ++i)
		{
			std::vector<SpatialIndex::id_type> const& ids = (*i).GetIDs();
			const SpatialIndex::Region* b = (*i).GetBounds();

			(*nLeafIDs)[k] = (*i).getIdentifier();
			(*nLeafSizes)[k] = (uint32_t)ids.size();

			(*nLeafChildIDs)[k] = (int64_t*) malloc( (*nLeafSizes)[k] * sizeof(int64_t));
			(*pppdMin)[k] = (ts_type*) malloc ( (*nLeafSizes)[k] *  sizeof(ts_type));
			(*pppdMax)[k] = (ts_type*) malloc ( (*nLeafSizes)[k] *  sizeof(ts_type));
			for (uint32_t i=0; i< *nDimension; ++i) {
				(*pppdMin)[k][i] = b->getLow(i);
				(*pppdMax)[k][i] = b->getHigh(i);
			}
			for (uint32_t cChild = 0; cChild < ids.size(); cChild++)
			{
				(*nLeafChildIDs)[k][cChild] = ids[cChild];
			}
			++k;
		}


		delete query;

	} catch (Tools::Exception& e)
	{
		Error_PushError(RT_Failure,
						e.what().c_str(),
						"Index_GetLeaves");
		delete query;
		return RT_Failure;
	} catch (std::exception const& e)
	{
		Error_PushError(RT_Failure,
						e.what(),
						"Index_GetLeaves");
		delete query;
		return RT_Failure;
	} catch (...) {
		Error_PushError(RT_Failure,
						"Unknown Error",
						"Index_GetLeaves");
		delete query;
		return RT_Failure;
	}
	return RT_None;
}


SIDX_C_DLL void IndexItem_Destroy(IndexItemH item)
{
	VALIDATE_POINTER0(item, "IndexItem_Destroy");
	SpatialIndex::IData* it = static_cast<SpatialIndex::IData*>(item);
	if (it != 0) delete it;
}

SIDX_C_DLL RTError IndexItem_GetData( IndexItemH item,
									uint8_t** data,
									uint64_t* length)
{
	VALIDATE_POINTER1(item, "IndexItem_GetData", RT_Failure);
	SpatialIndex::IData* it = static_cast<SpatialIndex::IData*>(item);
    uint8_t* p_data;
    uint32_t* l= new uint32_t;

	it->getData(*l,&p_data);
	*length = (uint64_t)*l;
	*data = (uint8_t*) malloc (*length * sizeof(uint8_t));

	memcpy(*data, p_data, *length);
        delete[] p_data;
        delete l;
	return RT_None;

}

SIDX_C_DLL int64_t IndexItem_GetID(IndexItemH item)
{
	VALIDATE_POINTER1(item, "IndexItem_GetID",0);
	SpatialIndex::IData* it = static_cast<SpatialIndex::IData*>(item);
	int64_t value = it->getIdentifier();
	return value;
}

SIDX_C_DLL RTError IndexItem_GetBounds(	  IndexItemH item,
										ts_type** ppdMin,
										ts_type** ppdMax,
										uint32_t* nDimension)
{
	VALIDATE_POINTER1(item, "IndexItem_GetBounds", RT_Failure);
	SpatialIndex::IData* it = static_cast<SpatialIndex::IData*>(item);

	SpatialIndex::IShape* s;
    it->getShape(&s);

	SpatialIndex::Region *bounds = new SpatialIndex::Region();
    s->getMBR(*bounds);

	if (bounds == 0) {
		*nDimension = 0;
                delete bounds;
                delete s;
		return RT_None;
	}
	*nDimension = bounds->getDimension();

	*ppdMin = (ts_type*) malloc (*nDimension * sizeof(ts_type));
	*ppdMax = (ts_type*) malloc (*nDimension * sizeof(ts_type));

	if (ppdMin == NULL || ppdMax == NULL) {
		Error_PushError(RT_Failure,
						"Unable to allocation bounds array(s)",
						"IndexItem_GetBounds");
		return RT_Failure;
	}

	for (uint32_t i=0; i< *nDimension; ++i) {
		(*ppdMin)[i] = bounds->getLow(i);
		(*ppdMax)[i] = bounds->getHigh(i);
	}
	delete bounds;
	delete s;
	return RT_None;
}
SIDX_C_DLL IndexPropertyH IndexProperty_Create()
{
	Tools::PropertySet* ps = GetDefaults();
	Tools::Variant var;
	return (IndexPropertyH)ps;
}

SIDX_C_DLL void IndexProperty_Destroy(IndexPropertyH hProp)
{
	VALIDATE_POINTER0(hProp, "IndexProperty_Destroy");
	Tools::PropertySet* prop = static_cast<Tools::PropertySet*>(hProp);
	if (prop != 0) delete prop;
}

SIDX_C_DLL RTError IndexProperty_SetIndexType(IndexPropertyH hProp,
											RTIndexType value)
{
	VALIDATE_POINTER1(hProp, "IndexProperty_SetIndexType", RT_Failure);
	Tools::PropertySet* prop = static_cast<Tools::PropertySet*>(hProp);

	try
	{
	  if (!(value == RT_RTree )) {
			throw std::runtime_error("Inputted value is not a valid index type");
		}
		Tools::Variant var;
		var.m_varType = Tools::VT_ULONG;
		var.m_val.ulVal = value;
		prop->setProperty("IndexType", var);


	} catch (Tools::Exception& e)
	{
		Error_PushError(RT_Failure,
						e.what().c_str(),
						"IndexProperty_SetIndexType");
		return RT_Failure;
	} catch (std::exception const& e)
	{
		Error_PushError(RT_Failure,
						e.what(),
						"IndexProperty_SetIndexType");
		return RT_Failure;
	} catch (...) {
		Error_PushError(RT_Failure,
						"Unknown Error",
						"IndexProperty_SetIndexType");
		return RT_Failure;
	}
	return RT_None;
}

SIDX_C_DLL RTIndexType IndexProperty_GetIndexType(IndexPropertyH hProp)
{
	VALIDATE_POINTER1(hProp, "IndexProperty_GetIndexType", RT_InvalidIndexType);
	Tools::PropertySet* prop = static_cast<Tools::PropertySet*>(hProp);

	Tools::Variant var;
	var = prop->getProperty("IndexType");

	if (var.m_varType != Tools::VT_EMPTY)
	{
		if (var.m_varType != Tools::VT_ULONG) {
			Error_PushError(RT_Failure,
							"Property IndexType must be Tools::VT_ULONG",
							"IndexProperty_GetIndexType");
			return RT_InvalidIndexType;
		}
		return (RTIndexType) var.m_val.ulVal;
	}

	Error_PushError(RT_Failure,
					"Property IndexType was empty",
					"IndexProperty_GetIndexType");
	return RT_InvalidIndexType;

}

SIDX_C_DLL RTError IndexProperty_SetDimension(IndexPropertyH hProp, uint32_t value)
{
	VALIDATE_POINTER1(hProp, "IndexProperty_SetDimension", RT_Failure);
	Tools::PropertySet* prop = static_cast<Tools::PropertySet*>(hProp);

	try
	{
		Tools::Variant var;
		var.m_varType = Tools::VT_ULONG;
		var.m_val.ulVal = value;
		prop->setProperty("Dimension", var);
	} catch (Tools::Exception& e)
	{
		Error_PushError(RT_Failure,
						e.what().c_str(),
						"IndexProperty_SetDimension");
		return RT_Failure;
	} catch (std::exception const& e)
	{
		Error_PushError(RT_Failure,
						e.what(),
						"IndexProperty_SetDimension");
		return RT_Failure;
	} catch (...) {
		Error_PushError(RT_Failure,
						"Unknown Error",
						"IndexProperty_SetDimension");
		return RT_Failure;
	}
	return RT_None;
}

SIDX_C_DLL uint32_t IndexProperty_GetDimension(IndexPropertyH hProp)
{
	VALIDATE_POINTER1(hProp, "IndexProperty_GetDimension", 0);
	Tools::PropertySet* prop = static_cast<Tools::PropertySet*>(hProp);

	Tools::Variant var;
	var = prop->getProperty("Dimension");

	if (var.m_varType != Tools::VT_EMPTY)
	{
		if (var.m_varType != Tools::VT_ULONG) {
			Error_PushError(RT_Failure,
							"Property IndexType must be Tools::VT_ULONG",
							"IndexProperty_GetDimension");
			return 0;
		}

		return var.m_val.ulVal;
	}

	// A zero dimension index is invalid.
	Error_PushError(RT_Failure,
					"Property Dimension was empty",
					"IndexProperty_GetDimension");
	return 0;
}

SIDX_C_DLL RTError IndexProperty_SetIndexVariant( IndexPropertyH hProp,
												RTIndexVariant value)
{
	using namespace SpatialIndex;

	VALIDATE_POINTER1(hProp, "IndexProperty_SetIndexVariant", RT_Failure);
	Tools::PropertySet* prop = static_cast<Tools::PropertySet*>(hProp);

	Tools::Variant var;

	try
	{

		if (!(value == RT_Linear || value == RT_Quadratic || value == RT_Star)) {
			throw std::runtime_error("Inputted value is not a valid index variant");
		}

		var.m_varType = Tools::VT_LONG;
		RTIndexType type = IndexProperty_GetIndexType(hProp);
		if (type == RT_InvalidIndexType ) {
			Error_PushError(RT_Failure,
							"Index type is not properly set",
							"IndexProperty_SetIndexVariant");
			return RT_Failure;
		}
		if (type == RT_RTree) {
			var.m_val.lVal = static_cast<RTree::RTreeVariant>(value);
			prop->setProperty("TreeVariant", var);
		}

	} catch (Tools::Exception& e)
	{
		Error_PushError(RT_Failure,
						e.what().c_str(),
						"IndexProperty_SetIndexVariant");
		return RT_Failure;
	} catch (std::exception const& e)
	{
		Error_PushError(RT_Failure,
						e.what(),
						"IndexProperty_SetIndexCapacity");
		return RT_Failure;
	} catch (...) {
		Error_PushError(RT_Failure,
						"Unknown Error",
						"IndexProperty_SetIndexCapacity");
		return RT_Failure;
	}
	return RT_None;
}

SIDX_C_DLL RTIndexVariant IndexProperty_GetIndexVariant(IndexPropertyH hProp)
{
	VALIDATE_POINTER1(	hProp,
						"IndexProperty_GetIndexVariant",
						RT_InvalidIndexVariant);

	Tools::PropertySet* prop = static_cast<Tools::PropertySet*>(hProp);

	Tools::Variant var;
	var = prop->getProperty("TreeVariant");


	if (var.m_varType != Tools::VT_EMPTY)
	{
		if (var.m_varType != Tools::VT_LONG) {
			Error_PushError(RT_Failure,
							"Property IndexVariant must be Tools::VT_LONG",
							"IndexProperty_GetIndexVariant");
			return RT_InvalidIndexVariant;
		}

		return static_cast<RTIndexVariant>(var.m_val.lVal);
	}

	// if we didn't get anything, we're returning an error condition
	Error_PushError(RT_Failure,
					"Property IndexVariant was empty",
					"IndexProperty_GetIndexVariant");
	return RT_InvalidIndexVariant;

}

SIDX_C_DLL RTError IndexProperty_SetIndexStorage( IndexPropertyH hProp,
												RTStorageType value)
{
	VALIDATE_POINTER1(hProp, "IndexProperty_SetIndexStorage", RT_Failure);
	Tools::PropertySet* prop = static_cast<Tools::PropertySet*>(hProp);

	try
	{
		if (!(value == RT_Disk || value == RT_Memory || value == RT_Custom)) {
			throw std::runtime_error("Inputted value is not a valid index storage type");
		}
		Tools::Variant var;
		var.m_varType = Tools::VT_ULONG;
		var.m_val.ulVal = value;
		prop->setProperty("IndexStorageType", var);
	} catch (Tools::Exception& e)
	{
		Error_PushError(RT_Failure,
						e.what().c_str(),
						"IndexProperty_SetIndexStorage");
		return RT_Failure;
	} catch (std::exception const& e)
	{
		Error_PushError(RT_Failure,
						e.what(),
						"IndexProperty_SetIndexStorage");
		return RT_Failure;
	} catch (...) {
		Error_PushError(RT_Failure,
						"Unknown Error",
						"IndexProperty_SetIndexStorage");
		return RT_Failure;
	}
	return RT_None;
}

SIDX_C_DLL RTStorageType IndexProperty_GetIndexStorage(IndexPropertyH hProp)
{
	VALIDATE_POINTER1(	hProp,
						"IndexProperty_GetIndexStorage",
						RT_InvalidStorageType);

	Tools::PropertySet* prop = static_cast<Tools::PropertySet*>(hProp);

	Tools::Variant var;
	var = prop->getProperty("IndexStorageType");

	if (var.m_varType != Tools::VT_EMPTY)
	{
		if (var.m_varType != Tools::VT_ULONG) {
			Error_PushError(RT_Failure,
							"Property IndexStorage must be Tools::VT_ULONG",
							"IndexProperty_GetIndexStorage");
			return RT_InvalidStorageType;
		}

		return static_cast<RTStorageType>(var.m_val.ulVal);
	}

	// if we didn't get anything, we're returning an error condition
	Error_PushError(RT_Failure,
					"Property IndexStorage was empty",
					"IndexProperty_GetIndexStorage");
	return RT_InvalidStorageType;

}

SIDX_C_DLL RTError IndexProperty_SetIndexCapacity(IndexPropertyH hProp,
												uint32_t value)
{
	VALIDATE_POINTER1(hProp, "IndexProperty_SetIndexCapacity", RT_Failure);
	Tools::PropertySet* prop = static_cast<Tools::PropertySet*>(hProp);

	try
	{
		Tools::Variant var;
		var.m_varType = Tools::VT_ULONG;
		var.m_val.ulVal = value;
		prop->setProperty("IndexCapacity", var);
	} catch (Tools::Exception& e)
	{
		Error_PushError(RT_Failure,
						e.what().c_str(),
						"IndexProperty_SetIndexCapacity");
		return RT_Failure;
	} catch (std::exception const& e)
	{
		Error_PushError(RT_Failure,
						e.what(),
						"IndexProperty_SetIndexCapacity");
		return RT_Failure;
	} catch (...) {
		Error_PushError(RT_Failure,
						"Unknown Error",
						"IndexProperty_SetIndexCapacity");
		return RT_Failure;
	}
	return RT_None;
}

SIDX_C_DLL uint32_t IndexProperty_GetIndexCapacity(IndexPropertyH hProp)
{
	VALIDATE_POINTER1(hProp, "IndexProperty_GetIndexCapacity", 0);
	Tools::PropertySet* prop = static_cast<Tools::PropertySet*>(hProp);

	Tools::Variant var;
	var = prop->getProperty("IndexCapacity");

	if (var.m_varType != Tools::VT_EMPTY)
	{
		if (var.m_varType != Tools::VT_ULONG) {
			Error_PushError(RT_Failure,
							"Property IndexCapacity must be Tools::VT_ULONG",
							"IndexProperty_GetIndexCapacity");
			return 0;
		}

		return var.m_val.ulVal;
	}

	// return nothing for an error
	Error_PushError(RT_Failure,
					"Property IndexCapacity was empty",
					"IndexProperty_GetIndexCapacity");
	return 0;
}

SIDX_C_DLL RTError IndexProperty_SetLeafCapacity( IndexPropertyH hProp,
												uint32_t value)
{
	VALIDATE_POINTER1(hProp, "IndexProperty_SetLeafCapacity", RT_Failure);
	Tools::PropertySet* prop = static_cast<Tools::PropertySet*>(hProp);

	try
	{
		Tools::Variant var;
		var.m_varType = Tools::VT_ULONG;
		var.m_val.ulVal = value;
		prop->setProperty("LeafCapacity", var);
	} catch (Tools::Exception& e)
	{
		Error_PushError(RT_Failure,
						e.what().c_str(),
						"IndexProperty_SetLeafCapacity");
		return RT_Failure;
	} catch (std::exception const& e)
	{
		Error_PushError(RT_Failure,
						e.what(),
						"IndexProperty_SetLeafCapacity");
		return RT_Failure;
	} catch (...) {
		Error_PushError(RT_Failure,
						"Unknown Error",
						"IndexProperty_SetLeafCapacity");
		return RT_Failure;
	}
	return RT_None;
}

SIDX_C_DLL uint32_t IndexProperty_GetLeafCapacity(IndexPropertyH hProp)
{
	VALIDATE_POINTER1(hProp, "IndexProperty_GetLeafCapacity", 0);
	Tools::PropertySet* prop = static_cast<Tools::PropertySet*>(hProp);

	Tools::Variant var;
	var = prop->getProperty("LeafCapacity");

	if (var.m_varType != Tools::VT_EMPTY)
	{
		if (var.m_varType != Tools::VT_ULONG) {
			Error_PushError(RT_Failure,
							"Property LeafCapacity must be Tools::VT_ULONG",
							"IndexProperty_GetLeafCapacity");
			return 0;
		}

		return var.m_val.ulVal;
	}

	// return nothing for an error
	Error_PushError(RT_Failure,
					"Property LeafCapacity was empty",
					"IndexProperty_GetLeafCapacity");
	return 0;
}

SIDX_C_DLL RTError IndexProperty_SetPagesize( IndexPropertyH hProp,
											uint32_t value)
{
	VALIDATE_POINTER1(hProp, "IndexProperty_SetPagesize", RT_Failure);
	Tools::PropertySet* prop = static_cast<Tools::PropertySet*>(hProp);

	try
	{
		Tools::Variant var;
		var.m_varType = Tools::VT_ULONG;
		var.m_val.ulVal = value;
		prop->setProperty("PageSize", var);
	} catch (Tools::Exception& e)
	{
		Error_PushError(RT_Failure,
						e.what().c_str(),
						"IndexProperty_SetPagesize");
		return RT_Failure;
	} catch (std::exception const& e)
	{
		Error_PushError(RT_Failure,
						e.what(),
						"IndexProperty_SetPagesize");
		return RT_Failure;
	} catch (...) {
		Error_PushError(RT_Failure,
						"Unknown Error",
						"IndexProperty_SetPagesize");
		return RT_Failure;
	}
	return RT_None;
}

SIDX_C_DLL uint32_t IndexProperty_GetPagesize(IndexPropertyH hProp)
{
	VALIDATE_POINTER1(hProp, "IndexProperty_GetPagesize", 0);
	Tools::PropertySet* prop = static_cast<Tools::PropertySet*>(hProp);

	Tools::Variant var;
	var = prop->getProperty("PageSize");

	if (var.m_varType != Tools::VT_EMPTY)
	{
		if (var.m_varType != Tools::VT_ULONG) {
			Error_PushError(RT_Failure,
							"Property PageSize must be Tools::VT_ULONG",
							"IndexProperty_GetPagesize");
			return 0;
		}

		return var.m_val.ulVal;
	}

	// return nothing for an error
	Error_PushError(RT_Failure,
					"Property PageSize was empty",
					"IndexProperty_GetPagesize");
	return 0;
}

SIDX_C_DLL RTError IndexProperty_SetLeafPoolCapacity( IndexPropertyH hProp,
													uint32_t value)
{
	VALIDATE_POINTER1(hProp, "IndexProperty_SetLeafPoolCapacity", RT_Failure);
	Tools::PropertySet* prop = static_cast<Tools::PropertySet*>(hProp);

	try
	{
		Tools::Variant var;
		var.m_varType = Tools::VT_ULONG;
		var.m_val.ulVal = value;
		prop->setProperty("LeafPoolCapacity", var);
	} catch (Tools::Exception& e)
	{
		Error_PushError(RT_Failure,
						e.what().c_str(),
						"IndexProperty_SetLeafPoolCapacity");
		return RT_Failure;
	} catch (std::exception const& e)
	{
		Error_PushError(RT_Failure,
						e.what(),
						"IndexProperty_SetLeafPoolCapacity");
		return RT_Failure;
	} catch (...) {
		Error_PushError(RT_Failure,
						"Unknown Error",
						"IndexProperty_SetLeafPoolCapacity");
		return RT_Failure;
	}
	return RT_None;
}

SIDX_C_DLL uint32_t IndexProperty_GetLeafPoolCapacity(IndexPropertyH hProp)
{
	VALIDATE_POINTER1(hProp, "IndexProperty_GetLeafPoolCapacity", 0);
	Tools::PropertySet* prop = static_cast<Tools::PropertySet*>(hProp);

	Tools::Variant var;
	var = prop->getProperty("LeafPoolCapacity");

	if (var.m_varType != Tools::VT_EMPTY)
	{
		if (var.m_varType != Tools::VT_ULONG) {
			Error_PushError(RT_Failure,
							"Property LeafPoolCapacity must be Tools::VT_ULONG",
							"IndexProperty_GetLeafPoolCapacity");
			return 0;
		}

		return var.m_val.ulVal;
	}

	// return nothing for an error
	Error_PushError(RT_Failure,
					"Property LeafPoolCapacity was empty",
					"IndexProperty_GetLeafPoolCapacity");
	return 0;
}

SIDX_C_DLL RTError IndexProperty_SetIndexPoolCapacity(IndexPropertyH hProp,
													uint32_t value)
{
	VALIDATE_POINTER1(hProp, "IndexProperty_SetIndexPoolCapacity", RT_Failure);
	Tools::PropertySet* prop = static_cast<Tools::PropertySet*>(hProp);

	try
	{
		Tools::Variant var;
		var.m_varType = Tools::VT_ULONG;
		var.m_val.ulVal = value;
		prop->setProperty("IndexPoolCapacity", var);
	} catch (Tools::Exception& e)
	{
		Error_PushError(RT_Failure,
						e.what().c_str(),
						"IndexProperty_SetIndexPoolCapacity");
		return RT_Failure;
	} catch (std::exception const& e)
	{
		Error_PushError(RT_Failure,
						e.what(),
						"IndexProperty_SetIndexPoolCapacity");
		return RT_Failure;
	} catch (...) {
		Error_PushError(RT_Failure,
						"Unknown Error",
						"IndexProperty_SetIndexPoolCapacity");
		return RT_Failure;
	}
	return RT_None;
}

SIDX_C_DLL uint32_t IndexProperty_GetIndexPoolCapacity(IndexPropertyH hProp)
{
	VALIDATE_POINTER1(hProp, "IndexProperty_GetIndexPoolCapacity", 0);
	Tools::PropertySet* prop = static_cast<Tools::PropertySet*>(hProp);

	Tools::Variant var;
	var = prop->getProperty("IndexPoolCapacity");

	if (var.m_varType != Tools::VT_EMPTY)
	{
		if (var.m_varType != Tools::VT_ULONG) {
			Error_PushError(RT_Failure,
							"Property IndexPoolCapacity must be Tools::VT_ULONG",
							"IndexProperty_GetIndexPoolCapacity");
			return 0;
		}

		return var.m_val.ulVal;
	}

	// return nothing for an error
	Error_PushError(RT_Failure,
					"Property IndexPoolCapacity was empty",
					"IndexProperty_GetIndexPoolCapacity");
	return 0;
}

SIDX_C_DLL RTError IndexProperty_SetRegionPoolCapacity(IndexPropertyH hProp,
													uint32_t value)
{
	VALIDATE_POINTER1(hProp, "IndexProperty_SetRegionPoolCapacity", RT_Failure);
	Tools::PropertySet* prop = static_cast<Tools::PropertySet*>(hProp);

	try
	{
		Tools::Variant var;
		var.m_varType = Tools::VT_ULONG;
		var.m_val.ulVal = value;
		prop->setProperty("RegionPoolCapacity", var);
	} catch (Tools::Exception& e)
	{
		Error_PushError(RT_Failure,
						e.what().c_str(),
						"IndexProperty_SetRegionPoolCapacity");
		return RT_Failure;
	} catch (std::exception const& e)
	{
		Error_PushError(RT_Failure,
						e.what(),
						"IndexProperty_SetRegionPoolCapacity");
		return RT_Failure;
	} catch (...) {
		Error_PushError(RT_Failure,
						"Unknown Error",
						"IndexProperty_SetRegionPoolCapacity");
		return RT_Failure;
	}
	return RT_None;
}

SIDX_C_DLL uint32_t IndexProperty_GetRegionPoolCapacity(IndexPropertyH hProp)
{
	VALIDATE_POINTER1(hProp, "IndexProperty_GetRegionPoolCapacity", 0);
	Tools::PropertySet* prop = static_cast<Tools::PropertySet*>(hProp);

	Tools::Variant var;
	var = prop->getProperty("RegionPoolCapacity");

	if (var.m_varType != Tools::VT_EMPTY)
	{
		if (var.m_varType != Tools::VT_ULONG) {
			Error_PushError(RT_Failure,
							"Property RegionPoolCapacity must be Tools::VT_ULONG",
							"IndexProperty_GetRegionPoolCapacity");
			return 0;
		}

		return var.m_val.ulVal;
	}

	// return nothing for an error
	Error_PushError(RT_Failure,
					"Property RegionPoolCapacity was empty",
					"IndexProperty_GetRegionPoolCapacity");
	return 0;
}

SIDX_C_DLL RTError IndexProperty_SetPointPoolCapacity(IndexPropertyH hProp,
													uint32_t value)
{
	VALIDATE_POINTER1(hProp, "IndexProperty_SetPointPoolCapacity", RT_Failure);
	Tools::PropertySet* prop = static_cast<Tools::PropertySet*>(hProp);

	try
	{
		Tools::Variant var;
		var.m_varType = Tools::VT_ULONG;
		var.m_val.ulVal = value;
		prop->setProperty("PointPoolCapacity", var);
	} catch (Tools::Exception& e)
	{
		Error_PushError(RT_Failure,
						e.what().c_str(),
						"IndexProperty_SetPointPoolCapacity");
		return RT_Failure;
	} catch (std::exception const& e)
	{
		Error_PushError(RT_Failure,
						e.what(),
						"IndexProperty_SetPointPoolCapacity");
		return RT_Failure;
	} catch (...) {
		Error_PushError(RT_Failure,
						"Unknown Error",
						"IndexProperty_SetPointPoolCapacity");
		return RT_Failure;
	}
	return RT_None;
}

SIDX_C_DLL uint32_t IndexProperty_GetPointPoolCapacity(IndexPropertyH hProp)
{
	VALIDATE_POINTER1(hProp, "IndexProperty_GetPointPoolCapacity", 0);
	Tools::PropertySet* prop = static_cast<Tools::PropertySet*>(hProp);

	Tools::Variant var;
	var = prop->getProperty("PointPoolCapacity");

	if (var.m_varType != Tools::VT_EMPTY)
	{
		if (var.m_varType != Tools::VT_ULONG) {
			Error_PushError(RT_Failure,
							"Property PointPoolCapacity must be Tools::VT_ULONG",
							"IndexProperty_GetPointPoolCapacity");
			return 0;
		}

		return var.m_val.ulVal;
	}

	// return nothing for an error
	Error_PushError(RT_Failure,
					"Property PointPoolCapacity was empty",
					"IndexProperty_GetPointPoolCapacity");
	return 0;
}

SIDX_C_DLL RTError IndexProperty_SetNearMinimumOverlapFactor( IndexPropertyH hProp,
															uint32_t value)
{
	VALIDATE_POINTER1(	hProp,
						"IndexProperty_SetNearMinimumOverlapFactor",
						RT_Failure);
	Tools::PropertySet* prop = static_cast<Tools::PropertySet*>(hProp);

	try
	{
		Tools::Variant var;
		var.m_varType = Tools::VT_ULONG;
		var.m_val.ulVal = value;
		prop->setProperty("NearMinimumOverlapFactor", var);
	} catch (Tools::Exception& e)
	{
		Error_PushError(RT_Failure,
						e.what().c_str(),
						"IndexProperty_SetNearMinimumOverlapFactor");
		return RT_Failure;
	} catch (std::exception const& e)
	{
		Error_PushError(RT_Failure,
						e.what(),
						"IndexProperty_SetNearMinimumOverlapFactor");
		return RT_Failure;
	} catch (...) {
		Error_PushError(RT_Failure,
						"Unknown Error",
						"IndexProperty_SetNearMinimumOverlapFactor");
		return RT_Failure;
	}
	return RT_None;
}

SIDX_C_DLL uint32_t IndexProperty_GetNearMinimumOverlapFactor(IndexPropertyH hProp)
{
	VALIDATE_POINTER1(hProp, "IndexProperty_GetNearMinimumOverlapFactor", 0);
	Tools::PropertySet* prop = static_cast<Tools::PropertySet*>(hProp);

	Tools::Variant var;
	var = prop->getProperty("NearMinimumOverlapFactor");

	if (var.m_varType != Tools::VT_EMPTY)
	{
		if (var.m_varType != Tools::VT_ULONG) {
			Error_PushError(RT_Failure,
							"Property NearMinimumOverlapFactor must be Tools::VT_ULONG",
							"IndexProperty_GetNearMinimumOverlapFactor");
			return 0;
		}

		return var.m_val.ulVal;
	}

	// return nothing for an error
	Error_PushError(RT_Failure,
					"Property NearMinimumOverlapFactor was empty",
					"IndexProperty_GetNearMinimumOverlapFactor");
	return 0;
}


SIDX_C_DLL RTError IndexProperty_SetBufferingCapacity(IndexPropertyH hProp,
												uint32_t value)
{
	VALIDATE_POINTER1(hProp, "IndexProperty_SetBufferingCapacity", RT_Failure);
	Tools::PropertySet* prop = static_cast<Tools::PropertySet*>(hProp);

	try
	{
		Tools::Variant var;
		var.m_varType = Tools::VT_ULONG;
		var.m_val.ulVal = value;
		prop->setProperty("Capacity", var);
	} catch (Tools::Exception& e)
	{
		Error_PushError(RT_Failure,
						e.what().c_str(),
						"IndexProperty_SetBufferingCapacity");
		return RT_Failure;
	} catch (std::exception const& e)
	{
		Error_PushError(RT_Failure,
						e.what(),
						"IndexProperty_SetBufferingCapacity");
		return RT_Failure;
	} catch (...) {
		Error_PushError(RT_Failure,
						"Unknown Error",
						"IndexProperty_SetBufferingCapacity");
		return RT_Failure;
	}
	return RT_None;
}

SIDX_C_DLL uint32_t IndexProperty_GetBufferingCapacity(IndexPropertyH hProp)
{
	VALIDATE_POINTER1(hProp, "IndexProperty_GetBufferingCapacity", 0);
	Tools::PropertySet* prop = static_cast<Tools::PropertySet*>(hProp);

	Tools::Variant var;
	var = prop->getProperty("Capacity");

	if (var.m_varType != Tools::VT_EMPTY)
	{
		if (var.m_varType != Tools::VT_ULONG) {
			Error_PushError(RT_Failure,
							"Property Capacity must be Tools::VT_ULONG",
							"IndexProperty_GetBufferingCapacity");
			return 0;
		}

		return var.m_val.ulVal;
	}

	// return nothing for an error
	Error_PushError(RT_Failure,
					"Property Capacity was empty",
					"IndexProperty_GetBufferingCapacity");
	return 0;
}

SIDX_C_DLL RTError IndexProperty_SetEnsureTightMBRs(  IndexPropertyH hProp,
													uint32_t value)
{
	VALIDATE_POINTER1(hProp, "IndexProperty_SetEnsureTightMBRs", RT_Failure);
	Tools::PropertySet* prop = static_cast<Tools::PropertySet*>(hProp);

	try
	{
		if (value > 1 ) {
			Error_PushError(RT_Failure,
							"EnsureTightMBRs is a boolean value and must be 1 or 0",
							"IndexProperty_SetEnsureTightMBRs");
			return RT_Failure;
		}
		Tools::Variant var;
		var.m_varType = Tools::VT_BOOL;
		var.m_val.blVal = value != 0;
		prop->setProperty("EnsureTightMBRs", var);
	} catch (Tools::Exception& e)
	{
		Error_PushError(RT_Failure,
						e.what().c_str(),
						"IndexProperty_SetEnsureTightMBRs");
		return RT_Failure;
	} catch (std::exception const& e)
	{
		Error_PushError(RT_Failure,
						e.what(),
						"IndexProperty_SetEnsureTightMBRs");
		return RT_Failure;
	} catch (...) {
		Error_PushError(RT_Failure,
						"Unknown Error",
						"IndexProperty_SetEnsureTightMBRs");
		return RT_Failure;
	}
	return RT_None;
}

SIDX_C_DLL uint32_t IndexProperty_GetEnsureTightMBRs(IndexPropertyH hProp)
{
	VALIDATE_POINTER1(hProp, "IndexProperty_GetEnsureTightMBRs", 0);
	Tools::PropertySet* prop = static_cast<Tools::PropertySet*>(hProp);

	Tools::Variant var;
	var = prop->getProperty("EnsureTightMBRs");

	if (var.m_varType != Tools::VT_EMPTY)
	{
		if (var.m_varType != Tools::VT_BOOL) {
			Error_PushError(RT_Failure,
							"Property EnsureTightMBRs must be Tools::VT_BOOL",
							"IndexProperty_GetEnsureTightMBRs");
			return 0;
		}

		return var.m_val.blVal;
	}

	// return nothing for an error
	Error_PushError(RT_Failure,
					"Property EnsureTightMBRs was empty",
					"IndexProperty_GetEnsureTightMBRs");
	return 0;
}

SIDX_C_DLL RTError IndexProperty_SetWriteThrough(IndexPropertyH hProp,
													uint32_t value)
{
	VALIDATE_POINTER1(hProp, "IndexProperty_SetWriteThrough", RT_Failure);
	Tools::PropertySet* prop = static_cast<Tools::PropertySet*>(hProp);

	try
	{
		if (value > 1 ) {
			Error_PushError(RT_Failure,
							"WriteThrough is a boolean value and must be 1 or 0",
							"IndexProperty_SetWriteThrough");
			return RT_Failure;
		}
		Tools::Variant var;
		var.m_varType = Tools::VT_BOOL;
		var.m_val.blVal = value != 0;
		prop->setProperty("WriteThrough", var);
	} catch (Tools::Exception& e)
	{
		Error_PushError(RT_Failure,
						e.what().c_str(),
						"IndexProperty_SetWriteThrough");
		return RT_Failure;
	} catch (std::exception const& e)
	{
		Error_PushError(RT_Failure,
						e.what(),
						"IndexProperty_SetWriteThrough");
		return RT_Failure;
	} catch (...) {
		Error_PushError(RT_Failure,
						"Unknown Error",
						"IndexProperty_SetWriteThrough");
		return RT_Failure;
	}
	return RT_None;
}

SIDX_C_DLL uint32_t IndexProperty_GetWriteThrough(IndexPropertyH hProp)
{
	VALIDATE_POINTER1(hProp, "IndexProperty_GetWriteThrough", 0);
	Tools::PropertySet* prop = static_cast<Tools::PropertySet*>(hProp);

	Tools::Variant var;
	var = prop->getProperty("WriteThrough");

	if (var.m_varType != Tools::VT_EMPTY)
	{
		if (var.m_varType != Tools::VT_BOOL) {
			Error_PushError(RT_Failure,
							"Property WriteThrough must be Tools::VT_BOOL",
							"IndexProperty_GetWriteThrough");
			return 0;
		}

		return var.m_val.blVal;
	}

	// return nothing for an error
	Error_PushError(RT_Failure,
					"Property WriteThrough was empty",
					"IndexProperty_GetWriteThrough");
	return 0;
}

SIDX_C_DLL RTError IndexProperty_SetOverwrite(IndexPropertyH hProp,
											uint32_t value)
{
	VALIDATE_POINTER1(hProp, "IndexProperty_SetOverwrite", RT_Failure);
	Tools::PropertySet* prop = static_cast<Tools::PropertySet*>(hProp);

	try
	{
		if (value > 1 ) {
			Error_PushError(RT_Failure,
							"Overwrite is a boolean value and must be 1 or 0",
							"IndexProperty_SetOverwrite");
			return RT_Failure;
		}
		Tools::Variant var;
		var.m_varType = Tools::VT_BOOL;
		var.m_val.blVal = value != 0;
		prop->setProperty("Overwrite", var);
	} catch (Tools::Exception& e)
	{
		Error_PushError(RT_Failure,
						e.what().c_str(),
						"IndexProperty_SetOverwrite");
		return RT_Failure;
	} catch (std::exception const& e)
	{
		Error_PushError(RT_Failure,
						e.what(),
						"IndexProperty_SetOverwrite");
		return RT_Failure;
	} catch (...) {
		Error_PushError(RT_Failure,
						"Unknown Error",
						"IndexProperty_SetOverwrite");
		return RT_Failure;
	}
	return RT_None;
}

SIDX_C_DLL uint32_t IndexProperty_GetOverwrite(IndexPropertyH hProp)
{
	VALIDATE_POINTER1(hProp, "IndexProperty_GetOverwrite", 0);
	Tools::PropertySet* prop = static_cast<Tools::PropertySet*>(hProp);

	Tools::Variant var;
	var = prop->getProperty("Overwrite");

	if (var.m_varType != Tools::VT_EMPTY)
	{
		if (var.m_varType != Tools::VT_BOOL) {
			Error_PushError(RT_Failure,
							"Property Overwrite must be Tools::VT_BOOL",
							"IndexProperty_GetOverwrite");
			return 0;
		}

		return var.m_val.blVal;
	}

	// return nothing for an error
	Error_PushError(RT_Failure,
					"Property Overwrite was empty",
					"IndexProperty_GetOverwrite");
	return 0;
}


SIDX_C_DLL RTError IndexProperty_SetFillFactor(	  IndexPropertyH hProp,
												ts_type value)
{
	VALIDATE_POINTER1(hProp, "IndexProperty_SetFillFactor", RT_Failure);
	Tools::PropertySet* prop = static_cast<Tools::PropertySet*>(hProp);

	try
	{
		Tools::Variant var;
		var.m_varType = Tools::VT_FLOAT;
		var.m_val.dblVal = value;
		prop->setProperty("FillFactor", var);
	} catch (Tools::Exception& e)
	{
		Error_PushError(RT_Failure,
						e.what().c_str(),
						"IndexProperty_SetFillFactor");
		return RT_Failure;
	} catch (std::exception const& e)
	{
		Error_PushError(RT_Failure,
						e.what(),
						"IndexProperty_SetFillFactor");
		return RT_Failure;
	} catch (...) {
		Error_PushError(RT_Failure,
						"Unknown Error",
						"IndexProperty_SetFillFactor");
		return RT_Failure;
	}
	return RT_None;
}

SIDX_C_DLL ts_type IndexProperty_GetFillFactor(IndexPropertyH hProp)
{
	VALIDATE_POINTER1(hProp, "IndexProperty_GetFillFactor", 0);
	Tools::PropertySet* prop = static_cast<Tools::PropertySet*>(hProp);

	Tools::Variant var;
	var = prop->getProperty("FillFactor");

	if (var.m_varType != Tools::VT_EMPTY)
	{
		if (var.m_varType != Tools::VT_FLOAT) {
			Error_PushError(RT_Failure,
							"Property FillFactor must be Tools::VT_FLOAT",
							"IndexProperty_GetFillFactor");
			return 0;
		}

		return var.m_val.dblVal;
	}

	// return nothing for an error
	Error_PushError(RT_Failure,
					"Property FillFactor was empty",
					"IndexProperty_GetFillFactor");
	return 0;
}

SIDX_C_DLL RTError IndexProperty_SetSplitDistributionFactor(  IndexPropertyH hProp,
															ts_type value)
{
	VALIDATE_POINTER1(	hProp,
						"IndexProperty_SetSplitDistributionFactor",
						RT_Failure);
	Tools::PropertySet* prop = static_cast<Tools::PropertySet*>(hProp);

	try
	{
		Tools::Variant var;
		var.m_varType = Tools::VT_FLOAT;
		var.m_val.dblVal = value;
		prop->setProperty("SplitDistributionFactor", var);
	} catch (Tools::Exception& e)
	{
		Error_PushError(RT_Failure,
						e.what().c_str(),
						"IndexProperty_SetSplitDistributionFactor");
		return RT_Failure;
	} catch (std::exception const& e)
	{
		Error_PushError(RT_Failure,
						e.what(),
						"IndexProperty_SetSplitDistributionFactor");
		return RT_Failure;
	} catch (...) {
		Error_PushError(RT_Failure,
						"Unknown Error",
						"IndexProperty_SetSplitDistributionFactor");
		return RT_Failure;
	}
	return RT_None;
}

SIDX_C_DLL ts_type IndexProperty_GetSplitDistributionFactor(IndexPropertyH hProp)
{
	VALIDATE_POINTER1(hProp, "IndexProperty_GetSplitDistributionFactor", 0);
	Tools::PropertySet* prop = static_cast<Tools::PropertySet*>(hProp);

	Tools::Variant var;
	var = prop->getProperty("SplitDistributionFactor");

	if (var.m_varType != Tools::VT_EMPTY)
	{
		if (var.m_varType != Tools::VT_FLOAT) {
			Error_PushError(RT_Failure,
							"Property SplitDistributionFactor must be Tools::VT_FLOAT",
							"IndexProperty_GetSplitDistributionFactor");
			return 0;
		}

		return var.m_val.dblVal;
	}

	// return nothing for an error
	Error_PushError(RT_Failure,
					"Property SplitDistributionFactor was empty",
					"IndexProperty_GetSplitDistributionFactor");
	return 0;
}


SIDX_C_DLL RTError IndexProperty_SetReinsertFactor(	  IndexPropertyH hProp,
													ts_type value)
{
	VALIDATE_POINTER1(	hProp,
						"IndexProperty_SetReinsertFactor",
						RT_Failure);
	Tools::PropertySet* prop = static_cast<Tools::PropertySet*>(hProp);

	try
	{
		Tools::Variant var;
		var.m_varType = Tools::VT_FLOAT;
		var.m_val.dblVal = value;
		prop->setProperty("ReinsertFactor", var);
	} catch (Tools::Exception& e)
	{
		Error_PushError(RT_Failure,
						e.what().c_str(),
						"IndexProperty_SetReinsertFactor");
		return RT_Failure;
	} catch (std::exception const& e)
	{
		Error_PushError(RT_Failure,
						e.what(),
						"IndexProperty_SetReinsertFactor");
		return RT_Failure;
	} catch (...) {
		Error_PushError(RT_Failure,
						"Unknown Error",
						"IndexProperty_SetReinsertFactor");
		return RT_Failure;
	}
	return RT_None;
}

SIDX_C_DLL ts_type IndexProperty_GetReinsertFactor(IndexPropertyH hProp)
{
	VALIDATE_POINTER1(hProp, "IndexProperty_GetReinsertFactor", 0);
	Tools::PropertySet* prop = static_cast<Tools::PropertySet*>(hProp);

	Tools::Variant var;
	var = prop->getProperty("ReinsertFactor");

	if (var.m_varType != Tools::VT_EMPTY)
	{
		if (var.m_varType != Tools::VT_FLOAT) {
			Error_PushError(RT_Failure,
							"Property ReinsertFactor must be Tools::VT_FLOAT",
							"IndexProperty_GetReinsertFactor");
			return 0;
		}

		return var.m_val.dblVal;
	}

	// return nothing for an error
	Error_PushError(RT_Failure,
					"Property ReinsertFactor was empty",
					"IndexProperty_GetReinsertFactor");
	return 0;
}

SIDX_C_DLL RTError IndexProperty_SetFileName( IndexPropertyH hProp,
											const char* value)
{
	VALIDATE_POINTER1(	hProp,
						"IndexProperty_SetFileName",
						RT_Failure);
	Tools::PropertySet* prop = static_cast<Tools::PropertySet*>(hProp);

	try
	{
		Tools::Variant var;
		var.m_varType = Tools::VT_PCHAR;
		var.m_val.pcVal = STRDUP(value); // not sure if we should copy here
		prop->setProperty("FileName", var);
	} catch (Tools::Exception& e)
	{
		Error_PushError(RT_Failure,
						e.what().c_str(),
						"IndexProperty_SetFileName");
		return RT_Failure;
	} catch (std::exception const& e)
	{
		Error_PushError(RT_Failure,
						e.what(),
						"IndexProperty_SetFileName");
		return RT_Failure;
	} catch (...) {
		Error_PushError(RT_Failure,
						"Unknown Error",
						"IndexProperty_SetFileName");
		return RT_Failure;
	}
	return RT_None;
}

SIDX_C_DLL char* IndexProperty_GetFileName(IndexPropertyH hProp)
{
	VALIDATE_POINTER1(hProp, "IndexProperty_GetFileName", 0);
	Tools::PropertySet* prop = static_cast<Tools::PropertySet*>(hProp);

	Tools::Variant var;
	var = prop->getProperty("FileName");

	if (var.m_varType != Tools::VT_EMPTY)
	{
		if (var.m_varType != Tools::VT_PCHAR) {
			Error_PushError(RT_Failure,
							"Property FileName must be Tools::VT_PCHAR",
							"IndexProperty_GetFileName");
			return NULL;
		}

		return STRDUP(var.m_val.pcVal);
	}

	// return nothing for an error
	Error_PushError(RT_Failure,
					"Property FileName was empty",
					"IndexProperty_GetFileName");
	return NULL;
}


SIDX_C_DLL RTError IndexProperty_SetFileNameExtensionDat( IndexPropertyH hProp,
														const char* value)
{
	VALIDATE_POINTER1(	hProp,
						"IndexProperty_SetFileNameExtensionDat",
						RT_Failure);
	Tools::PropertySet* prop = static_cast<Tools::PropertySet*>(hProp);

	try
	{
		Tools::Variant var;
		var.m_varType = Tools::VT_PCHAR;
		var.m_val.pcVal = STRDUP(value); // not sure if we should copy here
		prop->setProperty("FileNameDat", var);

	} catch (Tools::Exception& e)
	{
		Error_PushError(RT_Failure,
						e.what().c_str(),
						"IndexProperty_SetFileNameExtensionDat");
		return RT_Failure;
	} catch (std::exception const& e)
	{
		Error_PushError(RT_Failure,
						e.what(),
						"IndexProperty_SetFileNameExtensionDat");
		return RT_Failure;
	} catch (...) {
		Error_PushError(RT_Failure,
						"Unknown Error",
						"IndexProperty_SetFileNameExtensionDat");
		return RT_Failure;
	}
	return RT_None;
}

SIDX_C_DLL char* IndexProperty_GetFileNameExtensionDat(IndexPropertyH hProp)
{
	VALIDATE_POINTER1(hProp, "IndexProperty_GetFileNameExtensionDat", 0);
	Tools::PropertySet* prop = static_cast<Tools::PropertySet*>(hProp);

	Tools::Variant var;
	var = prop->getProperty("FileNameDat");

	if (var.m_varType != Tools::VT_EMPTY)
	{
		if (var.m_varType != Tools::VT_PCHAR) {
			Error_PushError(RT_Failure,
							"Property FileNameDat must be Tools::VT_PCHAR",
							"IndexProperty_GetFileNameExtensionDat");
			return NULL;
		}

		return STRDUP(var.m_val.pcVal);
	}

	// return nothing for an error
	Error_PushError(RT_Failure,
					"Property FileNameDat was empty",
					"IndexProperty_GetFileNameExtensionDat");
	return NULL;
}

SIDX_C_DLL RTError IndexProperty_SetFileNameExtensionIdx( IndexPropertyH hProp,
														const char* value)
{
	VALIDATE_POINTER1(	hProp,
						"IndexProperty_SetFileNameExtensionIdx",
						RT_Failure);
	Tools::PropertySet* prop = static_cast<Tools::PropertySet*>(hProp);

	try
	{
		Tools::Variant var;
		var.m_varType = Tools::VT_PCHAR;
		var.m_val.pcVal = STRDUP(value); // not sure if we should copy here
		prop->setProperty("FileNameIdx", var);

	} catch (Tools::Exception& e)
	{
		Error_PushError(RT_Failure,
						e.what().c_str(),
						"IndexProperty_SetFileNameExtensionIdx");
		return RT_Failure;
	} catch (std::exception const& e)
	{
		Error_PushError(RT_Failure,
						e.what(),
						"IndexProperty_SetFileNameExtensionIdx");
		return RT_Failure;
	} catch (...) {
		Error_PushError(RT_Failure,
						"Unknown Error",
						"IndexProperty_SetFileNameExtensionIdx");
		return RT_Failure;
	}
	return RT_None;
}

SIDX_C_DLL char* IndexProperty_GetFileNameExtensionIdx(IndexPropertyH hProp)
{
	VALIDATE_POINTER1(hProp, "IndexProperty_GetFileNameExtensionIdx", 0);
	Tools::PropertySet* prop = static_cast<Tools::PropertySet*>(hProp);

	Tools::Variant var;
	var = prop->getProperty("FileNameIdx");

	if (var.m_varType != Tools::VT_EMPTY)
	{
		if (var.m_varType != Tools::VT_PCHAR) {
			Error_PushError(RT_Failure,
							"Property FileNameIdx must be Tools::VT_PCHAR",
							"IndexProperty_GetFileNameExtensionIdx");
			return NULL;
		}

		return STRDUP(var.m_val.pcVal);
	}

	// return nothing for an error
	Error_PushError(RT_Failure,
					"Property FileNameIdx was empty",
					"IndexProperty_GetFileNameExtensionIdx");
	return NULL;
}

SIDX_C_DLL RTError IndexProperty_SetCustomStorageCallbacksSize(IndexPropertyH hProp,
												uint32_t value)
{
	VALIDATE_POINTER1(hProp, "IndexProperty_SetCustomStorageCallbacksSize", RT_Failure);
	Tools::PropertySet* prop = static_cast<Tools::PropertySet*>(hProp);

	try
	{
		Tools::Variant var;
		var.m_varType = Tools::VT_ULONG;
		var.m_val.ulVal = value;
		prop->setProperty("CustomStorageCallbacksSize", var);
	} catch (Tools::Exception& e)
	{
		Error_PushError(RT_Failure,
						e.what().c_str(),
						"IndexProperty_SetCustomStorageCallbacksSize");
		return RT_Failure;
	} catch (std::exception const& e)
	{
		Error_PushError(RT_Failure,
						e.what(),
						"IndexProperty_SetCustomStorageCallbacksSize");
		return RT_Failure;
	} catch (...) {
		Error_PushError(RT_Failure,
						"Unknown Error",
						"IndexProperty_SetCustomStorageCallbacksSize");
		return RT_Failure;
	}
	return RT_None;
}

SIDX_C_DLL uint32_t IndexProperty_GetCustomStorageCallbacksSize(IndexPropertyH hProp)
{
	VALIDATE_POINTER1(hProp, "IndexProperty_GetCustomStorageCallbacksSize", 0);
	Tools::PropertySet* prop = static_cast<Tools::PropertySet*>(hProp);

	Tools::Variant var;
	var = prop->getProperty("CustomStorageCallbacksSize");

	if (var.m_varType != Tools::VT_EMPTY)
	{
		if (var.m_varType != Tools::VT_ULONG) {
			Error_PushError(RT_Failure,
							"Property CustomStorageCallbacksSize must be Tools::VT_ULONG",
							"IndexProperty_GetCustomStorageCallbacksSize");
			return 0;
		}

		return var.m_val.ulVal;
	}

	// return nothing for an error
	Error_PushError(RT_Failure,
					"Property CustomStorageCallbacksSize was empty",
					"IndexProperty_GetCustomStorageCallbacksSize");
	return 0;
}

SIDX_C_DLL RTError IndexProperty_SetCustomStorageCallbacks( IndexPropertyH hProp,
														const void* value)
{
	VALIDATE_POINTER1(	hProp,
						"IndexProperty_SetCustomStorageCallbacks",
						RT_Failure);
	Tools::PropertySet* prop = static_cast<Tools::PropertySet*>(hProp);

    // check if the CustomStorageCallbacksSize is alright, so we can make a copy of the passed in structure
  	Tools::Variant varSize;
    varSize = prop->getProperty("CustomStorageCallbacksSize");
    if ( varSize.m_val.ulVal != sizeof(SpatialIndex::StorageManager::CustomStorageManagerCallbacks) )
    {
        std::ostringstream ss;
        ss << "The supplied storage callbacks size is wrong, expected "
            << sizeof(SpatialIndex::StorageManager::CustomStorageManagerCallbacks)
           << ", got " << varSize.m_val.ulVal;
		Error_PushError(RT_Failure,
						ss.str().c_str(),
						"IndexProperty_SetCustomStorageCallbacks");
		return RT_Failure;
    }

    try
	{
		Tools::Variant var;
		var.m_varType = Tools::VT_PVOID;
        var.m_val.pvVal = value ?
                            new SpatialIndex::StorageManager::CustomStorageManagerCallbacks(
                                    *static_cast<const SpatialIndex::StorageManager::CustomStorageManagerCallbacks*>(value)
                                    )
                            : 0;
		prop->setProperty("CustomStorageCallbacks", var);

	} catch (Tools::Exception& e)
	{
		Error_PushError(RT_Failure,
						e.what().c_str(),
						"IndexProperty_SetCustomStorageCallbacks");
		return RT_Failure;
	} catch (std::exception const& e)
	{
		Error_PushError(RT_Failure,
						e.what(),
						"IndexProperty_SetCustomStorageCallbacks");
		return RT_Failure;
	} catch (...) {
		Error_PushError(RT_Failure,
						"Unknown Error",
						"IndexProperty_SetCustomStorageCallbacks");
		return RT_Failure;
	}
	return RT_None;
}

SIDX_C_DLL void* IndexProperty_GetCustomStorageCallbacks(IndexPropertyH hProp)
{
	VALIDATE_POINTER1(hProp, "IndexProperty_GetCustomStorageCallbacks", 0);
	Tools::PropertySet* prop = static_cast<Tools::PropertySet*>(hProp);

	Tools::Variant var;
	var = prop->getProperty("CustomStorageCallbacks");

	if (var.m_varType != Tools::VT_EMPTY)
	{
		if (var.m_varType != Tools::VT_PVOID) {
			Error_PushError(RT_Failure,
							"Property CustomStorageCallbacks must be Tools::VT_PVOID",
							"IndexProperty_GetCustomStorageCallbacks");
			return NULL;
		}

		return var.m_val.pvVal;
	}

	// return nothing for an error
	Error_PushError(RT_Failure,
					"Property CustomStorageCallbacks was empty",
					"IndexProperty_GetCustomStorageCallbacks");
	return NULL;
}

SIDX_C_DLL RTError IndexProperty_SetIndexID(IndexPropertyH hProp,
											int64_t value)
{
	VALIDATE_POINTER1(hProp, "IndexProperty_SetIndexID", RT_Failure);
	Tools::PropertySet* prop = static_cast<Tools::PropertySet*>(hProp);

	try
	{
		Tools::Variant var;
		var.m_varType = Tools::VT_LONGLONG;
		var.m_val.llVal = value;
		prop->setProperty("IndexIdentifier", var);
	} catch (Tools::Exception& e)
	{
		Error_PushError(RT_Failure,
						e.what().c_str(),
						"IndexProperty_SetIndexID");
		return RT_Failure;
	} catch (std::exception const& e)
	{
		Error_PushError(RT_Failure,
						e.what(),
						"IndexProperty_SetIndexID");
		return RT_Failure;
	} catch (...) {
		Error_PushError(RT_Failure,
						"Unknown Error",
						"IndexProperty_SetIndexID");
		return RT_Failure;
	}
	return RT_None;
}

SIDX_C_DLL int64_t IndexProperty_GetIndexID(IndexPropertyH hProp)
{
	VALIDATE_POINTER1(hProp, "IndexProperty_GetIndexID", 0);
	Tools::PropertySet* prop = static_cast<Tools::PropertySet*>(hProp);

	Tools::Variant var;
	var = prop->getProperty("IndexIdentifier");

	if (var.m_varType != Tools::VT_EMPTY)
	{
		if (var.m_varType != Tools::VT_LONGLONG) {
			Error_PushError(RT_Failure,
							"Property IndexIdentifier must be Tools::VT_LONGLONG",
							"IndexProperty_GetIndexID");
			return 0;
		}

		return var.m_val.llVal;
	}

	// return nothing for an error
	Error_PushError(RT_Failure,
					"Property IndexIdentifier was empty",
					"IndexProperty_GetIndexID");
	return 0;
}

SIDX_C_DLL void* SIDX_NewBuffer(size_t length)
{
    return new char[length];
}

SIDX_DLL RTError IndexProperty_SetResultSetLimit(IndexPropertyH hProp, int64_t value)
{
	VALIDATE_POINTER1(hProp, "IndexProperty_SetResultSetLimit", RT_Failure);

	Tools::PropertySet* prop = static_cast<Tools::PropertySet*>(hProp);

	try
	{
		Tools::Variant var;
		var.m_varType = Tools::VT_LONGLONG;
		var.m_val.llVal = value;
		prop->setProperty("ResultSetLimit", var);
	} catch (Tools::Exception& e)
	{
		Error_PushError(RT_Failure,
						e.what().c_str(),
						"IndexProperty_SetResultSetLimit");
		return RT_Failure;
	} catch (std::exception const& e)
	{
		Error_PushError(RT_Failure,
						e.what(),
						"IndexProperty_SetResultSetLimit");
		return RT_Failure;
	} catch (...) {
		Error_PushError(RT_Failure,
						"Unknown Error",
						"IndexProperty_SetResultSetLimit");
		return RT_Failure;
	}
	return RT_None;
}

SIDX_DLL int64_t IndexProperty_GetResultSetLimit(IndexPropertyH hProp)
{
	VALIDATE_POINTER1(hProp, "IndexProperty_GetResultSetLimit", 0);
	Tools::PropertySet* prop = static_cast<Tools::PropertySet*>(hProp);

	Tools::Variant var;
	var = prop->getProperty("ResultSetLimit");

	if (var.m_varType != Tools::VT_EMPTY)
	{
		if (var.m_varType != Tools::VT_LONGLONG) {
			Error_PushError(RT_Failure,
							"Property ResultSetLimit must be Tools::VT_LONGLONG",
							"IndexProperty_GetResultSetLimit");
			return 0;
		}

		return var.m_val.llVal;
	}

	// return nothing for an error
	Error_PushError(RT_Failure,
					"Property ResultSetLimit was empty",
					"IndexProperty_GetResultSetLimit");
	return 0;
}


SIDX_C_DLL void SIDX_DeleteBuffer(void* buffer)
{
    delete [] static_cast<char*>(buffer);
}


SIDX_C_DLL char* SIDX_Version()
{

	std::ostringstream ot;

#ifdef SIDX_RELEASE_NAME
	ot << SIDX_RELEASE_NAME;
#else
	ot << "1.3.2";
#endif

	std::string out(ot.str());
	return STRDUP(out.c_str());

}
IDX_C_END

#ifdef _WIN32
#  pragma warning(pop)
#endif

	
