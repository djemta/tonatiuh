/***************************************************************************
Copyright (C) 2008 by the Tonatiuh Software Development Team.

This file is part of Tonatiuh.

Tonatiuh program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.


Acknowledgments:

The development of Tonatiuh was started on 2004 by Dr. Manuel Blanco,
at the time Chair of the Department of Engineering of the University of Texas
at Brownsville. From May 2004 to August 2008 Tonatiuh's development was
supported by the Department of Energy (DOE) and the National Renewable
Energy Laboratory (NREL) under the Minority Research Associate (MURA)
Program Subcontract ACQ-4-33623-06. During 2007, NREL also contributed to
the validation of Tonatiuh under the framework of the Memorandum of
Understanding signed with the Spanish National Renewable Energy Centre (CENER)
on February, 20, 2007 (MOU#NREL-07-117). Since June 2006, the development of
Tonatiuh is being led by CENER, under the direction of Dr. Blanco, now
Manager of the Solar Thermal Energy Department of CENER.

Developers: Manuel J. Blanco (mblanco@cener.com), Amaia Mutuberria, Victor Martin.

Contributors: Javier Garcia-Barberena, Inaki Perez, Inigo Pagola,  Gilda Jimenez,
Juana Amieva, Azael Mancillas, Cesar Cantu.
***************************************************************************/

/*!
 * \class PluginManager
 * \brief PluginManager class manages plugin loading.
 * PluginManager is used to load plugins, manage the list of loaded plugins.
 */

#include <QDir>
#include <QPluginLoader>
#include <QStringList>

#include "PluginManager.h"
#include "RandomDeviateFactory.h"
#include "tgf.h"
#include "TMaterialFactory.h"
#include "TPhotonMapFactory.h"
#include "TShapeFactory.h"
#include "TSunShapeFactory.h"
#include "TTrackerFactory.h"

/*!
 * Creates a new PluginManager object.
 */
PluginManager::PluginManager()
:m_RandomDeviateFactoryList( 0 ),
 m_materialFactoryList( 0 ),
 m_photonmapFactoryList( 0 ),
 m_shapeFactoryList( 0 ),
 m_sunshapeFactoryList( 0 )
{

}

/*!
 * Destroyes PluginManager object.
 */
PluginManager::~PluginManager()
{

}

/*!
 * Returns available random deviates plugins factory list.
 */
QVector< RandomDeviateFactory* > PluginManager::GetRandomDeviateFactories() const
{
	return	m_RandomDeviateFactoryList;
}

/*!
 * Returns available random deviates plugins factory list.
 */
QVector< TMaterialFactory* > PluginManager::GetMaterialFactories() const
{
	return	m_materialFactoryList;
}

/*!
 * Returns available photonmap plugins factory list.
 */
QVector< TPhotonMapFactory* > PluginManager::GetPhotonMapFactories() const
{
	return	m_photonmapFactoryList;
}

/*!
 * Returns available shape plugins factory list.
 */
QVector< TShapeFactory* > PluginManager::GetShapeFactories() const
{
	return m_shapeFactoryList;
}

/*!
 * Returns available sunshape plugins factory list.
 */
QVector< TSunShapeFactory* > PluginManager::GetSunShapeFactories() const
{
	return m_sunshapeFactoryList;
}


/*!
 * Returns available tracker plugins factory list.
 */
QVector< TTrackerFactory* > PluginManager::GetTrackerFactories() const
{
	return m_trackerFactoryList;
}

/*!
 * Loads all the valid plugins from "plugins" subdirecotry of the directory in
 * which the running version of Tonatiuh is located.
 */
void PluginManager::LoadAvailablePlugins( QDir pluginsDirectory )
{
	QStringList filesList;
	BuildFileList( pluginsDirectory, filesList );
	foreach( QString fileName, filesList ) LoadTonatiuhPlugin( fileName );
}

/*!
 * Appends to \a fileList directory files.
 */
void PluginManager::AddFilesToList( QDir directory, QStringList& filesList )
{
	QString directoryPath( directory.absolutePath().append( QLatin1String( "/" ) ) );

    QStringList filenamesList = directory.entryList( QDir::Files, QDir::Unsorted );
    for( int i = 0; i < filenamesList.size(); ++i )
    	filesList << ( directoryPath + filenamesList[i] );
}


/*!
 * Creates a list with the files al files in the defined \a directory
 * and its subdirectories.
 */
void PluginManager::BuildFileList( QDir directory, QStringList& filesList )
{
	AddFilesToList( directory, filesList );

	QString directoryPath( directory.absolutePath().append( QLatin1String( "/" ) ) );
    QStringList subdirectoriesList = directory.entryList( QDir::Dirs, QDir::Unsorted );

   for( int i = 0; i< subdirectoriesList.size(); ++i )
   {
    	QString subdirectoryName = subdirectoriesList[i];
   		if( ValidDirectoryName( subdirectoryName ) )
   			BuildFileList( QDir( directoryPath + subdirectoryName ), filesList );
   	}

}

void PluginManager::LoadMaterialPlugin( QObject* plugin )
{

	TMaterialFactory* pTMaterialFactory = qobject_cast<TMaterialFactory* >( plugin );
	if( !pTMaterialFactory )  tgf::SevereError( "MainWindow::LoadPlugins: Material plug-in not recognized" );
	pTMaterialFactory->CreateTMaterial();
	m_materialFactoryList.push_back( pTMaterialFactory );
}

void PluginManager::LoadPhotonMapPlugin( QObject* plugin )
{
	TPhotonMapFactory* pTPhotonMapFactory = qobject_cast<TPhotonMapFactory* >( plugin );
	if( !pTPhotonMapFactory ) tgf::SevereError( "MainWindow::LoadPlugins: PhotonMap plug-in not recognized" );;
	m_photonmapFactoryList.push_back( pTPhotonMapFactory );
}

/*!
 * Loads the \a plugin as RandomDeviatePlugin type.
 */
void PluginManager::LoadRandomDeviatePlugin( QObject* plugin )
{
	RandomDeviateFactory* pRamdomDeviateFactory = qobject_cast<RandomDeviateFactory* >( plugin );
	m_RandomDeviateFactoryList.push_back( pRamdomDeviateFactory );

}

void PluginManager::LoadShapePlugin( QObject* plugin )
{
	TShapeFactory* pTShapeFactory = qobject_cast<TShapeFactory* >( plugin );
	if ( !pTShapeFactory ) tgf::SevereError( "MainWindow::LoadPlugins: Shape plug-in not recognized" );
	//if( pTShapeFactory->IsFlat() )	m_TFlatShapeFactoryList.push_back( pTShapeFactory );
	pTShapeFactory->CreateTShape();
	m_shapeFactoryList.push_back( pTShapeFactory );
}

void PluginManager::LoadSunshapePlugin( QObject* plugin )
{
    TSunShapeFactory* pTSunShapeFactory = qobject_cast<TSunShapeFactory* >( plugin );
    if( !pTSunShapeFactory ) tgf::SevereError( "MainWindow::LoadPlugins: SunShape plug-in not recognized" );     	    	;
   	pTSunShapeFactory->CreateTSunShape( );
   	m_sunshapeFactoryList.push_back( pTSunShapeFactory );
}

void PluginManager::LoadTrackerPlugin( QObject* plugin )
{
    TTrackerFactory* pTTrackerFactory = qobject_cast< TTrackerFactory* >( plugin );
    if( !pTTrackerFactory ) tgf::SevereError( "MainWindow::LoadPlugins: Tracker plug-in not recognized" );
   	pTTrackerFactory->CreateTTracker( );
   	m_trackerFactoryList.push_back( pTTrackerFactory );
}


/*!
 * Loads the plugin is defined in \a fileName, if it is a valid plugin.
 */
void PluginManager::LoadTonatiuhPlugin( const QString& fileName )
{
 	QPluginLoader loader( fileName );
    QObject* plugin = loader.instance();
    if ( plugin != 0)
    {
    	if( plugin->inherits( "RandomDeviateFactory" ) ) LoadRandomDeviatePlugin( plugin );
    	if( plugin->inherits( "TShapeFactory" ) ) LoadShapePlugin( plugin );
    	if( plugin->inherits( "TSunShapeFactory" ) ) LoadSunshapePlugin( plugin );
    	if( plugin->inherits( "TMaterialFactory" ) ) LoadMaterialPlugin( plugin );
    	if( plugin->inherits( "TPhotonMapFactory" ) ) LoadPhotonMapPlugin( plugin );
    	if( plugin->inherits( "TTrackerFactory" ) ) LoadTrackerPlugin( plugin );
	}
}

/*!
 * Checks if the \a directoryName is a valid directory name.
 * '.' and '..' are not valid names.
 */
bool PluginManager::ValidDirectoryName( QString& directoryName  )
{
	return ( directoryName != QLatin1String( "." ) ) && ( directoryName != QLatin1String( ".." ) );
}
