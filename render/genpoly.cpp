// Aqsis
// Copyright � 1997 - 2001, Paul C. Gregory
//
// Contact: pgregory@aqsis.com
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA


/** \file
		\brief Declares a class for handling general polygons with loops.
		\author Paul C. Gregory (pgregory@aqsis.com)
*/

#include	"aqsis.h"
#include	"genpoly.h"

START_NAMESPACE( Aqsis )

//---------------------------------------------------------------------
/** Copy constructor
 */

CqPolygonGeneral2D::CqPolygonGeneral2D( const CqPolygonGeneral2D& From )
{
	*this = From;
}


//---------------------------------------------------------------------
/** Copy operator
 */

CqPolygonGeneral2D& CqPolygonGeneral2D::operator=( const CqPolygonGeneral2D& From )
{
	// Copy the vertices
	TqInt iVertex = From.cVertices();
	m_aiVertices.resize( From.cVertices() );
	while ( iVertex-- > 0 )
		m_aiVertices[ iVertex ] = From.m_aiVertices[ iVertex ];

	m_Orientation = From.m_Orientation;
	m_Axis = From.m_Axis;

	m_pVertices = From.m_pVertices;
	if ( m_pVertices )
		m_pVertices->AddRef();

	return ( *this );
}


//---------------------------------------------------------------------
/** Swap the direction of a polygon.
 */

void CqPolygonGeneral2D::SwapDirection()
{
	for ( TqInt iVertex = 0; iVertex < cVertices() / 2; iVertex++ )
	{
		TqInt	itemp = m_aiVertices[ iVertex ];
		m_aiVertices[ iVertex ] = m_aiVertices[ cVertices() - 1 - iVertex ];
		m_aiVertices[ cVertices() - 1 - iVertex ] = itemp;
	}
	CalcOrientation();
}


//---------------------------------------------------------------------
/** Calculate and store the orientation of this polygon.
 */

TqInt CqPolygonGeneral2D::CalcOrientation()
{
	// Calculate the area of this polygon, and
	// if it is negative the polygon is clockwise.
	TqFloat	Area = ( *this ) [ cVertices() - 1 ].x() * ( *this ) [ 0 ].y() -
	               ( *this ) [ 0 ].x() * ( *this ) [ cVertices() - 1 ].y();

	for ( TqInt iVertex = 0; iVertex < cVertices() - 1; iVertex++ )
		Area += ( *this ) [ iVertex ].x() * ( *this ) [ iVertex + 1 ].y() -
		        ( *this ) [ iVertex + 1 ].x() * ( *this ) [ iVertex ].y();

	if ( Area >= 0.0 )
		m_Orientation = Orientation_AntiClockwise;
	else
		m_Orientation = Orientation_Clockwise;

	return ( m_Orientation );
}


//---------------------------------------------------------------------
/** Calculate the orientation of the triangle formed using the vertices of this
 *  polygon indexed by the 3 provided indices.
 */

TqInt CqPolygonGeneral2D::CalcDeterminant( TqInt i1, TqInt i2, TqInt i3 ) const
{
	assert( i1 >= 0 && i1 <= cVertices() );
	assert( i2 >= 0 && i2 <= cVertices() );
	assert( i3 >= 0 && i3 <= cVertices() );

	// TODO: Look up what a determinant is and therefore, why this works.
	TqFloat	Determ = ( ( *this ) [ i2 ].x() - ( *this ) [ i1 ].x() )
	                 * ( ( *this ) [ i3 ].y() - ( *this ) [ i1 ].y() )
	                 - ( ( *this ) [ i3 ].x() - ( *this ) [ i1 ].x() )
	                 * ( ( *this ) [ i2 ].y() - ( *this ) [ i1 ].y() );

	if ( Determ > 0.0 )
		return ( Orientation_AntiClockwise );
	else
	{
		if ( Determ == 0.0 )
		{
			if ( ( *this ) [ i1 ] == ( *this ) [ i2 ] ||
			        ( *this ) [ i1 ] == ( *this ) [ i3 ] ||
			        ( *this ) [ i2 ] == ( *this ) [ i3 ] )
				return ( Orientation_AntiClockwise );
			else
				return ( Orientation_Clockwise );
		}
		else
			return ( Orientation_Clockwise );
	}
	return ( Orientation_Unknown );
}


//---------------------------------------------------------------------
/** Determine whether any of the vertices indexed by the values in iList are within the
 *  triangle formed by the vertices indexed by i1, i2, and i3
 */

TqBool CqPolygonGeneral2D::NoneInside( TqInt i1, TqInt i2, TqInt i3, std::vector<TqInt>& iList ) const
{
	TqUint iVertex;
	for ( iVertex = 0; iVertex < iList.size(); iVertex++ )
	{
		TqInt iN = iList[ iVertex ];

		// Ignore the vertices which make up the specified triangle
		if ( ( iN == i1 ) || ( iN == i2 ) || ( iN == i3 ) )
			continue;

		// Use the reverse direction of the triangle lines, and if the
		// triangle fromed with the vertex is the same orientation, the
		// vertex lies outside that edge.
		if ( ( CalcDeterminant( i2, i1, iN ) == m_Orientation ) ||
		        ( CalcDeterminant( i1, i3, iN ) == m_Orientation ) ||
		        ( CalcDeterminant( i3, i2, iN ) == m_Orientation ) )
			continue;
		else
		{
			// If it is coincident with one of the vertices, then presume it is inside.
			if ( ( ( *this ) [ iN ] == ( *this ) [ i1 ] ) ||
			        ( ( *this ) [ iN ] == ( *this ) [ i2 ] ) ||
			        ( ( *this ) [ iN ] == ( *this ) [ i3 ] ) )
				continue;
			else
				return ( TqFalse );
		}
	}
	return ( TqTrue );
}


//---------------------------------------------------------------------
/** Check for any duplicate points in the polygon and remove them.
 */

void CqPolygonGeneral2D::EliminateDuplicatePoints()
{}


//---------------------------------------------------------------------
/** Determine whether the specified polygon is entirely within this one.
 *  Uses AntiClockwise as direction.
 */

TqBool CqPolygonGeneral2D::Contains( CqPolygonGeneral2D& polyCheck )
{
	assert( polyCheck.cVertices() > 0 && cVertices() > 0 );

	for ( TqInt iVertex = 0; iVertex < polyCheck.cVertices(); iVertex++ )
	{
		TqInt	c = 0;
		TqFloat	x = polyCheck[ iVertex ].x();
		TqFloat	y = polyCheck[ iVertex ].y();

		// Check if this vertex is inside this polygon.
		TqInt	i, j;
		for ( i = 0, j = cVertices() - 1; i < cVertices(); j = i++ )
		{
			// Check if this edge spans the vertex in y
			if ( ( ( ( ( *this ) [ i ].y() <= y ) && ( y < ( *this ) [ j ].y() ) ) ||
			        ( ( ( *this ) [ j ].y() <= y ) && ( y < ( *this ) [ i ].y() ) ) ) &&
			        // and if so, check the position of the vertex in relation to the edge.
			        ( x < ( ( *this ) [ j ].x() - ( *this ) [ i ].x() ) * ( y - ( *this ) [ i ].y() ) /
			          ( ( *this ) [ j ].y() - ( *this ) [ i ].y() ) + ( *this ) [ i ].x() ) )
				c = !c;
		}
		// If this point is outside, then the polygon cannot be entirely inside.
		if ( !c ) return ( TqFalse );
	}
	return ( TqTrue );
}


//---------------------------------------------------------------------
/** Combine the two polygons.
 *  Determine the two closest points on the two polygons, and then insert the 
 *  new polygon into the list at this point.  Closing the end afterwards.
 */

void CqPolygonGeneral2D::Combine( CqPolygonGeneral2D& polyFrom )
{
	// Go through and find the two points on the polygons
	// which are closest together.

	CqVector2D	currToPrev, currToNext, minToPrev, minToNext;
	TqInt	iMinThis = 0;
	TqInt	iMinThat = 0;
	TqFloat	CurrDist;
	TqFloat	MinDist = FLT_MAX;

	TqInt i;
	for ( i = 0; i < cVertices(); i++ )
	{
		TqInt j;
		for ( j = 0; j < polyFrom.cVertices(); j++ )
		{
			CqVector2D	vecTemp( ( *this ) [ i ] - polyFrom[ j ] );
			CurrDist = static_cast<TqFloat>( sqrt( vecTemp * vecTemp ) );

			if ( CurrDist == MinDist )
			{
				currToPrev = ( i > 0 ) ? ( *this ) [ i - 1 ] - ( *this ) [ i ] :
				             ( *this ) [ cVertices() - 1 ] - ( *this ) [ i ];
				currToNext = ( i < cVertices() - 1 ) ? ( *this ) [ i + 1 ] - ( *this ) [ i ] :
				             ( *this ) [ 0 ] - ( *this ) [ i ];

				minToPrev = ( iMinThis > 0 ) ? ( *this ) [ iMinThis - 1 ] - ( *this ) [ iMinThis ] :
				            ( *this ) [ cVertices() - 1 ] - ( *this ) [ iMinThis ];
				minToNext = ( iMinThis < cVertices() - 1 ) ? ( *this ) [ iMinThis + 1 ] - ( *this ) [ iMinThis ] :
				            ( *this ) [ 0 ] - ( *this ) [ iMinThis ];

				CqVector2D	vecTest = polyFrom[ j ] - ( *this ) [ i ];

				currToPrev.Unit();
				currToNext.Unit();
				minToPrev.Unit();
				minToNext.Unit();

				vecTemp = currToPrev - vecTest;
				TqFloat	distCP = static_cast<TqFloat>( sqrt( vecTemp * vecTemp ) );
				vecTemp = currToNext - vecTest;
				TqFloat	distCN = static_cast<TqFloat>( sqrt( vecTemp * vecTemp ) );
				vecTemp = minToPrev - vecTest;
				TqFloat	distMP = static_cast<TqFloat>( sqrt( vecTemp * vecTemp ) );
				vecTemp = minToNext - vecTest;
				TqFloat	distMN = static_cast<TqFloat>( sqrt( vecTemp * vecTemp ) );

				if ( ( distCP + distCN ) < ( distMP + distMN ) )
				{
					MinDist = CurrDist;
					iMinThis = i;
					iMinThat = j;
				}
			}
			else
			{
				if ( CurrDist < MinDist )
				{
					MinDist = CurrDist;
					iMinThis = i;
					iMinThat = j;
				}
			}
		}
	}
	// Now combine the two polygons at the closest points.

	std::vector<TqInt>	avecNew;

	// First copy the vertices from this one, from the min point up to the end...
	for ( i = iMinThis; i < cVertices(); i++ )
		avecNew.push_back( m_aiVertices[ i ] );

	// ...then copy the vertices from this one, from 0 up to (and including) the min point...
	for ( i = 0; i <= iMinThis; i++ )
		avecNew.push_back( m_aiVertices[ i ] );

	// ...then copy the vertices from that one, from the min point up to the end...
	for ( i = iMinThat; i < polyFrom.cVertices(); i++ )
		avecNew.push_back( polyFrom.m_aiVertices[ i ] );

	// ...then copy the vertices from that one, from 0 up to (and including) the min point...
	for ( i = 0; i <= iMinThat; i++ )
		avecNew.push_back( polyFrom.m_aiVertices[ i ] );

	// Now copy the new list of vertices to this new polygon.
	m_aiVertices.resize( avecNew.size() );
	TqUint ivert;
	for ( ivert = 0; ivert < avecNew.size(); ivert++ )
		m_aiVertices[ ivert ] = avecNew[ ivert ];
}


//---------------------------------------------------------------------
/** Return a list of triangles which cove the surface of this general polygon.
 */

void CqPolygonGeneral2D::Triangulate( std::vector<TqInt>& aiList ) const
{
	// This is done by checking each vertex in turn to see if it can successfully be chopped off.
	// If at the end there are more than 3 vertices left which cannot be chopped off, the
	// polygon is self intersecting.

	std::vector<TqInt>	iList;
	iList.resize( m_aiVertices.size() );
	TqInt iVertex = m_aiVertices.size();
	while ( iVertex-- > 0 )
		iList[ iVertex ] = iVertex;

	TqInt cVertex = m_aiVertices.size();
	while ( cVertex > 3 )
	{
		TqBool	fDone = TqFalse;
		TqInt	iPrev = cVertex - 1;
		TqInt	iCurr = 0;
		TqInt	iNext = 1;

		while ( ( iCurr < cVertex ) && ( fDone == TqFalse ) )
		{
			iPrev = iCurr - 1;
			iNext = iCurr + 1;

			if ( iCurr == 0 )
				iPrev = cVertex - 1;
			else
				if ( iCurr == cVertex - 1 )
					iNext = 0;

			TqInt	CurrDeterm = CalcDeterminant( iList[ iPrev ],
			                                    iList[ iCurr ],
			                                    iList[ iNext ] );
			TqInt	CurrPos = NoneInside( iList[ iPrev ],
			                            iList[ iCurr ],
			                            iList[ iNext ],
			                            iList );
			if ( ( CurrDeterm == Orientation() ) &&
			        ( CurrPos != 0 ) )
				fDone = TqTrue;
			else
				iCurr++;
		}

		if ( fDone == TqFalse )
			return ;
		else
		{
			aiList.push_back( m_aiVertices[ iList[ iPrev ] ] );
			aiList.push_back( m_aiVertices[ iList[ iCurr ] ] );
			aiList.push_back( m_aiVertices[ iList[ iNext ] ] );

			cVertex -= 1;
			for ( iVertex = iCurr; iVertex < cVertex; iVertex++ )
				iList[ iVertex ] = iList[ iVertex + 1 ];
			iList.resize( cVertex );
		}
	}
	aiList.push_back( m_aiVertices[ iList[ 0 ] ] );
	aiList.push_back( m_aiVertices[ iList[ 1 ] ] );
	aiList.push_back( m_aiVertices[ iList[ 2 ] ] );
}


//---------------------------------------------------------------------

END_NAMESPACE( Aqsis )
