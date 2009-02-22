// Aqsis
// Copyright (C) 1997 - 2001, Paul C. Gregory
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
		\brief Declares the CqRiFile class for handling files with RenderMan searchpath option support.
		\author Paul C. Gregory (pgregory@aqsis.org)
*/

//? Is .h included already?
#ifndef RIFILE_H_INCLUDED
#define RIFILE_H_INCLUDED 1

#include	<iostream>

#include	"aqsis.h"
#include	"file.h"
#include	"renderer.h"

namespace Aqsis {

/** \brief Find a file in the given RI search path.
 *
 * The standard way to specify search paths is using RiOption:
 *
 * \code
 * RiOption("searchpath", "procedural", "list:of:paths");
 * \endcode
 *
 * procedurals will then be looked up in list:of:paths.
 *
 * \param fileName - name of the file
 * \param riSearchPathName - name of the search path as specified to RiOption
 *                           ("procdural" in the example above)
 */
std::string findFileInRiSearchPath(const std::string& fileName,
		const char* riSearchPathName);


//----------------------------------------------------------------------
/** \class CqRiFile
 * \brief Standard handling of all file types utilising the searchpath options.
 */

class CqRiFile : public CqFile
{
	public:
		/** Default constructor
		 */
		CqRiFile()
		{}
		/** Constructor taking an open stream pointer and a name.
		 * \param Stream a pointer to an already opened input stream to attach this object to.
		 * \param strRealName the name of the file associated with this stream.
		 */
		CqRiFile( std::istream* Stream, const char* strRealName ) :
				CqFile( Stream, strRealName )
		{}
		CqRiFile( const char* strFilename, const char* strSearchPathOption = "" )
		{
			//									std::cout << "CqRiFile::CqRiFile() " << strFilename << " - " << strSearchPathOption << std::endl;
			Open( strFilename, strSearchPathOption );
		}
		/** Destructor. Takes care of closing the stream if the constructor opened it.
		 */
		virtual	~CqRiFile()
		{}


		void	Open( const char* strFilename, const char* strSearchPathOption = 0, std::ios::openmode mode = std::ios::in )
		{
			CqString SearchPath( "" );
			if ( strSearchPathOption )
			{
				// if not found there, search in the specified option searchpath.
				const CqString * poptShader = QGetRenderContext() ->poptCurrent()->GetStringOption( "searchpath", strSearchPathOption );
				if ( poptShader != 0 )
					SearchPath = poptShader[ 0 ];

				//std::cout << "\t" << SearchPath.c_str() << std::endl;
			}
			CqFile::Open( strFilename, SearchPath.c_str(), mode );
			// If the file was not found, then try the "resource" searchpath.
			if( !IsValid() )
			{
				// if not found there, search in the specified option searchpath.
				const CqString * poptResource = QGetRenderContext() ->poptCurrent()->GetStringOption( "searchpath", "resource" );
				if ( poptResource != 0 )
				{
					SearchPath = poptResource[ 0 ];
					CqFile::Open( strFilename, SearchPath.c_str(), mode );
				}
			}
		}

};

//==============================================================================
// Implementation details
//==============================================================================

inline std::string findFileInRiSearchPath(const std::string& fileName,
		const char* riSearchPathName)
{
	const CqString* searchPath = QGetRenderContext()->poptCurrent()->
		GetStringOption("searchpath", riSearchPathName);
	if(searchPath)
		return findFileInPath(fileName, searchPath->c_str());
	else
		return findFileInPath(fileName, "");
}


} // namespace Aqsis

#endif	// !RIFILE_H_INCLUDED
