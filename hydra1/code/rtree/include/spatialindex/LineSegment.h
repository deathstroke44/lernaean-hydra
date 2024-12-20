/******************************************************************************
 * Project:  libspatialindex - A C++ library for spatial indexing
 * Author:   Marios Hadjieleftheriou, mhadji@gmail.com
 ******************************************************************************
 * Copyright (c) 2004, Marios Hadjieleftheriou
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

#pragma once

namespace SpatialIndex
{
	class SIDX_DLL LineSegment : public Tools::IObject, public virtual IShape
	{
	public:
		LineSegment();
		LineSegment(const ts_type* startPoint, const ts_type* endPoint, uint32_t dimension);
		LineSegment(const Point& startPoint, const Point& endPoint);
		LineSegment(const LineSegment& l);
		virtual ~LineSegment();

		virtual LineSegment& operator=(const LineSegment& p);
		virtual bool operator==(const LineSegment& p) const;

		//
		// IObject interface
		//
		virtual LineSegment* clone();

		//
		// ISerializable interface
		//
		virtual uint32_t getByteArraySize();
		virtual void loadFromByteArray(const byte* data);
		virtual void storeToByteArray(byte** data, uint32_t& length);

		//
		// IShape interface
		//
		virtual bool intersectsShape(const IShape& in) const;
		virtual bool containsShape(const IShape& in) const;
		virtual bool touchesShape(const IShape& in) const;
		virtual void getCenter(Point& out) const;
		virtual uint32_t getDimension() const;
		virtual void getMBR(Region& out) const;
		virtual ts_type getArea() const;
		virtual ts_type getMinimumDistance(const IShape& in) const;

		virtual bool intersectsLineSegment(const LineSegment& l) const;
		virtual bool intersectsRegion(const Region& p) const;
		virtual ts_type getMinimumDistance(const Point& p) const;
		//virtual ts_type getMinimumDistance(const Region& r) const;
		virtual ts_type getRelativeMinimumDistance(const Point& p) const;
		virtual ts_type getRelativeMaximumDistance(const Region& r) const;
		virtual ts_type getAngleOfPerpendicularRay();

		virtual void makeInfinite(uint32_t dimension);
		virtual void makeDimension(uint32_t dimension);
        
	public:
		uint32_t m_dimension;
		ts_type* m_pStartPoint;
		ts_type* m_pEndPoint;

		friend class Region;
		friend class Point;
		friend SIDX_DLL std::ostream& operator<<(std::ostream& os, const LineSegment& pt);

    protected:

        //some helpers for intersects methods
        static ts_type doubleAreaTriangle(const Point& a, const Point& b, const Point& c); 
        static bool leftOf(const Point& a, const Point& b, const Point& c); 
        static bool collinear(const Point& a, const Point& b, const Point& c); 
        static bool between(const Point& a, const Point& b, const Point& c); 
        static bool between(ts_type a, ts_type b, ts_type c); 
        static bool intersectsProper(const Point& a, const Point& b, const Point& c, const Point& d); 
        static bool intersects(const Point& a, const Point& b, const Point& c, const Point& d); 

	}; // LineSegment

	SIDX_DLL std::ostream& operator<<(std::ostream& os, const LineSegment& pt);
}

