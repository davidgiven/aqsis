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
		\brief Declares classes for the subdivision surface GPrim.
		\author Paul C. Gregory (pgregory@aqsis.com)
*/

//? Is subdivision.h included already?
#ifndef SUBDIVISION_H_INCLUDED
#define SUBDIVISION_H_INCLUDED 1

#include <stdio.h>

#include "ri.h"
#include "vector3d.h"
#include "surface.h"
#include "polygon.h"
#include "ishaderdata.h"
#include "memorypool.h"

#define		_qShareName	CORE
#include	"share.h"

START_NAMESPACE( Aqsis )


class CqSubdivider;
class CqWSurf;
class CqWEdge;
class CqWFace;
class CqWVert;

//----------------------------------------
/** \class CqWReference
 * Smart reference to an edge, has an associated face, and takes this into account when performing
 * actions which rely on head or tail being the correct way round.
 */

class CqWReference
{
	public:
		/** Default constructor.
		 * \param pEdge Pointer to a CqWEdge class.
		 * \param pFace Pointer to the asociated CqWFace used to determine edge direction.
		 */
		CqWReference( CqWEdge* pEdge = 0, CqWFace* pFace = 0 )
		{
			Reset( pEdge, pFace );
		}
		~CqWReference()
		{}

		void	Reset( CqWEdge* pE, CqWFace* pF );

		/** Get a pointer to the edge.
		 */
		CqWEdge*	peCurrent()
		{
			return ( m_pEdge );
		}
		CqWReference&	peNext();
		CqWReference&	pePrev();
		CqWEdge*	peHeadRight();
		CqWEdge*	peHeadLeft();
		CqWEdge*	peTailRight();
		CqWEdge*	peTailLeft();

		CqWVert*	pvHead();
		CqWVert*	pvTail();

		CqWEdge*	peHeadHalf();
		CqWEdge*	peTailHalf();

		/** Get ap ointer to the face to the left of this edge.
		 * \return A CqWFace pointer to the associated face.
		 */
		CqWFace*	pfLeft()
		{
			return ( m_pFace );
		}
		CqWFace*	pfRight();

		void	SetpfTailLeft( CqWFace* pfTailLeft );
		void	SetpfHeadLeft( CqWFace* pfHeadLeft );
		void	SetpfTailRight( CqWFace* pfTailRight );
		void	SetpfHeadRight( CqWFace* pfHeadRight );

		void	SetpeTailTailLeft( CqWEdge* pe );
		void	SetpeTailHeadLeft( CqWEdge* pe );
		void	SetpeTailTailRight( CqWEdge* pe );
		void	SetpeTailHeadRight( CqWEdge* pe );
		void	SetpeHeadTailLeft( CqWEdge* pe );
		void	SetpeHeadHeadLeft( CqWEdge* pe );
		void	SetpeHeadTailRight( CqWEdge* pe );
		void	SetpeHeadHeadRight( CqWEdge* pe );

	private:
		TqBool	m_bLeft;		///< Flag indicating whether the associated face lies to the left or right.
		CqWEdge*	m_pEdge;		///< Pointer to the associated edge.
		CqWFace*	m_pFace;		///< Pointer to the associated face.
}
;


//----------------------------------------
/** \class CqWVert
 * Winged-Edge vertex structure.
 */

class CqWVert : public CqPoolable<CqWVert>
{
	public:
		CqWVert( TqInt iV ) : m_iVertex( iV )
		{}
		~CqWVert()
		{}

		/** Get the index into the vertices array of this vertex.
		 */
		TqInt	iVertex()
		{
			return ( m_iVertex );
		}
		/** Set the index into the vertices array of this vertex.
		 */
		void	SetiVertex( TqInt iV )
		{
			m_iVertex = iV;
		}

		/** Get a count of edges emanating from this vertex.
		 */
		TqInt	cEdges()
		{
			return ( m_apEdges.size() );
		}
		/** Get a reference to the array of edge pointers emanating from this vertex.
		 */
		std::vector<CqWEdge*>& lEdges()
		{
			return ( m_apEdges );
		}
		/** Get a pointer to an indexed edge from the list.
		 * \param iE Integer index.
		 */
		CqWEdge*	pEdge( TqInt iE )
		{
			return ( m_apEdges[ iE ] );
		}

		/** Add a new edge to the list of edges emanating from this vertex.
		 * \param pE Pointer to a CqWEdge class to add.
		 */
		void	AddEdge( CqWEdge* pE )
		{
			m_apEdges.push_back( pE );
		}
		void	RemoveEdge( CqWEdge* pE );

		/** Templatised function to perform the subdivision arithmetic on a paramter type.
		 * \param t Temp of the template type to overcome the VC++ problem with template functions.
		 * \param F A pointer to a function to get indexed values of the appropriate type.
		 * \param pSurf Pointer to the CqWSurf on which we are working. 
		 */
		template <class T>
		T GetSmoothedScalar( T& t, T( CqSubdivider::*F ) ( CqPolygonPoints*, TqInt ), CqSubdivider* pSurf, CqPolygonPoints* pPoints )
		{
			// NOTE: Checks should have been made prior to this call to ensure it is neither
			// a boundary point or a crease/corner point with sharp edges.

			// Q the average of the face points surrounding the vertex.
			T Q = T( 0.0f );
			std::vector<CqWEdge*>::iterator iE;
			CqWFace* pF;
			TqInt cE = 0;

			for ( iE = m_apEdges.begin(); iE != m_apEdges.end(); iE++ )
			{
				if ( ( *iE ) ->pvHead() == this ) pF = ( *iE ) ->pfLeft();
				else	pF = ( *iE ) ->pfRight();
				if ( pF )
				{
					Q += ( pSurf->*F ) ( pPoints, pF->pvSubdivide() ->iVertex() );
					cE++;
				}
			}
			Q *= ( 1.0f / ( cE * cE ) );

			// R average of the midpoints of the edges that share the old vertex.
			T R = T( 0.0f );
			cE = 0;
			for ( iE = m_apEdges.begin(); iE != m_apEdges.end(); iE++ )
			{
				if ( ( *iE ) ->IsValid() )
				{
					if ( ( *iE ) ->pvHead() == this ) R += ( pSurf->*F ) ( pPoints, ( *iE ) ->pvTail() ->iVertex() );
					else	R += ( pSurf->*F ) ( pPoints, ( *iE ) ->pvHead() ->iVertex() );
					cE++;
				}
			}
			R *= ( 1.0f / ( cE * cE ) );

			T S = ( pSurf->*F ) ( pPoints, iVertex() ) * ( ( cE - 2.0f ) / ( TqFloat ) cE );
			return ( S + R + Q );
		}

		/** Templatised function to perform the subdivision arithmetic on a paramter type.
		 * \param t Temp of the template type to overcome the VC++ problem with template functions.
		 * \param F A pointer to a function to get indexed values of the appropriate type.
		 * \param pSurf Pointer to the CqWSurf on which we are working. 
		 */
		template <class T>
		T GetCreaseScalar( T& t, T( CqSubdivider::*F ) ( CqPolygonPoints*, TqInt ), CqSubdivider* pSurf, CqPolygonPoints* pPoints )
		{
			T P = T( 0.0f );
			std::vector<CqWEdge*>::iterator iE;
			TqFloat S = 0.0;
			TqInt cS = 0;

			for ( iE = m_apEdges.begin(); iE != m_apEdges.end(); iE++ )
			{
				if ( ( *iE ) ->Sharpness() > 0 && ( *iE ) ->IsValid() )
				{
					if ( ( *iE ) ->pvHead() == this ) P += ( pSurf->*F ) ( pPoints, ( *iE ) ->pvTail() ->iVertex() );
					else	P += ( pSurf->*F ) ( pPoints, ( *iE ) ->pvHead() ->iVertex() );
					S += ( *iE ) ->Sharpness();
					cS++;
				}
			}
			P += ( pSurf->*F ) ( pPoints, iVertex() ) * 6.0f;
			P /= 8.0f;				// Crease point

			S /= ( TqFloat ) cS;
			if ( cS == 2 && S > 0.0f && S < 1.0f )
			{
				T P2;
				P2 = GetSmoothedScalar( P2, F, pSurf, pPoints );
				P = ( P2 * ( 1.0f - S ) ) + ( P * S );	// Linear blend for variable crease.
			}
			return ( P );
		}

		/** Templatised function to perform the subdivision arithmetic on a paramter type.
		 * \param t Temp of the template type to overcome the VC++ problem with template functions.
		 * \param F A pointer to a function to get indexed values of the appropriate type.
		 * \param pSurf Pointer to the CqWSurf on which we are working. 
		 */
		template <class T>
		T GetBoundaryScalar( T& t, T( CqSubdivider::*F ) ( CqPolygonPoints*, TqInt ), CqSubdivider* pSurf, CqPolygonPoints* pPoints )
		{
			T P = T( 0.0f );
			std::vector<CqWEdge*>::iterator iE;
			for ( iE = m_apEdges.begin(); iE != m_apEdges.end(); iE++ )
				if ( ( *iE ) ->IsBoundary() )
					if ( ( *iE ) ->pvHead() == this ) P += ( pSurf->*F ) ( pPoints, ( *iE ) ->pvTail() ->iVertex() );
					else	P += ( pSurf->*F ) ( pPoints, ( *iE ) ->pvHead() ->iVertex() );

			P += ( pSurf->*F ) ( pPoints, iVertex() ) * 6.0f;
			P /= 8.0f;
			return ( P );
		}

	private:
		TqInt	m_iVertex;				///< Index of the vertex in the vertex list
		std::vector<CqWEdge*> m_apEdges;	///< Array of shared edge indexes
}
;


class CqWEdge;

//----------------------------------------
/** \class CqWFace
 * Winged-Edge face structure.
 */

class CqWFace : public CqPoolable<CqWFace>
{
	public:
		CqWFace() : m_pvSubdivide( 0 )
		{}


		/** Get a pointer to the indexed edge surrounding this face.
		 */
		CqWEdge*	pEdge( TqInt iE )
		{
			return ( m_apEdges[ iE ] );
		}
		/** Get a pointer to the indexed edge surrounding this face.
		 */
		const CqWEdge*	pEdge( TqInt iE ) const
		{
			return ( m_apEdges[ iE ] );
		}
		/** Get The count of edges making up this face,
		 */
		TqInt	cEdges() const
		{
			return ( m_apEdges.size() );
		}
		/** Add a CqWEdge pointer to the array of edges surrounding this face.
		 */
		void	AddEdge( CqWEdge* pE )
		{
			m_apEdges.push_back( pE );
		}
		/** Common case, add four edges at once,
		 */
		void	SetQuad( CqWEdge* pE0, CqWEdge* pE1, CqWEdge* pE2, CqWEdge* pE3 )
		{
			m_apEdges.resize( 4 );
			m_apEdges[ 0 ] = pE0;
			m_apEdges[ 1 ] = pE1;
			m_apEdges[ 2 ] = pE2;
			m_apEdges[ 3 ] = pE3;
		}
		/** Get a pointer to the calculated midpoint of this face.
		 * \return CqWVert pointer.
		 * \attention Must have called CreateSubdividePoint first.
		 */
		CqWVert*	pvSubdivide()
		{
			return ( m_pvSubdivide );
		}

		CqWVert*	CreateSubdividePoint( CqSubdivider* pSurf, CqPolygonPoints* pPoints, CqWVert* pV, TqBool uses_s, TqBool uses_t, TqBool uses_Cs, TqBool uses_Os,
		                               TqBool has_s, TqBool has_t, TqBool has_Cs, TqBool has_Os );

		/** Templatised function to perform the subdivision arithmetic on a paramter type.
		 * \param t Temp of the template type to overcome the VC++ problem with template functions.
		 * \param F A pointer to a function to get indexed values of the appropriate type.
		 * \param pSurf Pointer to the CqWSurf on which we are working. 
		 */
		template <class T>
		T	CreateSubdivideScalar( T& t, T( CqSubdivider::*F ) ( CqPolygonPoints*, TqInt ), CqSubdivider* pSurf, CqPolygonPoints* pPoints )
		{
			T P = T( 0.0f );

			CqWReference grE( m_apEdges[ 0 ], this );

			for ( TqUint j = 0; j < m_apEdges.size(); j++ )
			{
				P += ( pSurf->*F ) ( pPoints, grE.pvHead() ->iVertex() );
				grE.peNext();
			}
			P /= ( TqFloat ) m_apEdges.size();

			return ( P );
		}

	private:
		std::vector<CqWEdge*> m_apEdges;	///< Array of shared edge indexes
		CqWVert*	m_pvSubdivide;			///< Pointer to the calculated midpoint, set by CreateSubdividePoint.
}
;


//----------------------------------------
/** \class CqWEdge
 * Winged-Edge edge structure.
 */

class CqWEdge : public CqPoolable<CqWEdge>
{
	public:
		CqWEdge() :
				m_pvHead( 0 ), m_pvTail( 0 ),
				m_peHeadRight( 0 ), m_peHeadLeft( 0 ),
				m_peTailRight( 0 ), m_peTailLeft( 0 ),
				m_pfLeft( 0 ), m_pfRight( 0 ),
				m_peHeadHalf( 0 ),
				m_peTailHalf( 0 ),
				m_pvSubdivide( 0 ), m_Sharpness( 0 ), m_cFaces( 0 )
		{}
		CqWEdge( CqWVert* pH, CqWVert* pT ) :
				m_pvHead( pH ), m_pvTail( pT ),
				m_peHeadRight( 0 ), m_peHeadLeft( 0 ),
				m_peTailRight( 0 ), m_peTailLeft( 0 ),
				m_pfLeft( 0 ), m_pfRight( 0 ),
				m_peHeadHalf( 0 ),
				m_peTailHalf( 0 ),
				m_pvSubdivide( 0 ), m_Sharpness( 0 ), m_cFaces( 0 )
		{}

		/** Get a pointer to the head vertex of the edge.
		 * \return A CqWVert Pointer.
		 */
		CqWVert*	pvHead()
		{
			return ( m_pvHead );
		}
		/** Get a pointer to the tail vertex of the edge.
		 * \return A CqWVert Pointer.
		 */
		CqWVert*	pvTail()
		{
			return ( m_pvTail );
		}
		/** Get a pointer to the opposite vertex to the specified one.
		 * \param pA A CqWVert pointer, should be one of the head or tail vertices.
		 * \return A CqWVert Pointer.
		 */
		CqWVert*	pvOpposite( CqWVert* pA )
		{
			return ( pA == m_pvHead ? m_pvTail : m_pvHead );
		}
		/** Get a pointer to the edge to the right of the head vertex.
		 * \return A CqWEdge Pointer.
		 */
		CqWEdge*	peHeadRight()
		{
			return ( m_peHeadRight );
		}
		/** Get a pointer to the edge to the left of the head vertex.
		 * \return A CqWEdge Pointer.
		 */
		CqWEdge*	peHeadLeft()
		{
			return ( m_peHeadLeft );
		}
		/** Get a pointer to the edge to the right of the tail vertex.
		 * \return A CqWEdge Pointer.
		 */
		CqWEdge*	peTailRight()
		{
			return ( m_peTailRight );
		}
		/** Get a pointer to the edge to the left of the tail vertex.
		 * \return A CqWEdge Pointer.
		 */
		CqWEdge*	peTailLeft()
		{
			return ( m_peTailLeft );
		}
		/** Get a pointer to the face to the left of this edge.
		 * \return A CqWEdge Pointer.
		 */
		CqWFace*	pfLeft()
		{
			return ( m_pfLeft );
		}
		/** Get a pointer to the face to the right of this edge.
		 * \return A CqWFace Pointer.
		 */
		CqWFace*	pfRight()
		{
			return ( m_pfRight );
		}
		/** Get a pointer to the edge which is the subdivided head half.
		 * \return A CqWFace Pointer.
		 * \attention Must have called Subdivide first.
		 */
		CqWEdge*	peHeadHalf()
		{
			return ( m_peHeadHalf );
		}
		/** Get a pointer to the edge which is the subdivided tail half.
		 * \return A CqWEdge Pointer.
		 * \attention Must have called Subdivide first.
		 */
		CqWEdge*	peTailHalf()
		{
			return ( m_peTailHalf );
		}



		/** Get a pointer to the head vertex of the edge.
		 * \return A CqWVert Pointer.
		 */
		const CqWVert*	pvHead() const
		{
			return ( m_pvHead );
		}
		/** Get a pointer to the tail vertex of the edge.
		 * \return A CqWVert Pointer.
		 */
		const CqWVert*	pvTail() const
		{
			return ( m_pvTail );
		}
		/** Get a pointer to the opposite vertex to the specified one.
		 * \param pA A CqWVert pointer, should be one of the head or tail vertices.
		 * \return A CqWVert Pointer.
		 */
		const CqWVert*	pvOpposite( CqWVert* pA ) const
		{
			return ( pA == m_pvHead ? m_pvTail : m_pvHead );
		}
		/** Get a pointer to the edge to the right of the head vertex.
		 * \return A CqWEdge Pointer.
		 */
		const CqWEdge*	peHeadRight() const
		{
			return ( m_peHeadRight );
		}
		/** Get a pointer to the edge to the left of the head vertex.
		 * \return A CqWEdge Pointer.
		 */
		const CqWEdge*	peHeadLeft() const
		{
			return ( m_peHeadLeft );
		}
		/** Get a pointer to the edge to the right of the tail vertex.
		 * \return A CqWEdge Pointer.
		 */
		const CqWEdge*	peTailRight() const
		{
			return ( m_peTailRight );
		}
		/** Get a pointer to the edge to the left of the tail vertex.
		 * \return A CqWEdge Pointer.
		 */
		const CqWEdge*	peTailLeft() const
		{
			return ( m_peTailLeft );
		}
		/** Get a pointer to the face to the left of this edge.
		 * \return A CqWEdge Pointer.
		 */
		const CqWFace*	pfLeft() const
		{
			return ( m_pfLeft );
		}
		/** Get a pointer to the face to the right of this edge.
		 * \return A CqWFace Pointer.
		 */
		const CqWFace*	pfRight() const
		{
			return ( m_pfRight );
		}
		/** Get a pointer to the edge which is the subdivided head half.
		 * \return A CqWFace Pointer.
		 * \attention Must have called Subdivide first.
		 */
		const CqWEdge*	peHeadHalf() const
		{
			return ( m_peHeadHalf );
		}
		/** Get a pointer to the edge which is the subdivided tail half.
		 * \return A CqWEdge Pointer.
		 * \attention Must have called Subdivide first.
		 */
		const CqWEdge*	peTailHalf() const
		{
			return ( m_peTailHalf );
		}

		/** Determine if this edge is completely specified, ie has two associated faces.
		 * If this returns true and another face is specified, the surface is non-manifold.
		 */
		TqBool	bComplete()
		{
			return ( m_cFaces == 2 );
		}
		/** Get the sharpness of this edge.
		 * \return Float sharpness setting.
		 */
		TqFloat	Sharpness()
		{
			return ( m_Sharpness );
		}
		/** Increment the number of faces referencing this edge, used to determin validity of the surface.
		 */
		void	IncrFaces()
		{
			m_cFaces++;
		}
		/** Get the number of faces referencing this edge, used to determin validity of the surface.
		 */
		TqInt	cFaces() const
		{
			return ( m_cFaces );
		}

		/** Set the pointer to the head vertex of the edge.
		 * \param pvHead A CqWVert Pointer.
		 */
		void	SetpvHead( CqWVert* pvHead )
		{
			m_pvHead = pvHead;
		}
		/** Set the pointer to the tail vertex of the edge.
		 * \param pvHead A CqWVert Pointer.
		 */
		void	SetpvTail( CqWVert* pvTail )
		{
			m_pvTail = pvTail;
		}
		/** Set the pointer to the edge to the right of the head vertex.
		 * \param peHeadRight A CqWEdge Pointer.
		 */
		void	SetpeHeadRight( CqWEdge* peHeadRight )
		{
			m_peHeadRight = peHeadRight;
		}
		/** Set the pointer to the edge to the left of the head vertex.
		 * \param peHeadLeft A CqWEdge Pointer.
		 */
		void	SetpeHeadLeft( CqWEdge* peHeadLeft )
		{
			m_peHeadLeft = peHeadLeft;
		}
		/** Set the pointer to the edge to the right of the tail vertex.
		 * \param peTailRight A CqWEdge Pointer.
		 */
		void	SetpeTailRight( CqWEdge* peTailRight )
		{
			m_peTailRight = peTailRight;
		}
		/** Set the pointer to the edge to the left of the tail vertex.
		 * \param peTailLeft A CqWEdge Pointer.
		 */
		void	SetpeTailLeft( CqWEdge* peTailLeft )
		{
			m_peTailLeft = peTailLeft;
		}
		/** Set the pointer to the face to the left of this edge.
		 * \param pfLeft A CqWFace Pointer.
		 */
		void	SetpfLeft( CqWFace* pfLeft )
		{
			m_pfLeft = pfLeft;
		}
		/** Set the pointer to the face to the right of this edge.
		 * \param pfRight A CqWFace Pointer.
		 */
		void	SetpfRight( CqWFace* pfRight )
		{
			m_pfRight = pfRight;
		}

		/** Set the sharpness of this edge.
		 * \param Sharpness Float sharpness value.
		 */
		void	SetSharpness( TqFloat Sharpness )
		{
			m_Sharpness = Sharpness;
		}

		TqBool	IsBoundary();
		TqBool	IsValid();
		CqWVert*	CreateSubdividePoint( CqSubdivider* pSurf, CqPolygonPoints* pPoints, CqWVert* pV, TqBool uses_s, TqBool uses_t, TqBool uses_Cs, TqBool uses_Os,
		                               TqBool has_s, TqBool has_t, TqBool has_Cs, TqBool has_Os );
		void	Subdivide( CqSubdivider* pSurf );
		/** Get a pointer to the calculated midpoint of this edge.
		 * \return A CqWVert pointer.
		 * \attention Must have called CreateSubdividePoint first.
		 */
		CqWVert*	pvSubdivide()
		{
			return ( m_pvSubdivide );
		}

		/** Templatised function to perform the subdivision arithmetic on a paramter type.
		 * \param t Temp of the template type to overcome the VC++ problem with template functions.
		 * \param F A pointer to a function to get indexed values of the appropriate type.
		 * \param pSurf Pointer to the CqWSurf on which we are working. 
		 */
		template <class T>
		T	CreateSubdivideScalar( T& t, T( CqSubdivider::*F ) ( CqPolygonPoints*, TqInt ), CqSubdivider* pSurf, CqPolygonPoints* pPoints )
		{
			T P = ( pSurf->*F ) ( pPoints, pvHead() ->iVertex() );
			P += ( pSurf->*F ) ( pPoints, pvTail() ->iVertex() );

			// Check for sharp edges.
			if ( IsBoundary() == TqTrue || m_Sharpness > 0 )  		// Boundary check.
			{
				if ( m_Sharpness < 1 && IsBoundary() == TqFalse )  	// Infinitely sharp?
				{
					T P2 = P;
					P *= 0.5;
					P2 += ( pSurf->*F ) ( pPoints, pfLeft() ->pvSubdivide() ->iVertex() );
					P2 += ( pSurf->*F ) ( pPoints, pfRight() ->pvSubdivide() ->iVertex() );
					P2 *= 0.25;
					P = ( P2 * ( 1.0f - m_Sharpness ) ) + ( P * m_Sharpness );
				}
				else
					P *= 0.5f;								// Sharp edges are just midpoint
			}
			else										// Smooth.
			{
				P += ( pSurf->*F ) ( pPoints, pfLeft() ->pvSubdivide() ->iVertex() );
				P += ( pSurf->*F ) ( pPoints, pfRight() ->pvSubdivide() ->iVertex() );
				P *= 0.25;
			}
			return ( P );
		}

	private:
		CqWVert	*m_pvHead,  				///< Pointer to the head vertex.
		*m_pvTail;				///< Pointer to the tail vertex.
		CqWEdge	*m_peHeadRight,  			///< Pointer to the edge to the right of the head vertex.
		*m_peHeadLeft;			///< Pointer to the edge to the left of the head vertex.
		CqWEdge	*m_peTailRight,  			///< Pointer to the edge to the right of the tail vertex.
		*m_peTailLeft;			///< Pointer to the edge to the left of the tail vertex.
		CqWFace	*m_pfLeft,  				///< Pointer to the face to the left of the edge.
		*m_pfRight;				///< Pointer to the face to the right of the edge.
		CqWEdge	*m_peHeadHalf;			///< Pointer to the subdivided edge a the head end, set by Subdivide.
		CqWEdge	*m_peTailHalf;			///< Pointer to the subdivided edge a the tail end, set by Subdivide.

		CqWVert	*m_pvSubdivide;			///< Pointer to the subdivided midpoint vertex, set by CreateSubdividePoint.

		TqFloat	m_Sharpness;			///< Float sharpness.
		TqInt	m_cFaces;				///< Internal count of faces using this edge, used to determine validity of edge.
}
;



//----------------------------------------
/** \class CqSubdivider
 * Base class with core subdivision functionality.
 */

class CqSubdivider
{
	public:
		CqSubdivider() :
				m_fSubdivided( false ),
				m_bInterpolateBoundary( false )
		{}
		CqSubdivider( TqInt cExpectedVertices, TqInt cExpectedFaces ) :
				m_cExpectedVertices( cExpectedVertices ),
				m_cExpectedFaces( cExpectedFaces ),
				m_fSubdivided( false ),
				m_bInterpolateBoundary( false )
		{}
		virtual	~CqSubdivider();

		/** Get a pointer to the indexed vertex in the list.
		 * \param iV Integer index.
		 * \return A CqWVert pointer.
		 */
		CqWVert*	pVert( TqInt iV )
		{
			return ( m_apVerts[ iV ] );
		}
		/** Get a pointer to the indexed edge in the list.
		 * \param iV Integer index.
		 * \return A CqWEdge pointer.
		 */
		CqWEdge*	pEdge( TqInt iE )
		{
			return ( m_apEdges[ iE ] );
		}
		/** Get a pointer to the indexed face in the list.
		 * \param iV Integer index.
		 * \return A CqWFace pointer.
		 */
		CqWFace*	pFace( TqInt iF )
		{
			return ( m_apFaces[ iF ] );
		}
		/** Get a pointer to the indexed vertex in the list.
		 * \param iV Integer index.
		 * \return A CqWVert pointer.
		 */
		const CqWVert*	pVert( TqInt iV ) const
		{
			return ( m_apVerts[ iV ] );
		}
		/** Get a pointer to the indexed edge in the list.
		 * \param iV Integer index.
		 * \return A CqWEdge pointer.
		 */
		const CqWEdge*	pEdge( TqInt iE ) const
		{
			return ( m_apEdges[ iE ] );
		}
		/** Get a pointer to the indexed face in the list.
		 * \param iV Integer index.
		 * \return A CqWFace pointer.
		 */
		const CqWFace*	pFace( TqInt iF ) const
		{
			return ( m_apFaces[ iF ] );
		}
		/** Get the count of edges in this surface.
		 */
		TqInt	cEdges() const
		{
			return ( m_apEdges.size() );
		}
		/** Get the count of faces in this surface.
		 */
		TqInt	cFaces() const
		{
			return ( m_apFaces.size() );
		}
		/** Get the count of vertices in this surface.
		 */
		TqInt	cVerts() const
		{
			return ( m_apVerts.size() );
		}

		CqWEdge*	FindEdge( CqWEdge* pE );
		CqWVert*	FindVertex( CqPolygonPoints* pPoints, const CqVector3D& V );

		CqWEdge*	AddEdge( CqWVert* pA, CqWVert* pB );
		/** Add an edge to the list.
		 * \param pE Pointer to a CqWEdge to add.
		 */
		void	AddEdge( CqWEdge* pE )
		{
			m_apEdges.push_back( pE );
		}
		/** Add a vertex to the list.
		 * \param pV Pointer to a CqWVert to add.
		 */
		void	AddVert( CqWVert* pV )
		{
			m_apVerts.push_back( pV );
		}
		CqWFace*	AddFace( CqWEdge** pE, TqInt cE );
		/** Add a face to the list.
		 * \param pF Pointer to a CqWFace to add.
		 */
		void	AddFace( CqWFace* pF )
		{
			m_apFaces.push_back( pF );
		}

		/** Get an indexed P value from the specified points storage class.
		 */
		CqVector3D	SubdP( CqPolygonPoints* pPoints, TqInt index )
		{
			assert( static_cast<TqUint>( index ) < pPoints->P().Size() );
			return ( pPoints->P() [ index ] );
		}
		/** Get an indexed s value from the specified points storage class.
		 */
		TqFloat	Subds( CqPolygonPoints* pPoints, TqInt index )
		{
			assert( static_cast<TqUint>( index ) < pPoints->s().Size() );
			return ( pPoints->s() [ index ] );
		}
		/** Get an indexed t value from the specified points storage class.
		 */
		TqFloat	Subdt( CqPolygonPoints* pPoints, TqInt index )
		{
			assert( static_cast<TqUint>( index ) < pPoints->t().Size() );
			return ( pPoints->t() [ index ] );
		}
		/** Get an indexed Cs value from the specified points storage class.
		 */
		CqColor	SubdCs( CqPolygonPoints* pPoints, TqInt index )
		{
			assert( static_cast<TqUint>( index ) < pPoints->Cs().Size() );
			return ( pPoints->Cs() [ index ] );
		}
		/** Get an indexed Os value from the specified points storage class.
		 */
		CqColor	SubdOs( CqPolygonPoints* pPoints, TqInt index )
		{
			assert( static_cast<TqUint>( index ) < pPoints->Os().Size() );
			return ( pPoints->Os() [ index ] );
		}

		void	Subdivide();

		void	Subdivide( TqInt count )
		{
			while ( count-- > 0 ) Subdivide();
		}


		void	DiceSubdivide();
		/** Subdivide this patch to a specified level.
		 * \param Count Integer subdivision count.
		 */
		void	DiceSubdivide( TqInt Count )
		{
			while ( Count-- > 0 ) DiceSubdivide();
		}
		/** Set the dice level required to achieve the appropriate shading rate.
		 * \param c Integer dice count.
		 */
		void	SetDiceCount( TqInt c )
		{
			m_DiceCount = c;
		}
		void	StoreDice( TqInt Level, TqInt& iFace, CqPolygonPoints* pPoints,
		                TqInt uOff, TqInt vOff, TqInt cuv, CqMicroPolyGrid* pGrid,
		                TqBool uses_s, TqBool uses_t, TqBool uses_Cs, TqBool uses_Os,
		                TqBool has_s, TqBool has_t, TqBool has_Cs, TqBool has_Os );

		/** Get the flag indicating whethe to interpolate two edge vertices as corners.
		 */
		TqBool	bInterpolateBoundary() const
		{
			return ( m_bInterpolateBoundary );
		}
		/** Set the flag indicating whethe to interpolate two edge vertices as corners.
		 * \param v Boolean flag value.
		 */
		void	InterpolateBoundary( TqBool v = TqTrue )
		{
			m_bInterpolateBoundary = v;
		}

		virtual	TqInt	Uses() const = 0;

		virtual	void	CreateFacePoints( TqInt& iStartIndex, TqBool uses_s, TqBool uses_t, TqBool uses_Cs, TqBool uses_Os, TqBool has_s, TqBool has_t, TqBool has_Cs, TqBool has_Os ) = 0;
		virtual	void	CreateEdgePoints( TqInt& iStartIndex, TqBool uses_s, TqBool uses_t, TqBool uses_Cs, TqBool uses_Os, TqBool has_s, TqBool has_t, TqBool has_Cs, TqBool has_Os ) = 0;
		virtual	void	SmoothVertexPoints( TqInt cOldVerts, TqBool uses_s, TqBool uses_t, TqBool uses_Cs, TqBool uses_Os, TqBool has_s, TqBool has_t, TqBool has_Cs, TqBool has_Os ) = 0;

		/** Determine whether this surface has per vertex normals.
		 */
		virtual	TqBool	bHasN() const = 0;
		/** Determine whether this surface has per vertex colors.
		 */
		virtual	TqBool	bHasCs() const = 0;
		/** Determine whether this surface has per vertex opacities.
		 */
		virtual	TqBool	bHasOs() const = 0;
		/** Determine whether this surface has per vertex s cordinates.
		 */
		virtual	TqBool	bHass() const = 0;
		/** Determine whether this surface has per vertex t coordinates.
		 */
		virtual	TqBool	bHast() const = 0;
		/** Determine whether this surface has per vertex u coordinates.
		 */
		virtual	TqBool	bHasu() const = 0;
		/** Determine whether this surface has per vertex v coordinates.
		 */
		virtual	TqBool	bHasv() const = 0;

	protected:
		std::vector<CqWVert*>	m_apVerts;				///< Array of pointers to winged edge vertex structures.
		std::vector<CqWEdge*>	m_apEdges;				///< Array of pointers to winged edge edge structures.
		std::vector<CqWFace*>	m_apFaces;				///< Array of pointers to winged edge face structures.
		TqInt	m_cExpectedVertices;	///< Number of vertices to be filled in.
		TqInt	m_cExpectedFaces;		///< Number of faces to be filled in.
		TqInt	m_DiceCount;			///< Integer subdivision count to achieve the appropriate shading rate.
		TqBool	m_fSubdivided;
		TqBool	m_bInterpolateBoundary;	///< Flag indicating whether or not to make vertices with only two adjacent edges into corners.
}
;



//----------------------------------------
/** \class CqWSurf
 * Subdivision surface GPrim.
 */

class CqWSurf : public CqSubdivider, public CqBasicSurface
{
	public:
		CqWSurf() : CqSubdivider(), CqBasicSurface(),
				m_pPoints( 0 )
		{}
		CqWSurf( CqPolygonPoints* pVertices ) : CqSubdivider(), CqBasicSurface(),
				m_pPoints( pVertices )
		{}
		CqWSurf( TqInt cExpectedVertices, TqInt cExpectedFaces, CqPolygonPoints* pVertices ) :
				CqSubdivider( cExpectedVertices, cExpectedFaces ),
				CqBasicSurface(),
				m_pPoints( pVertices )
		{}
		CqWSurf( CqWSurf* pSurf, TqInt iFace );
		virtual	~CqWSurf();

		CqPolygonPoints* pPoints()
		{
			return ( m_pPoints );
		}
		void	_OutputMesh( char* pname );
		CqWVert*	TransferVert( CqWSurf* pSurf, TqInt iVert, TqBool uses_s, TqBool uses_t, TqBool uses_Cs, TqBool uses_Os,
		                       TqBool has_s, TqBool has_t, TqBool has_Cs, TqBool has_Os );
		CqWVert*	GetpWVert( CqPolygonPoints* pPoints, const CqVector3D& V );

		// Overridden from CqBasicSurface
		virtual	CqBound	Bound() const;
		virtual	CqMicroPolyGridBase* Dice();
		virtual	TqInt	Split( std::vector<CqBasicSurface*>& aSplits );
		virtual TqBool	Diceable();
		virtual void	Transform( const CqMatrix& matTx, const CqMatrix& matITTx, const CqMatrix& matRTx );
		virtual	TqUint	cUniform() const
		{
			return ( m_cExpectedFaces );
		}
		virtual	TqUint	cVarying() const
		{
			return ( m_cExpectedVertices );
		}
		virtual	TqUint	cVertex() const
		{
			return ( m_cExpectedVertices );
		}

		// Overridden from CqSubdivider
		virtual TqInt	Uses() const
		{
			return ( CqBasicSurface::Uses() );
		}

		virtual	void	CreateFacePoints( TqInt& iStartIndex, TqBool uses_s, TqBool uses_t, TqBool uses_Cs, TqBool uses_Os, TqBool has_s, TqBool has_t, TqBool has_Cs, TqBool has_Os );
		virtual	void	CreateEdgePoints( TqInt& iStartIndex, TqBool uses_s, TqBool uses_t, TqBool uses_Cs, TqBool uses_Os, TqBool has_s, TqBool has_t, TqBool has_Cs, TqBool has_Os );
		virtual	void	SmoothVertexPoints( TqInt cOldVerts, TqBool uses_s, TqBool uses_t, TqBool uses_Cs, TqBool uses_Os, TqBool has_s, TqBool has_t, TqBool has_Cs, TqBool has_Os );

		virtual	TqBool	bHasN() const
		{
			return ( m_pPoints->N().Size() >= cVertex() );
		}
		virtual	TqBool	bHasCs() const
		{
			return ( m_pPoints->Cs().Size() >= cVertex() );
		}
		virtual	TqBool	bHasOs() const
		{
			return ( m_pPoints->Os().Size() >= cVertex() );
		}
		virtual	TqBool	bHass() const
		{
			return ( m_pPoints->s().Size() >= cVertex() );
		}
		virtual	TqBool	bHast() const
		{
			return ( m_pPoints->t().Size() >= cVertex() );
		}
		virtual	TqBool	bHasu() const
		{
			return ( m_pPoints->u().Size() >= cVertex() );
		}
		virtual	TqBool	bHasv() const
		{
			return ( m_pPoints->v().Size() >= cVertex() );
		}

	protected:
		CqPolygonPoints*	m_pPoints;			///> Pointer to the CqSurface class with the surface vertices on.
}
;


//----------------------------------------
/** \class CqMotionWSurf
 * Motion blurrable subdivision surface GPrim.
 */

class CqMotionWSurf : public CqSubdivider, public CqBasicSurface, public CqMotionSpec<CqPolygonPoints*>
{
	public:
		CqMotionWSurf() :
				CqSubdivider(),
				CqMotionSpec<CqPolygonPoints*>( 0 )
		{}
		CqMotionWSurf( CqPolygonPoints* pVertices ) :
				CqSubdivider(),
				CqMotionSpec<CqPolygonPoints*>( pVertices )
		{}
		CqMotionWSurf( TqInt cExpectedVertices, TqInt cExpectedFaces, CqPolygonPoints* pVertices ) :
				CqSubdivider( cExpectedVertices, cExpectedFaces ),
				CqMotionSpec<CqPolygonPoints*>( pVertices )
		{}
		CqMotionWSurf( CqMotionWSurf* pSurf, TqInt iFace );
		virtual	~CqMotionWSurf()
		{}

		CqWVert*	TransferVert( CqMotionWSurf* pSurf, TqInt iVert, TqBool uses_s, TqBool uses_t, TqBool uses_Cs, TqBool uses_Os,
		                       TqBool has_s, TqBool has_t, TqBool has_Cs, TqBool has_Os );
		CqWVert*	GetpWVert( CqPolygonPoints* pPoints, const CqVector3D& V );

		// Overridden from CqBasicSurface
		virtual	CqBound	Bound() const;
		virtual	TqInt	Split( std::vector<CqBasicSurface*>& aSplits );
		virtual	CqMicroPolyGridBase* Dice();
		virtual TqBool	Diceable();
		virtual void	Transform( const CqMatrix& matTx, const CqMatrix& matITTx, const CqMatrix& matRTx )
		{
			TqInt i;
			for ( i = 0; i < cTimes(); i++ )
				GetMotionObject( Time( i ) ) ->Transform( matTx, matITTx, matRTx );
		}
		virtual	TqUint	cUniform() const
		{
			return ( m_cExpectedFaces );
		}
		virtual	TqUint	cVarying() const
		{
			return ( m_cExpectedVertices );
		}
		virtual	TqUint	cVertex() const
		{
			return ( m_cExpectedVertices );
		}

		// Overridden from CqSubdivider
		virtual	TqInt	Uses() const
		{
			return ( CqBasicSurface::Uses() );
		}
		virtual	void	CreateFacePoints( TqInt& iStartIndex, TqBool uses_s, TqBool uses_t, TqBool uses_Cs, TqBool uses_Os, TqBool has_s, TqBool has_t, TqBool has_Cs, TqBool has_Os );
		virtual	void	CreateEdgePoints( TqInt& iStartIndex, TqBool uses_s, TqBool uses_t, TqBool uses_Cs, TqBool uses_Os, TqBool has_s, TqBool has_t, TqBool has_Cs, TqBool has_Os );
		virtual	void	SmoothVertexPoints( TqInt cOldVerts, TqBool uses_s, TqBool uses_t, TqBool uses_Cs, TqBool uses_Os, TqBool has_s, TqBool has_t, TqBool has_Cs, TqBool has_Os );

		virtual	TqBool	bHasN() const
		{
			return ( GetMotionObject( Time( 0 ) ) ->bHasN() );
		}
		virtual	TqBool	bHasCs() const
		{
			return ( GetMotionObject( Time( 0 ) ) ->bHasCs() );
		}
		virtual	TqBool	bHasOs() const
		{
			return ( GetMotionObject( Time( 0 ) ) ->bHasOs() );
		}
		virtual	TqBool	bHass() const
		{
			return ( GetMotionObject( Time( 0 ) ) ->bHass() );
		}
		virtual	TqBool	bHast() const
		{
			return ( GetMotionObject( Time( 0 ) ) ->bHast() );
		}
		virtual	TqBool	bHasu() const
		{
			return ( GetMotionObject( Time( 0 ) ) ->bHasu() );
		}
		virtual	TqBool	bHasv() const
		{
			return ( GetMotionObject( Time( 0 ) ) ->bHasv() );
		}

		// Overridden from CqMotionSpec
		virtual	void	ClearMotionObject( CqPolygonPoints*& A ) const
			{}
		;
		virtual	CqPolygonPoints* ConcatMotionObjects( CqPolygonPoints* const & A, CqPolygonPoints* const & B ) const
		{
			return ( A );
		}
		virtual	CqPolygonPoints* LinearInterpolateMotionObjects( TqFloat Fraction, CqPolygonPoints* const & A, CqPolygonPoints* const & B ) const
		{
			return ( A );
		}
};


/** Get a pointer to the face on the right of this edge.
 */
inline CqWFace* CqWReference::pfRight()
{
	return ( m_pFace == m_pEdge->pfLeft() ? m_pEdge->pfRight() : m_pEdge->pfLeft() );
}


//-----------------------------------------------------------------------

END_NAMESPACE( Aqsis )

#endif // SUBDIVISION_H_INCLUDED
