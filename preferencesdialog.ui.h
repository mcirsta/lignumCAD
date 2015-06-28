/* -*- c++ -*-
 * preferencesdialog.h
 *
 * Header for the PreferencesDialog classes
 * Copyright (C) 2002 lignum Computing, Inc. <lignumcad@lignumcomputing.com>
 * $Id$
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */
/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you wish to add, delete or rename slots use Qt Designer which will
** update this file, preserving your code. Create an init() slot in place of
** a constructor, and a destroy() slot in place of a destructor.
*****************************************************************************/
/*!
  * There's quite a bit of initialization in this dialog (since it does so much).
  */
void PreferencesDialog::init ()
{    
    // The OpenGLExample is not a designer class, so we have to construct and lay it out
    // ourselves
    QGLFormat format;
    format.setDepth( false );

    example_ = new OpenGLExample( UnitsBasis::instance()->lengthUnit(),
				  format, "preferences example", exampleFrame );

    QGridLayout* grid = new QGridLayout( exampleFrame );

    grid->addWidget( example_, 0, 0 );

    // Populate the units listbox.
    QStringList unit_strings = UnitsBasis::instance()->lengthUnitStrings();
    QStringList::iterator unit_string = unit_strings.begin();

    for ( ; unit_string != unit_strings.end(); ++unit_string )
	unitsListBox->insertItem( *unit_string );
    
    // Wire up the predefined/custom color scheme radio buttons as exclusive.
    QButtonGroup* pcbg = new QButtonGroup( colorSchemePage, "PCBG" );
    pcbg->insert( predefinedRadioButton );
    pcbg->insert( customRadioButton );
    pcbg->hide();
    connect( pcbg, SIGNAL( clicked(int) ), SLOT(updatePredefinedScheme(int)));
    
    // Populate the predefined color scheme listbox
    QStringList scheme_strings = OpenGLGlobals::instance()->schemeStrings();
    QStringList::iterator scheme_string = scheme_strings.begin();

    for ( ; scheme_string != scheme_strings.end(); ++scheme_string )
      colorSchemeListBox->insertItem( *scheme_string );

    
    // Configure the dimension arrow head style default.
    arrowHeadStyleComboBox->insertItem( tr( arrowHeadStyleText( lC::OPEN ) ) );
    arrowHeadStyleComboBox->insertItem( tr( arrowHeadStyleText( lC::HOLLOW ) ) );
    arrowHeadStyleComboBox->insertItem( tr( arrowHeadStyleText( lC::FILLED ) ) );
    arrowHeadStyleComboBox->setDefaultValue( OpenGLGlobals::instance()->defaultArrowHeadStyle() );

    // And set the arrow head width to length ratio spin box default.
    arrowHeadWidthRatioSpinBox->setDefaultRatio( OpenGLGlobals::instance()->defaultArrowHeadWidthRatio() );
}
/*!
  * Respond to changes in the logo file name. The user can pick nothing (the default),
  * a bitmap or an SVG file (with varying degrees of success).
  * \param file_name either QString::null, i.e., no logo, or the name of file containing
  * a graphics image Qt understands.
  */
void PreferencesDialog::logoFileChooser_fileNameChanged( const QString & file_name )
{    
  if ( file_name.isEmpty() ) {
    logoLabel->setText( tr( "No Logo" ) );
    logoLabel->setEnabled( false );
  }
  else if ( QImageIO::imageFormat( file_name ) != 0 ) {
    logoLabel->setPixmap( file_name );
    logoLabel->setEnabled( true );
  }
  else if ( QFileInfo( file_name ).extension().lower() == "svg" ) {
    QPixmap pixmap( logoLabel->size() );
    QPainter painter( &pixmap, true );
    painter.fillRect( pixmap.rect(), logoLabel->paletteBackgroundColor() );

    QPicture picture;
    picture.load( file_name, "svg" );
    
    QPaintDeviceMetrics lpdm( logoLabel );
    QPaintDeviceMetrics pcpdm( &picture );
    // Try to center the logo image (as well as rendering at the proper scale)
    int width = pcpdm.width() * lpdm.logicalDpiX() / pcpdm.logicalDpiX();
    int height = pcpdm.height() * lpdm.logicalDpiY() / pcpdm.logicalDpiY();
    painter.translate( ( lpdm.width() - width ) / 2, ( lpdm.height() - height ) / 2 );
    painter.scale( (double)lpdm.logicalDpiX() / pcpdm.logicalDpiX(),
		   (double)lpdm.logicalDpiY() / pcpdm.logicalDpiY() );
    
    picture.play( &painter );

    logoLabel->setPixmap( pixmap );
    logoLabel->setEnabled( true );
  }
}

/*!
  * When the user changes the length units, update the representation choices,
  * the default precision, the OpenGL example window, and a few odd lengthspinbox's
  * which depend on the current length unit.
  * \param item the currently selected item in the list box.
  */
void PreferencesDialog::unitsListBox_currentChanged( QListBoxItem * item )
{
  int i = unitsListBox->index( item );

  LengthUnit* length_unit = UnitsBasis::instance()->lengthUnit( i );

  if ( length_unit->canBeFraction() ) {
    fractionalRadioButton->setEnabled( true );
    decimalRadioButton->setEnabled( true );
  }
  else {
    fractionalRadioButton->setEnabled( false );
    decimalRadioButton->setEnabled( true );
  }

  switch ( length_unit->defaultFormat() ) {
  case FRACTIONAL:
    fractionalRadioButton->setChecked( true ); break;
  case DECIMAL:
    decimalRadioButton->setChecked( true ); break;
  }

  enum UnitFormat format = length_unit->defaultFormat();

  precisionComboBox->setStringList( length_unit->precisionList( format ) );
  precisionComboBox->setDefaultValue( length_unit->defaultPrecision( format ) );

  updateUnitsDisplays( length_unit, format, length_unit->defaultPrecision( format));
}

/*!
  * Format changes affect the precision and any length spinboxes.
  * \param id == 0 is the Fractional radio button, == 1 is the Decimal radio button.
  */
void PreferencesDialog::formatButtonGroup_clicked( int id )
{
  UnitFormat format;

  if ( id == 0 )
    format = FRACTIONAL;
  else if ( id == 1 )
    format = DECIMAL;

  int i = unitsListBox->currentItem();

  LengthUnit* length_unit = UnitsBasis::instance()->lengthUnit( i );

  precisionComboBox->setStringList( length_unit->precisionList( format ) );
  precisionComboBox->setDefaultValue( length_unit->defaultPrecision( format ) );
  
  // Be sure to get the miscellaneous length spinboxes
  updateUnitsDisplays( length_unit, format, length_unit->defaultPrecision( format));
}

/*!
  * Mostly only affects the example here (but of course the user's input is profoundly
  * affected).
  * \param value the index into the list of possible precisions.
  */
void PreferencesDialog::precisionComboBox_valueChanged( int value )
{
  int i = unitsListBox->currentItem();

  LengthUnit* length_unit = UnitsBasis::instance()->lengthUnit( i );

  UnitFormat format;

  if ( fractionalRadioButton->isOn() )
    format = FRACTIONAL;
  else
    format = DECIMAL;

  example_->setLengthUnit( length_unit, format, value );
}

/*!
  * Toggle between the predefined/custom color scheme selections.
  * \param id == 0 is predefined radio button, == 1 is the custom radio button.
  */
void PreferencesDialog::updatePredefinedScheme( int id )
{
  switch ( id ) {
  case 0:
    predefinedRadioButton->setChecked( true );
    colorSchemeListBox->setEnabled( true );
    foregroundGroupBox->setEnabled( false );
    backgroundGroupBox->setEnabled( false );
#if 0
    // Now, we reset any changes that the user may have in order
    // to enforce the current scheme selected in the list box.
    updateColorScheme();
#endif
    break;
  case 1:
    customRadioButton->setChecked( true );
    colorSchemeListBox->setEnabled( false );
    foregroundGroupBox->setEnabled( true );
    backgroundGroupBox->setEnabled( true );
    break;
  }
}

/*!
  * The chose a new predefined scheme. Update the custom defaults and the example.
  * (The listbox item argument is ignored.)
  */
void PreferencesDialog::colorSchemeListBox_currentChanged( QListBoxItem * )
{
  int i = colorSchemeListBox->currentItem();

  PageColorScheme scheme = OpenGLGlobals::instance()->scheme( i );

  geometryColorChooser->setColor( scheme.geometryColor() );
  annotationColorChooser->setColor( scheme.annotationColor() );
  gridColorChooser->setColor( scheme.gridColor() );
  constraintPrimaryColorChooser->setColor( scheme.constraintPrimaryColor() );
  constraintSecondaryColorChooser->setColor( scheme.constraintSecondaryColor() );

  geometryColorChooser->setDefaultColor( scheme.geometryColor() );
  annotationColorChooser->setDefaultColor( scheme.annotationColor() );
  gridColorChooser->setDefaultColor( scheme.gridColor() );
  constraintPrimaryColorChooser->setDefaultColor( scheme.constraintPrimaryColor() );
  constraintSecondaryColorChooser->setDefaultColor( scheme.constraintSecondaryColor());

  backgroundSlider->setValue( scheme.backgroundStyle() );

  switch ( scheme.backgroundStyle() ) {
  case lC::Background::SOLID:
    gradientBackgroundColorChooser->setEnabled( false );
    patternBackgroundFileChooser->setEnabled( false );
    break;
  case lC::Background::GRADIENT:
    gradientBackgroundColorChooser->setEnabled( true );
    patternBackgroundFileChooser->setEnabled( false );
    break;
  case lC::Background::PATTERN:
    gradientBackgroundColorChooser->setEnabled( true );
    patternBackgroundFileChooser->setEnabled( true );
    break;
  }

  solidBackgroundColorChooser->setColor( scheme.backgroundColor() );
  gradientBackgroundColorChooser->setColor( scheme.gradientColor() );
  patternBackgroundFileChooser->setFileName( scheme.patternFile() );

  solidBackgroundColorChooser->setDefaultColor( scheme.backgroundColor() );
  gradientBackgroundColorChooser->setDefaultColor( scheme.gradientColor() );
  patternBackgroundFileChooser->setDefaultFileName( scheme.patternFile() );
  
  example_->setColorScheme( scheme );
}

/*!
  * The user selected a new default geometry color. Update the example.
  * \param color new default geometry color.
  */
void PreferencesDialog::geometryColorChooser_colorChanged( const QColor & color )
{
    example_->setGeometryColor( color );
}

/*!
  * The user selected a new default annotation color. Update the example.
  * \param color new default annotation color.
  */
void PreferencesDialog::annotationColorChooser_colorChanged( const QColor & color )
{
 example_->setAnnotationColor( color );   
}

/*!
  * The user selected a new default grid color. Update the example.
  * \param color new default grid color.
  */
void PreferencesDialog::gridColorChooser_colorChanged( const QColor & color )
{
    example_->setGridColor( color );
}

/*!
  * The user selected a new default constraint primary color. Update the example.
  * \param color new default contraint primary color.
  */
void PreferencesDialog::constraintPrimaryColorChooser_colorChanged( const QColor & color )
{
    example_->setConstraintPrimaryColor( color );
}

/*!
  * The user selected a new default constraint secondary color. Update the example.
  * \param color new default contraint secondary color.
  */
void PreferencesDialog::constraintSecondaryColorChooser_colorChanged( const QColor & color )
{
    example_->setConstraintSecondaryColor( color );
}

/*!
  * The user selected a new background style. Update the UI and the example.
  * \param style enum specifying the style.
  */
void PreferencesDialog::backgroundSlider_valueChanged( int style )
{
    switch ( style ) {
	case lC::Background::SOLID:
	gradientBackgroundColorChooser->setEnabled( false );
	patternBackgroundFileChooser->setEnabled( false );
	break;
	case lC::Background::GRADIENT:
	gradientBackgroundColorChooser->setEnabled( true );
	patternBackgroundFileChooser->setEnabled( false );
	break;
	case lC::Background::PATTERN:
	gradientBackgroundColorChooser->setEnabled( true );
	patternBackgroundFileChooser->setEnabled( true );
	break;
    }
    
    example_->setBackgroundStyle( (lC::Background::Style)style );
}

/*!
  * The user selected a new solid background color. Update the example.
  * \param color new solid background color.
  */
void PreferencesDialog::solidBackgroundColorChooser_colorChanged( const QColor & color )
{
    example_->setBackgroundColor( color );
}

/*!
  * The user selected a new gradient background color. Update the example.
  * \param color new gradient background color.
  */
void PreferencesDialog::gradientBackgroundColorChooser_colorChanged( const QColor & color )
{
    example_->setGradientColor( color );
}

/*!
  * The user selected a new image file for the background texture pattern. Update the example.
  * \param file new image file for the background texture pattern.
  */
void PreferencesDialog::patternBackgroundFileChooser_fileNameChanged( const QString & file )
{
    example_->setPatternFile( file );
}

/*!
  * The user selected a new dimension default font. Update the example.
  * \param font QFont::toString() representation of the new font.
  */
void PreferencesDialog::dimensionFontChooser_fontChanged( const QString & font )
{
    example_->setDimensionFont( font );
}

/*!
  * The user selected a new default arrow head length. Update the example.
  * \param length new default arrow head length (in paper inches).
  */
void PreferencesDialog::arrowHeadLengthSpinBox_valueChanged( double length )
{
    example_->setArrowHeadLength( length );
}


/*!
  * The user selected a new default arrow head style. Update the example.
  * \param style the new default arrow head style.
  */
void PreferencesDialog::arrowHeadStyleComboBox_valueChanged( int style )
{
  example_->setArrowHeadStyle( (lC::ArrowHeadStyle)style );
}

/*!
  * The user selected a new default dimension number clearance. Update the example.
  * \param clearance the new default dimension number clearance (in paper inches).
  */
void PreferencesDialog::clearanceLengthSpinBox_valueChanged( double clearance )
{
    example_->setClearance( clearance );
}

/*!
  * The user selected a new default dimension line thickness.
  * \param thickness the new default dimension line thickness (in paper inches).
  */
void PreferencesDialog::lineThicknessLengthSpinBox_valueChanged( double )
{
    // There is no example of this since it is for printing only.
}

/*!
  * The user selected a new default dimension extension line offset.
  * \param offset the new default dimension extension line offset (in paper inches).
  */
void PreferencesDialog::extensionOffsetLengthSpinBox_valueChanged( double offset )
{
    example_->setExtensionLineOffset( offset );
}

/*!
  * The user selected a new annotation default font. Update the example.
  * \param font QFont::toString() representation of the new font.
  */
void PreferencesDialog::annotationFontChooser_fontChanged( const QString & font )
{
    example_->setAnnotationFont( font ); 
}

/*!
  * The user selected a new default resize handle size..
  * \param size the new default resize handle size (in paper inches).
  */
void PreferencesDialog::handleLengthSpinBox_valueChanged( double size )
{
    example_->setHandleSize( size );
}

/*!
 * The user selected a new default arrow head width to length ratio. Update the example.
 * \param ratio the new default arrow head width to length ratio.
 */
void PreferencesDialog::arrowHeadWidthRatioSpinBox_valueChanged( const Ratio & ratio )
{
    example_->setArrowHeadWidthRatio( ratio );
}

/*!
  * Invoke this before exec()ing the dialog to update all the widgets to the current application
  * state.
  */
void PreferencesDialog::update( void )
{
    // Set up the default business info dialog
    businessNameLineEdit->setText( BusinessInfo::instance().name() );
    businessLocationLineEdit->setText( BusinessInfo::instance().location() );
    logoFileChooser->setFileName( BusinessInfo::instance().logo() );
    logoFileChooser_fileNameChanged( BusinessInfo::instance().logo() );
    
    // Set the LengthSpinBoxes to the current units basis. (Note: appropriate length limits must be
    // setup before setting the default value in the Units ListBox since setting the value in the
    // Units ListBox fires a signal which eventaully updates these spin boxes.)
    arrowHeadLengthSpinBox->setLengthLimits( UnitsBasis::instance()->lengthUnit(),
					     UnitsBasis::instance()->format(),
					     OpenGLGlobals::instance()->minimumArrowHeadLength(),
					     OpenGLGlobals::instance()->maximumArrowHeadLength(),
					     OpenGLGlobals::instance()->defaultArrowHeadLength() );
    
    clearanceLengthSpinBox->setLengthLimits( UnitsBasis::instance()->lengthUnit(),
					     UnitsBasis::instance()->format(),
					     OpenGLGlobals::instance()->minimumClearance(),
					     OpenGLGlobals::instance()->maximumClearance(),
					     OpenGLGlobals::instance()->defaultClearance() );

    lineThicknessLengthSpinBox->setLengthLimits( UnitsBasis::instance()->lengthUnit(),
						 UnitsBasis::instance()->format(),
						 OpenGLGlobals::instance()->minimumLineThickness(),
						 OpenGLGlobals::instance()->maximumLineThickness(),
						 OpenGLGlobals::instance()->defaultLineThickness() );

    extensionOffsetLengthSpinBox->setLengthLimits( UnitsBasis::instance()->lengthUnit(),
						   UnitsBasis::instance()->format(),
						   OpenGLGlobals::instance()->minimumExtensionLineOffset(),
						   OpenGLGlobals::instance()->maximumExtensionLineOffset(),
						   OpenGLGlobals::instance()->defaultExtensionLineOffset() );

    handleLengthSpinBox->setLengthLimits( UnitsBasis::instance()->lengthUnit(),
					  UnitsBasis::instance()->format(),
					  OpenGLGlobals::instance()->minimumHandleSize(),
					  OpenGLGlobals::instance()->maximumHandleSize(),
					  OpenGLGlobals::instance()->defaultHandleSize() );


    // Set up the default units dialog
    unitsListBox->setCurrentItem( UnitsBasis::instance()->at() );

    if ( UnitsBasis::instance()->canBeFraction() ) {
	fractionalRadioButton->setEnabled( true );
	decimalRadioButton->setEnabled( true );
    }
    else {
	fractionalRadioButton->setEnabled( false );
	decimalRadioButton->setEnabled( true );
    }

    switch ( UnitsBasis::instance()->format() ) {
    case FRACTIONAL:
	fractionalRadioButton->setChecked( true ); break;
    case DECIMAL:
	decimalRadioButton->setChecked( true ); break;
    }

    precisionComboBox->setStringList( UnitsBasis::instance()->lengthUnit()->
				      precisionList( UnitsBasis::instance()->format() ) );
    precisionComboBox->setValue( UnitsBasis::instance()->precision() );

    updateUnitsDisplays( UnitsBasis::instance()->lengthUnit(), UnitsBasis::instance()->format(),
			 UnitsBasis::instance()->precision() );
    
    // Set up the color schemes dialog

    if ( OpenGLGlobals::instance()->isPredefinedScheme() ) {
	predefinedRadioButton->setChecked( true );
	colorSchemeListBox->setEnabled( true );
	foregroundGroupBox->setEnabled( false );
	backgroundGroupBox->setEnabled( false );
    }
    else {
	customRadioButton->setChecked( true );
	colorSchemeListBox->setEnabled( false );
	foregroundGroupBox->setEnabled( true );
	backgroundGroupBox->setEnabled( true );
    }

    colorSchemeListBox->setCurrentItem( OpenGLGlobals::instance()->at() );

    geometryColorChooser->setColor( OpenGLGlobals::instance()->geometryColor() );
    annotationColorChooser->setColor( OpenGLGlobals::instance()->annotationColor() );
    gridColorChooser->setColor( OpenGLGlobals::instance()->gridColor() );
    constraintPrimaryColorChooser->setColor( OpenGLGlobals::instance()->constraintPrimaryColor() );
    constraintSecondaryColorChooser->setColor(OpenGLGlobals::instance()->constraintSecondaryColor());

  geometryColorChooser->setDefaultColor( OpenGLGlobals::instance()->defaultGeometryColor() );
  annotationColorChooser->setDefaultColor( OpenGLGlobals::instance()->defaultAnnotationColor() );
  gridColorChooser->setDefaultColor( OpenGLGlobals::instance()->defaultGridColor() );
  constraintPrimaryColorChooser->setDefaultColor( OpenGLGlobals::instance()->defaultConstraintPrimaryColor() );
  constraintSecondaryColorChooser->setDefaultColor(OpenGLGlobals::instance()->defaultConstraintSecondaryColor());

  backgroundSlider->setValue( OpenGLGlobals::instance()->backgroundStyle() );

  switch ( OpenGLGlobals::instance()->backgroundStyle() ) {
  case lC::Background::SOLID:
	gradientBackgroundColorChooser->setEnabled( false );
             patternBackgroundFileChooser->setEnabled( false );
	 break;
    case lC::Background::GRADIENT:
              gradientBackgroundColorChooser->setEnabled( true );
              patternBackgroundFileChooser->setEnabled( false );
              break;
  case lC::Background::PATTERN:
	  gradientBackgroundColorChooser->setEnabled( true );
              patternBackgroundFileChooser->setEnabled( true );
              break;
   }

  solidBackgroundColorChooser->setColor( OpenGLGlobals::instance()->backgroundColor() );
  gradientBackgroundColorChooser->setColor( OpenGLGlobals::instance()->gradientColor() );
  patternBackgroundFileChooser->setFileName( OpenGLGlobals::instance()->patternFile() );

  solidBackgroundColorChooser->setDefaultColor( OpenGLGlobals::instance()->defaultBackgroundColor() );
  gradientBackgroundColorChooser->setDefaultColor( OpenGLGlobals::instance()->defaultGradientColor() );
  patternBackgroundFileChooser->setDefaultFileName( OpenGLGlobals::instance()->defaultPatternFile() );
  
  // Set up the miscellaneous attributes dialog (dimensions, handles, etc.)
  dimensionFontChooser->setFont( OpenGLGlobals::instance()->dimensionFont() );

  arrowHeadLengthSpinBox->setLength( OpenGLGlobals::instance()->arrowHeadLength() );

  arrowHeadWidthRatioSpinBox->setRatio( OpenGLGlobals::instance()->arrowHeadWidthRatio() );

  arrowHeadStyleComboBox->setValue( OpenGLGlobals::instance()->arrowHeadStyle() );

  clearanceLengthSpinBox->setLength( OpenGLGlobals::instance()->clearance() );

  lineThicknessLengthSpinBox->setLength( OpenGLGlobals::instance()->lineThickness() );

  extensionOffsetLengthSpinBox->setLength( OpenGLGlobals::instance()->extensionLineOffset() );

  annotationFontChooser->setFont( OpenGLGlobals::instance()->annotationFont() );

  handleLengthSpinBox->setLength( OpenGLGlobals::instance()->handleSize() );

}

/*!
  * Update the various widgets whose display depends on the units.
  * \param length_unit the new length unit.
  * \param format the new format.
  * \param precisino the new precision.
  */
void PreferencesDialog::updateUnitsDisplays( LengthUnit* length_unit, UnitFormat format,
					    int precision )
{
  arrowHeadLengthSpinBox->setLengthUnit( length_unit, format );
  clearanceLengthSpinBox->setLengthUnit( length_unit, format );
  lineThicknessLengthSpinBox->setLengthUnit( length_unit, format );
  extensionOffsetLengthSpinBox->setLengthUnit( length_unit, format );
  handleLengthSpinBox->setLengthUnit( length_unit, format );

  example_->setLengthUnit( length_unit, format, precision );
}

/*!
  * Display the help text for the current tab page.
  */
void PreferencesDialog::buttonHelp_clicked()
{
    if (  preferencesTabWidget->currentPage() == identificationPage ) {
	QWhatsThis::display( tr( "<p><b>Identification</b></p> \
<p><i>lignumCAD</i> allows you to identify yourself with \
a Business name and location. Of course, a user can \
just as well use these fields since they are not interpreted \
by the program in any way. The contents of these fields \
will show up on printed pages in the configuration control \
box.</p>\
<p>You can also enter an optional logo. This is usually \
a raster image file or an SVG (scalable vector graphics). \
If specified, the logo will appear along-side the name \
and location in the configuration control box on the \
printed output.</p>\
<p>Once you have edited the identification to your \
satisfaction, you can click on the <b>OK</b> button \
(or press <b>Enter</b> or <b>Alt+O</b>) to apply \
the changes and exit the dialog. You can leave the \
dialog without applying the changes by clicking the \
<b>Cancel</b> button (or by pressing <b>ESC</b> \
or <b>Alt+C</b>).<p>\
<p>These settings are stored in a configuration file \
and are restored on the next invocation of \
<i>lignumCAD</i></p>" ) );
    }
    else if ( preferencesTabWidget->currentPage() == unitsPage ) {
	QWhatsThis::display( tr( "<p><b>Length Units Display Representation</b></p>\
<p>First a confession: Every length in <i>lignumCAD</i> is \
stored in inches. Even if you change the length unit, \
the internal representation is still in inches. \
Therefore, this is really the length unit <i>display</i> \
representation since each number displayed is converted \
from inches to the requested unit on the fly.</p> \
<p>Due to the wide variety of unit systems in current \
use, you have a number of choices for length unit display. \
You can choose from English (inch, foot, yard) or \
Metric (mm,cm,m). For the English units, you can choose \
either fractional (i.e., 1/2) or decimal (i.e., 0.5) \
format. (You only get decimal for metric units.) \
The only particular shortcoming of <i>lignumCAD</i>'s \
current implementation is that you cannot mix units. \
(However, if you choose feet or yards with fractional \
representation, the program will display only as \
many units as necessary, i.e., an inch and a half \
is displayed as <code>1 1/2\"</code>, not as \
<code>0 yd 0' 1 1/2\"</code></p>\
<p>Note, too, that as you vary the length unit, the example \
geometry in the window at the bottom of the dialog \
will show a typical value displayed in the requested \
style.</p>\
<p>Once you have edited the length units to your \
satisfaction, you can click on the <b>OK</b> button \
(or press <b>Enter</b> or <b>Alt+O</b>) to apply \
the changes and exit the dialog. You can leave the \
dialog without applying the changes by clicking the \
<b>Cancel</b> button (or by pressing <b>ESC</b> \
or <b>Alt+C</b>).<p>\
<p>These settings are stored in a configuration file \
and are restored on the next invocation of \
<i>lignumCAD</i></p>" ) );
    }	
    else if ( preferencesTabWidget->currentPage() == colorSchemePage ) {
	QWhatsThis::display( tr( "<p><b>Color Scheme</b></p>\
<p>All of the colors used in <i>lignumCAD</i> are selectable \
by the user. You can either select from a set of predefined \
color schemes which are compiled into the program or \
you can define a custom set.</p>\
<p>Note that as you vary the color choices, the example \
geometry in the window at the bottom of the dialog \
will update to show to display in the requested \
style.</p>\
<p>Once you have edited the color scheme to your \
satisfaction, you can click on the <b>OK</b> button \
(or press <b>Enter</b> or <b>Alt+O</b>) to apply \
the changes and exit the dialog. You can leave the \
dialog without applying the changes by clicking the \
<b>Cancel</b> button (or by pressing <b>ESC</b> \
or <b>Alt+C</b>).<p>\
<p>These settings are stored in a configuration file \
and are restored on the next invocation of \
<i>lignumCAD</i></p>" ) );
    }
    else if ( preferencesTabWidget->currentPage() == otherStylesPage ) {
	QWhatsThis::display( tr( "<p><b>Other Styles</b></p>\
<p>There are a number of stylistic properties related to the \
technical drawing <i>lignumCAD</i> tries to emulate. \
They are largely related to the sizes of drawn elements. \
However, rather than impose the values from an \
arbitrarily chosen \
standard (such as ANSI Y14.5-1966*), you can \
adjust most of these values to suit yourself.</p>\
<p>Note that properties which specify a length \
(including font size) are implicitly given in terms of \
<i>paper</i> units; that is, the size is that at which \
they will be printed. This is independent of the scale \
of a particular page view. For example, a 20\" x 15\" \
rectangle in a view with a scale of 5 : 1 \
will be printed (or drawn on the screen) \
as a 4\" x 3\" rectangle; but, a quarter inch dimension \
arrow head will be printed (or drawn on the screen) \
at a quarter inch.</p> \
<p>Once you have edited the styles to your \
satisfaction, you can click on the <b>OK</b> button \
(or press <b>Enter</b> or <b>Alt+O</b>) to apply \
the changes and exit the dialog. You can leave the \
dialog without applying the changes by clicking the \
<b>Cancel</b> button (or by pressing <b>ESC</b> \
or <b>Alt+C</b>).<p>\
<p>These settings are stored in a configuration file \
and are restored on the next invocation of \
<i>lignumCAD</i>.</p>\
<p><small>*Well, the defaults are actually from \
that standard.</small></p>" ) );
    }
}
