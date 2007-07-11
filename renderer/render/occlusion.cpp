// Aqsis
// Copyright � 1997 - 2002, Paul C. Gregory
//
// Contact: pgregory@aqsis.org
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
		\brief Implements the hierarchical occlusion culling class.
		\author Andy Gill (billybobjimboy@users.sf.net)
*/

#if _MSC_VER
#pragma warning(disable : 4786)
#endif

#include "occlusion.h"
#include "bound.h"
#include "bucket.h"
#include <deque>
#include <fstream>
#undef	min
#undef	max

START_NAMESPACE( Aqsis )

//----------------------------------------------------------------------
// Static Variables

bool CqOcclusionTree::CqOcclusionTreeComparator::operator()(const std::pair<TqInt, TqInt>& a, const std::pair<TqInt, TqInt>& b)
{
	const SqSampleData& A = m_bucket->ImageElement(a.first).SampleData(m_bucket->SamplePoints(), a.second);
	const SqSampleData& B = m_bucket->ImageElement(b.first).SampleData(m_bucket->SamplePoints(), b.second);
	return( A.m_Position[m_Dim] < B.m_Position[m_Dim] );
}


TqInt CqOcclusionTree::m_Tab = 0;


CqOcclusionTree::CqOcclusionTree(TqInt dimension)
		: m_Parent(0), m_Dimension(dimension)
{
	TqChildArray::iterator child = m_Children.begin();
	for(; child != m_Children.end(); ++child)
		(*child) = 0;
}

CqOcclusionTree::~CqOcclusionTree()
{
	TqChildArray::iterator child = m_Children.begin();
	for(; child != m_Children.end(); ++child)
	{
		if (*child != NULL)
		{
			delete (*child);
			(*child) = NULL;
		}
	}
}

void CqOcclusionTree::SplitNode(const CqBucket* bucket, CqOcclusionTreePtr& a, CqOcclusionTreePtr& b)
{
	SortElements(bucket, m_Dimension);

	TqInt samplecount = m_SampleIndices.size();
	TqInt median = samplecount / 2;

	// Create the children nodes.
	a = CqOcclusionTreePtr(new CqOcclusionTree());
	b = CqOcclusionTreePtr(new CqOcclusionTree());

	a->m_MinSamplePoint = m_MinSamplePoint;
	b->m_MinSamplePoint = m_MinSamplePoint;
	a->m_MaxSamplePoint = m_MaxSamplePoint;
	b->m_MaxSamplePoint = m_MaxSamplePoint;
	TqInt newdim = ( m_Dimension + 1 ) % Dimensions();
	a->m_Dimension = b->m_Dimension = newdim;

	TqFloat dividingplane = Sample(bucket, median).m_Position[m_Dimension];

	a->m_MaxSamplePoint[m_Dimension] = dividingplane;
	b->m_MinSamplePoint[m_Dimension] = dividingplane;

	TqFloat minTime = m_MaxTime, maxTime = m_MinTime;
	TqInt minDofIndex = m_MaxDofBoundIndex, maxDofIndex = m_MinDofBoundIndex;
	TqFloat minDetailLevel = m_MaxDetailLevel, maxDetailLevel = m_MinDetailLevel;

	TqInt i;
	for(i = 0; i<median; ++i)
	{
		a->m_SampleIndices.push_back(m_SampleIndices[i]);
		const SqSampleData& sample = Sample(bucket, i);
		minTime = MIN(minTime, sample.m_Time);
		maxTime = MAX(maxTime, sample.m_Time);
		minDofIndex = MIN(minDofIndex, sample.m_DofOffsetIndex);
		maxDofIndex = MAX(maxDofIndex, sample.m_DofOffsetIndex);
		minDetailLevel = MIN(minDetailLevel, sample.m_DetailLevel);
		maxDetailLevel = MAX(maxDetailLevel, sample.m_DetailLevel);
	}
	a->m_MinTime = minTime;
	a->m_MaxTime = maxTime;
	a->m_MinDofBoundIndex = minDofIndex;
	a->m_MaxDofBoundIndex = maxDofIndex;
	a->m_MinDetailLevel = minDetailLevel;
	a->m_MaxDetailLevel = maxDetailLevel;

	minTime = m_MaxTime, maxTime = m_MinTime;
	minDofIndex = m_MaxDofBoundIndex, maxDofIndex = m_MinDofBoundIndex;
	minDetailLevel = m_MaxDetailLevel, maxDetailLevel = m_MinDetailLevel;
	for(; i<samplecount; ++i)
	{
		b->m_SampleIndices.push_back(m_SampleIndices[i]);
		const SqSampleData& sample = Sample(bucket, i);
		minTime = MIN(minTime, sample.m_Time);
		maxTime = MAX(maxTime, sample.m_Time);
		minDofIndex = MIN(minDofIndex, sample.m_DofOffsetIndex);
		maxDofIndex = MAX(maxDofIndex, sample.m_DofOffsetIndex);
		minDetailLevel = MIN(minDetailLevel, sample.m_DetailLevel);
		maxDetailLevel = MAX(maxDetailLevel, sample.m_DetailLevel);
	}
	b->m_MinTime = minTime;
	b->m_MaxTime = maxTime;
	b->m_MinDofBoundIndex = minDofIndex;
	b->m_MaxDofBoundIndex = maxDofIndex;
	b->m_MinDetailLevel = minDetailLevel;
	b->m_MaxDetailLevel = maxDetailLevel;
}

void CqOcclusionTree::ConstructTree(const CqBucket* bucket)
{
	std::deque<CqOcclusionTreePtr> ChildQueue;
	ChildQueue.push_back(this/*shared_from_this()*/);

	TqInt NonLeafCount = NumSamples() >= 1 ? 1 : 0;
	TqInt split_counter = 0;

	while (NonLeafCount > 0 && ChildQueue.size() < s_ChildrenPerNode)
	{
		CqOcclusionTreePtr old = ChildQueue.front();
		ChildQueue.pop_front();
		if (old->NumSamples() > 1)
		{
			--NonLeafCount;
		}

		CqOcclusionTreePtr a;
		CqOcclusionTreePtr b;
		old->SplitNode(bucket, a, b);
		split_counter++;
		if (a)
		{
			ChildQueue.push_back(a);
			if (a->NumSamples() > 1)
			{
				++NonLeafCount;
			}
		}
		if (b)
		{
			ChildQueue.push_back(b);
			if (b->NumSamples() > 1)
			{
				++NonLeafCount;
			}
		}

		if(split_counter >1 )
			delete old;
	}

	TqChildArray::iterator ii;
	std::deque<CqOcclusionTreePtr>::const_iterator jj;
	for (ii = m_Children.begin(), jj = ChildQueue.begin(); jj != ChildQueue.end(); ++jj)
	{
		// Check if the child actually has any samples, ignore it if no.
		if( (*jj)->NumSamples() > 0)
		{
			*ii = *jj;
			(*ii)->m_Parent = this/*shared_from_this()*/;
			if ((*ii)->NumSamples() > 1)
			{
				(*ii)->ConstructTree(bucket);
			}
			++ii;
		}
	}

	while (ii != m_Children.end())
	{
		if (*ii != NULL)
		{
			delete *ii;
			*ii = NULL;
		}
		ii++;
	}
}


void CqOcclusionTree::InitialiseBounds(const CqBucket* bucket)
{
	if (m_SampleIndices.size() < 1)
		return;

	const SqSampleData& sample = Sample(bucket, 0);
	TqFloat minXVal = sample.m_Position.x();
	TqFloat maxXVal = minXVal;
	TqFloat minYVal = sample.m_Position.y();
	TqFloat maxYVal = minYVal;
	TqFloat minTime = sample.m_Time;
	TqFloat maxTime = minTime;
	TqInt	minDofIndex = sample.m_DofOffsetIndex;
	TqInt	maxDofIndex = minDofIndex;
	TqFloat	minDetailLevel = sample.m_DetailLevel;
	TqFloat	maxDetailLevel = minDetailLevel;
	for(size_t i = 1; i < m_SampleIndices.size(); i++)
	{
		const SqSampleData& sample = Sample(bucket, i);
		minXVal = MIN(minXVal, sample.m_Position.x());
		maxXVal = MAX(maxXVal, sample.m_Position.x());
		minYVal = MIN(minYVal, sample.m_Position.y());
		maxYVal = MAX(maxYVal, sample.m_Position.y());
		minTime = MIN(minTime, sample.m_Time);
		maxTime = MAX(maxTime, sample.m_Time);
		minDofIndex = MIN(minDofIndex, sample.m_DofOffsetIndex);
		maxDofIndex = MAX(maxDofIndex, sample.m_DofOffsetIndex);
		minDetailLevel = MIN(minDetailLevel, sample.m_DetailLevel);
		maxDetailLevel = MAX(maxDetailLevel, sample.m_DetailLevel);
	}
	m_MinSamplePoint[0] = minXVal;
	m_MaxSamplePoint[0] = maxXVal;
	m_MinSamplePoint[1] = minYVal;
	m_MaxSamplePoint[1] = maxYVal;
	m_MinTime = minTime;
	m_MaxTime = maxTime;
	m_MinDofBoundIndex = minDofIndex;
	m_MaxDofBoundIndex = maxDofIndex;
	m_MinDetailLevel = minDetailLevel;
	m_MaxDetailLevel = maxDetailLevel;

	// Set the opaque depths to the limits to begin with.
	m_MaxOpaqueZ = FLT_MAX;
}


void CqOcclusionTree::UpdateBounds(const CqBucket* bucket)
{
	if (m_Children[0])
	{
		assert(m_SampleIndices.size() > 1);

		TqChildArray::iterator child = m_Children.begin();
		(*child)->UpdateBounds(bucket);

		m_MinSamplePoint[0] = (*child)->m_MinSamplePoint[0];
		m_MaxSamplePoint[0] = (*child)->m_MaxSamplePoint[0];
		m_MinSamplePoint[1] = (*child)->m_MinSamplePoint[1];
		m_MaxSamplePoint[1] = (*child)->m_MaxSamplePoint[1];
		m_MinTime = (*child)->m_MinTime;
		m_MaxTime = (*child)->m_MaxTime;
		m_MinDofBoundIndex = (*child)->m_MinDofBoundIndex;
		m_MaxDofBoundIndex = (*child)->m_MaxDofBoundIndex;
		m_MinDetailLevel = (*child)->m_MinDetailLevel;
		m_MaxDetailLevel = (*child)->m_MaxDetailLevel;

		for(++child; child != m_Children.end(); ++child)
		{
			if (*child)
			{
				(*child)->UpdateBounds(bucket);

				m_MinSamplePoint[0] = std::min(m_MinSamplePoint[0], (*child)->m_MinSamplePoint[0]);
				m_MaxSamplePoint[0] = std::max(m_MaxSamplePoint[0], (*child)->m_MaxSamplePoint[0]);
				m_MinSamplePoint[1] = std::min(m_MinSamplePoint[1], (*child)->m_MinSamplePoint[1]);
				m_MaxSamplePoint[1] = std::max(m_MaxSamplePoint[1], (*child)->m_MaxSamplePoint[1]);
				m_MinTime = std::min(m_MinTime, (*child)->m_MinTime);
				m_MaxTime = std::max(m_MaxTime, (*child)->m_MaxTime);
				m_MinDofBoundIndex = std::min(m_MinDofBoundIndex, (*child)->m_MinDofBoundIndex);
				m_MaxDofBoundIndex = std::max(m_MaxDofBoundIndex, (*child)->m_MaxDofBoundIndex);
				m_MinDetailLevel = std::min(m_MinDetailLevel, (*child)->m_MinDetailLevel);
				m_MaxDetailLevel = std::max(m_MaxDetailLevel, (*child)->m_MaxDetailLevel);
			}
		}
	}
	else
	{
		assert(m_SampleIndices.size() == 1);

		const SqSampleData& sample = Sample(bucket, 0);
		m_MinSamplePoint[0] = m_MaxSamplePoint[0] = sample.m_Position[0];
		m_MinSamplePoint[1] = m_MaxSamplePoint[1] = sample.m_Position[1];
		m_MinTime = m_MaxTime = sample.m_Time;
		m_MinDofBoundIndex = m_MaxDofBoundIndex = sample.m_DofOffsetIndex;
		m_MinDetailLevel = m_MaxDetailLevel = sample.m_DetailLevel;
	}

	// Set the opaque depths to the limits to begin with.
	m_MaxOpaqueZ = FLT_MAX;
}

void CqOcclusionTree::PropagateChanges()
{
	CqOcclusionTreePtr node = this/*shared_from_this()*/;
	// Update our opaque depth based on that our our children.
	while(node)
	{
		if( node->m_Children[0] )
		{
			TqFloat maxdepth = node->m_Children[0]->m_MaxOpaqueZ;
			TqChildArray::iterator child = node->m_Children.begin();
			for (++child; child != node->m_Children.end(); ++child)
			{
				if (*child)
				{
					maxdepth = MAX((*child)->m_MaxOpaqueZ, maxdepth);
				}
			}
			// Only if this has resulted in a change at this level, should we process the parent.
			if(maxdepth < node->m_MaxOpaqueZ)
			{
				node->m_MaxOpaqueZ = maxdepth;
				node = node->m_Parent/*.lock()*/;
			}
			else
			{
				break;
			}
		}
		else
		{
			node = node->m_Parent/*.lock()*/;
		}
	}
}


bool CqOcclusionTree::CanCull( const CqBound* bound )
{
	// Recursively call each level to see if it can be culled at that point.
	// Stop recursing at a level that doesn't contain the whole bound.
	std::deque<CqOcclusionTree*> stack;
	stack.push_front(this);
	bool	top_level = true;
	while(!stack.empty())
	{
		CqOcclusionTree* node = stack.front();
		stack.pop_front();
		// Check the bound against the 2D limits of this level, if not entirely contained, then we
		// cannot cull at this level, nor at any of the children.
		CqBound b1(node->MinSamplePoint(), node->MaxSamplePoint());
		if(b1.Contains2D(*bound) || top_level)
		{
			top_level = false;
			if( bound->vecMin().z() > node->MaxOpaqueZ() )
				// If the bound is entirely contained within this node's 2D bound, and is further
				// away than the furthest opaque point, then cull.
				return(true);
			// If contained, but not behind the furthest point, push the children nodes onto the stack for
			// processing.
			CqOcclusionTree::TqChildArray::iterator childNode;
			for (childNode = node->m_Children.begin(); childNode != node->m_Children.end(); ++childNode)
			{
				if (*childNode)
				{
					stack.push_front(*childNode/*->get()*/);
				}
			}
		}
	}
	return(false);
}


SqSampleData& CqOcclusionTree::Sample(const CqBucket* bucket, size_t index) const
{
	return bucket->ImageElement(m_SampleIndices[index].first).SampleData(bucket->SamplePoints(),
									     m_SampleIndices[index].second);
}

SqSampleData& CqOcclusionTree::Sample(std::vector<CqImagePixel>& aieImage, std::vector<SqSampleData>& samplePoints, size_t index) const
{
	return aieImage[m_SampleIndices[index].first].SampleData(samplePoints, m_SampleIndices[index].second);
}


void CqOcclusionTree::StoreExtraData(const CqMicroPolygon* pMPG, SqImageSample& sample)
{
	std::map<std::string, CqRenderer::SqOutputDataEntry>& DataMap = QGetRenderContext() ->GetMapOfOutputDataEntries();
	std::map<std::string, CqRenderer::SqOutputDataEntry>::iterator entry;
	for ( entry = DataMap.begin(); entry != DataMap.end(); ++entry )
	{
		IqShaderData* pData = pMPG->pGrid()->FindStandardVar( entry->first.c_str() );
		if ( pData != NULL )
		{
			switch ( pData->Type() )
			{
					case type_float:
					case type_integer:
					{
						TqFloat f;
						pData->GetFloat( f, pMPG->GetIndex() );
						sample.Data()[ entry->second.m_Offset ] = f;
						break;
					}
					case type_point:
					case type_normal:
					case type_vector:
					case type_hpoint:
					{
						CqVector3D v;
						pData->GetPoint( v, pMPG->GetIndex() );
						sample.Data()[ entry->second.m_Offset ] = v.x();
						sample.Data()[ entry->second.m_Offset + 1 ] = v.y();
						sample.Data()[ entry->second.m_Offset + 2 ] = v.z();
						break;
					}
					case type_color:
					{
						CqColor c;
						pData->GetColor( c, pMPG->GetIndex() );
						sample.Data()[ entry->second.m_Offset ] = c.fRed();
						sample.Data()[ entry->second.m_Offset + 1 ] = c.fGreen();
						sample.Data()[ entry->second.m_Offset + 2 ] = c.fBlue();
						break;
					}
					case type_matrix:
					{
						CqMatrix m;
						pData->GetMatrix( m, pMPG->GetIndex() );
						TqFloat* pElements = m.pElements();
						sample.Data()[ entry->second.m_Offset ] = pElements[ 0 ];
						sample.Data()[ entry->second.m_Offset + 1 ] = pElements[ 1 ];
						sample.Data()[ entry->second.m_Offset + 2 ] = pElements[ 2 ];
						sample.Data()[ entry->second.m_Offset + 3 ] = pElements[ 3 ];
						sample.Data()[ entry->second.m_Offset + 4 ] = pElements[ 4 ];
						sample.Data()[ entry->second.m_Offset + 5 ] = pElements[ 5 ];
						sample.Data()[ entry->second.m_Offset + 6 ] = pElements[ 6 ];
						sample.Data()[ entry->second.m_Offset + 7 ] = pElements[ 7 ];
						sample.Data()[ entry->second.m_Offset + 8 ] = pElements[ 8 ];
						sample.Data()[ entry->second.m_Offset + 9 ] = pElements[ 9 ];
						sample.Data()[ entry->second.m_Offset + 10 ] = pElements[ 10 ];
						sample.Data()[ entry->second.m_Offset + 11 ] = pElements[ 11 ];
						sample.Data()[ entry->second.m_Offset + 12 ] = pElements[ 12 ];
						sample.Data()[ entry->second.m_Offset + 13 ] = pElements[ 13 ];
						sample.Data()[ entry->second.m_Offset + 14 ] = pElements[ 14 ];
						sample.Data()[ entry->second.m_Offset + 15 ] = pElements[ 15 ];
						break;
					}
					default:
					// left blank to avoid compiler warnings about unhandled
					//  types
					break;
			}
		}
	}
}



void CqOcclusionTree::SampleMPG( std::vector<CqImagePixel>& aieImage, std::vector<SqSampleData>& samplePoints, CqMicroPolygon* pMPG, const CqBound& bound, bool usingMB, TqFloat time0, TqFloat time1, bool usingDof, TqInt dofboundindex, const SqMpgSampleInfo& MpgSampleInfo, bool usingLOD, const SqGridInfo& gridInfo)
{
	// Check the current tree level, and if only one leaf, sample the MP, otherwise, pass it down to the left
	// and/or right side of the tree if it crosses.
	if(NumSamples() == 1)
	{
		// Sample the MPG
		SqSampleData& sample = Sample(aieImage, samplePoints, 0);

		CqStats::IncI( CqStats::SPL_count );
		TqFloat D;
		bool SampleHit = pMPG->Sample(sample, D, sample.m_Time, usingDof );

		if ( SampleHit )
		{
			bool Occludes = MpgSampleInfo.m_Occludes;
			bool opaque = MpgSampleInfo.m_IsOpaque;

			SqImageSample& currentOpaqueSample = sample.m_OpaqueSample;
			static SqImageSample localImageVal;

			SqImageSample& ImageVal = opaque ? currentOpaqueSample : localImageVal;

			// return if the sample is occluded and can be culled.
			if(opaque)
			{
				if((currentOpaqueSample.m_flags & SqImageSample::Flag_Valid) &&
				        currentOpaqueSample.Data()[Sample_Depth] <= D)
				{
					return;
				}
			}

			CqStats::IncI( CqStats::SPL_hits );
			pMPG->MarkHit();

			TqFloat* val = ImageVal.Data();
			const CqColor& col = MpgSampleInfo.m_Colour;
			const CqColor& opa = MpgSampleInfo.m_Opacity;
			val[ 0 ] = col[0];
			val[ 1 ] = col[1];
			val[ 2 ] = col[2];
			val[ 3 ] = opa[0];
			val[ 4 ] = opa[1];
			val[ 5 ] = opa[2];
			val[ 6 ] = D;

			// Now store any other data types that have been registered.
			if(gridInfo.m_UsesDataMap)
			{
				StoreExtraData(pMPG, ImageVal);
			}

			// \note: There used to be a test here to see if the current sample is 'exactly'
			// the same depth as the nearest in the list, ostensibly to check for a 'grid line' hit
			// but it didn't make sense, so was removed.

			// Update max depth values if the sample is opaque and can occlude
			// If the sample depth is closer than the current closest one, and is opaques
			// we can just replace, as we know we are in a treenode that is a leaf.
			if ( opaque )
			{
				if(D < MaxOpaqueZ())
				{
					SetMaxOpaqueZ(D);
					PropagateChanges();
				}
			}

			if(pMPG->pGrid()->usesCSG())
				ImageVal.m_pCSGNode = pMPG->pGrid() ->pCSGNode();

			ImageVal.m_flags = 0;
			if ( Occludes )
			{
				ImageVal.m_flags |= SqImageSample::Flag_Occludes;
			}
			if( gridInfo.m_IsMatte )
			{
				ImageVal.m_flags |= SqImageSample::Flag_Matte;
			}

			if(!opaque)
			{
				sample.m_Data.push_back( ImageVal );
			}
			else
			{
				// mark this sample as having been written into.
				ImageVal.m_flags |= SqImageSample::Flag_Valid;
			}
		}
	}
	else
	{
		TqChildArray::iterator child;
		TqChildArray::iterator end = m_Children.end();
		for(child = m_Children.begin(); child != end; ++child)
		{
			if (!*child)
				continue;

			if(	   (!usingDof || ((dofboundindex >= (*child)->m_MinDofBoundIndex) && (dofboundindex <= (*child)->m_MaxDofBoundIndex )) )
			        && (!usingMB || ((time0 <= (*child)->m_MaxTime) && (time1 >= (*child)->m_MinTime)) )
			        && (!usingLOD || ((gridInfo.m_LodBounds[0] <= (*child)->m_MaxDetailLevel) && (gridInfo.m_LodBounds[1] >= (*child)->m_MinDetailLevel)) )
			        && (bound.Intersects((*child)->m_MinSamplePoint, (*child)->m_MaxSamplePoint)) )
			{
				if(bound.vecMin().z() <= (*child)->m_MaxOpaqueZ || !gridInfo.m_IsCullable)
				{
					(*child)->SampleMPG(aieImage, samplePoints, pMPG, bound, usingMB, time0, time1, usingDof, dofboundindex, MpgSampleInfo, usingLOD, gridInfo);
				}
			}
		}
	}
}


/*void CqOcclusionTree::OutputTree(const char* name)
{
	std::ofstream strFile(name, std::ios_base::out|std::ios_base::app);
 
	strFile <<
			"(" << m_Tab << ", " <<  
			"(" << m_MinSamplePoint[0] << ", " << m_MinSamplePoint[1] << "), " << 
			"(" << m_MaxSamplePoint[0] << ", " << m_MinSamplePoint[1] << "), " <<
			"(" << m_MaxSamplePoint[0] << ", " << m_MinSamplePoint[1] << "), " <<
			"(" << m_MaxSamplePoint[0] << ", " << m_MaxSamplePoint[1] << "), " <<
			"(" << m_MaxSamplePoint[0] << ", " << m_MaxSamplePoint[1] << "), " <<
			"(" << m_MinSamplePoint[0] << ", " << m_MaxSamplePoint[1] << "), " <<
			"(" << m_MinSamplePoint[0] << ", " << m_MaxSamplePoint[1] << "), " <<
			"(" << m_MinSamplePoint[0] << ", " << m_MinSamplePoint[1] << ")" <<
			"), " << 
			std::endl;
 
	TqChildArray::iterator child;
	for(child = m_Children.begin(); child != m_Children.end(); ++child)
	{
		if (*child && (*child)->NumSamples() > 1)
		{
			m_Tab++;
			(*child)->OutputTree(name);
			m_Tab--;
		}
	}
}
*/


//----------------------------------------------------------------------
// Static Variables

CqOcclusionTreePtr	CqOcclusionBox::m_KDTree = 0;	///< KD Tree representing the samples in the bucket.

//----------------------------------------------------------------------
/** Constructor
*/

CqOcclusionBox::CqOcclusionBox()
{}


//----------------------------------------------------------------------
/** Destructor
*/

CqOcclusionBox::~CqOcclusionBox()
{
	delete m_KDTree;
	m_KDTree = NULL;
}


//----------------------------------------------------------------------
/** Setup the hierarchy for one bucket. Static.
	This should be called before rendering each bucket
 *\param bucket: the bucket we are about to render
 *\param xMin: left edge of this bucket (taking into account crop windows etc)
 *\param yMin: Top edge of this bucket
 *\param xMax: Right edge of this bucket
 *\param yMax: Bottom edge of this bucket
*/

void CqOcclusionBox::SetupHierarchy( const CqBucket* bucket )
{
	assert( bucket );

	if(!m_KDTree)
	{
		m_KDTree = CqOcclusionTreePtr(new CqOcclusionTree());
		// Setup the KDTree of samples
		TqInt numpixels = bucket->RealHeight() * bucket->RealWidth();
		TqInt numsamples = bucket->PixelXSamples() * bucket->PixelYSamples();
		for ( TqInt j = 0; j < numpixels; j++ )
		{
			// Gather all samples within the pixel
			for ( TqInt i = 0; i < numsamples; i++ )
			{
				m_KDTree->AddSample(std::pair<TqInt, TqInt>(j,i));
			}
		}
		// Now split the tree down until each leaf has only one sample.
		m_KDTree->InitialiseBounds(bucket);
		m_KDTree->ConstructTree(bucket);
	}

	m_KDTree->UpdateBounds(bucket);

	/*
		static TqInt i__ = 0;
		if(i__ == 800)
		{
			std::ofstream strFile("test.out");
			strFile << "xmin = " << xMin << std::endl << "ymin = " << yMin << std::endl << "xmax = " <<  xMax << std::endl << "ymax = " << yMax << std::endl << std::endl;
			strFile << "points = [" << std::endl;
			strFile.close();
			CqOcclusionTree::m_Tab = 0;
			m_KDTree->OutputTree("test.out");
			std::ofstream strFile2("test.out", std::ios_base::out|std::ios_base::app);
			strFile2 << "]" << std::endl;
		}
		i__++;
	*/
}


bool CqOcclusionBox::CanCull( const CqBound* bound ) const
{
	return(m_KDTree->CanCull(bound));
}


END_NAMESPACE( Aqsis )

