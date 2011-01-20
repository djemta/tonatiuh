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

The development of Tonatiuh was started on 2004 by Dr. Manuel J. Blanco,
then Chair of the Department of Engineering of the University of Texas at
Brownsville. From May 2004 to July 2008, it was supported by the Department
of Energy (DOE) and the National Renewable Energy Laboratory (NREL) under
the Minority Research Associate (MURA) Program Subcontract ACQ-4-33623-06.
During 2007, NREL also contributed to the validation of Tonatiuh under the
framework of the Memorandum of Understanding signed with the Spanish
National Renewable Energy Centre (CENER) on February, 20, 2007 (MOU#NREL-07-117).
Since June 2006, the development of Tonatiuh is being led by the CENER, under the
direction of Dr. Blanco, now Director of CENER Solar Thermal Energy Department.

Developers: Manuel J. Blanco (mblanco@cener.com), Amaia Mutuberria, Victor Martin.

Contributors: Javier Garcia-Barberena, Inaki Perez, Inigo Pagola,  Gilda Jimenez,
Juana Amieva, Azael Mancillas, Cesar Cantu.
***************************************************************************/

#include <QComboBox>
#include <QLineEdit>
#include <QTreeView>
#include <QVBoxLayout>

#include <Inventor/SbName.h>
#include <Inventor/SbString.h>
#include <Inventor/fields/SoField.h>
#include <Inventor/fields/SoFieldContainer.h>
#include <Inventor/fields/SoSFEnum.h>
#include <Inventor/lists/SoFieldList.h>

#include "FieldContainerWidget.h"
#include "ParametersDelegate.h"
#include "ParametersItem.h"
#include "ParametersModel.h"

/**
 * Creates a new FieldContainerWidget for the parameters in the \a fieldContainer with parent \a parent.
 *
 * The container name is \a containerName.
 */
FieldContainerWidget::FieldContainerWidget( SoFieldContainer* fieldContainer, QString containerName, QWidget* parent )
:QWidget( parent ), m_ptreeView( 0 ), m_pFieldContainer( fieldContainer ), m_delegate( 0 ), m_pModel ( 0 ), m_containerName( containerName )
{
	m_ptreeView = new QTreeView;
	m_ptreeView->setAlternatingRowColors( true );
	connect (m_ptreeView, SIGNAL(doubleClicked ( const QModelIndex& ) ), this, SLOT( EditorOpened( const QModelIndex& ) ) );


	m_delegate = new ParametersDelegate;
	m_ptreeView->setItemDelegate( m_delegate );
	connect( m_delegate, SIGNAL( closeEditor( QWidget* ) ), this, SLOT( EditorClosed( QWidget* ) ) );

	QVBoxLayout* mainLayout = new QVBoxLayout;
	mainLayout->addWidget( m_ptreeView );
	setLayout( mainLayout );

	m_pModel = new ParametersModel();
	m_pModel->SetEditable( true );
	m_pModel->setHorizontalHeaderLabels( QStringList() << tr("Parameter") << tr("Value") );
	m_ptreeView->setModel(m_pModel);

	if( m_pFieldContainer ) ReadFields( );
	m_ptreeView->resizeColumnToContents ( 1 );
}

/**
 * Destroys the FieldContainerWidget object.
 */
FieldContainerWidget::~FieldContainerWidget()
{
	delete m_ptreeView;
 	delete m_delegate;
 	delete m_pModel;

}

/**
 * Sets if the parameters values can be modified.
 */
void FieldContainerWidget::SetEditable( bool editable )
{
	m_pModel->SetEditable( editable );
}

/**
 * Sets \a index as the last parameter index that has been starting to modify.
 */
void FieldContainerWidget::EditorOpened( const QModelIndex& index )
{
	m_lastEditingIndex = index;
}

/**
 * Sets to the last parameter that has begun to change the \a editor value.
 *
 * Emits a valueModificated signal width the widget current container name and the container parameters old values;
 */
void FieldContainerWidget::EditorClosed( QWidget* editor )
{
	QString newValue;
	SoField* field = m_pModel->ModelItem( m_lastEditingIndex )->GetField();
	if( field->getTypeId().isDerivedFrom( SoSFEnum::getClassTypeId() ) )
	{
		QComboBox* combo = qobject_cast<QComboBox *>(editor);
		newValue = combo->currentText( );
	}
	else
	{
		QLineEdit* textEdit = qobject_cast<QLineEdit *>(editor);
		newValue = textEdit->text();
	}

	SoFieldList fieldList;
	int numFields = m_pFieldContainer->getFields (fieldList);

	QStringList* oldValuesList = new QStringList;

	for( int num = 0; num < numFields; ++num )
	{
		SbString indexValue;
		fieldList[num]->get( indexValue );
		oldValuesList->append( QString(indexValue.getString() ) );
	 }

	 m_pModel->setData(m_lastEditingIndex, newValue, Qt::UserRole);

	 emit valueModificated( *oldValuesList, m_containerName );
}

void FieldContainerWidget::Reset()
{
	m_ptreeView->reset();
}

/**
 * Reads container parameters and for each parameters adds its name and value to de widget.
 */
void FieldContainerWidget::ReadFields( )
{
	SoFieldList fieldList;
	int totalFields = m_pFieldContainer->getFields( fieldList );

	SoField* pField = 0;
	SbName fieldName;
	SbString fieldValue = "null";

	for( int index = 0; index < totalFields; ++index )
	{
		pField = fieldList.get( index );
		if( pField )
		{
			pField->get( fieldValue );
			if( m_pFieldContainer->getFieldName( pField, fieldName ) )
			{
				m_pModel->setItem( index, false, new ParametersItem ( QString(fieldName.getString()), false, pField ));
				ParametersItem* valueItem = new ParametersItem ( QString(fieldValue.getString()), true, pField );
				m_pModel->setItem( index, true, valueItem );
			}
		}
	}
}
