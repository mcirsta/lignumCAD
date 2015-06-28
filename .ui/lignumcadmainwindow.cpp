/****************************************************************************
** Form implementation generated from reading ui file 'lignumcadmainwindow.ui'
**
** Created: Fri Jan 3 15:57:02 2003
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#include "lignumcadmainwindow.h"

#include <qvariant.h>
#include <qapplication.h>
#include <qdockwindow.h>
#include <qfiledialog.h>
#include <qfileinfo.h>
#include <qlabel.h>
#include <qlistview.h>
#include <qmessagebox.h>
#include <qsettings.h>
#include <qstatusbar.h>
#include <qtextstream.h>
#include <qtoolbutton.h>
#include <qwhatsthis.h>
#include <stdlib.h>
#include <qmime.h>
#include <qdragobject.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qaction.h>
#include <qmenubar.h>
#include <qpopupmenu.h>
#include <qtoolbar.h>
#include <qimage.h>
#include <qpixmap.h>

#include "aboutdialog.h"
#include "configuration.h"
#include "designbookview.h"
#include "systemdependencies.h"
#include "usersettings.h"
#include "lignumcadmainwindow.ui.h"
static QPixmap uic_load_pixmap_lignumCADMainWindow( const QString &name )
{
    const QMimeSource *m = QMimeSourceFactory::defaultFactory()->data( name );
    if ( !m )
	return QPixmap();
    QPixmap pix;
    QImageDrag::decode( m, pix );
    return pix;
}
/* 
 *  Constructs a lignumCADMainWindow which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f'.
 *
 */
lignumCADMainWindow::lignumCADMainWindow( QWidget* parent,  const char* name, WFlags fl )
    : QMainWindow( parent, name, fl )
{
    (void)statusBar();
    if ( !name )
	setName( "lignumCADMainWindow" );
    resize( 957, 792 ); 
    setCaption( trUtf8( "lignumCAD" ) );
    setIcon( uic_load_pixmap_lignumCADMainWindow( "lignumCAD.png" ) );
    setIconText( trUtf8( "lignumCAD" ) );
    setCentralWidget( new QWidget( this, "qt_central_widget" ) );
    lignumCADMainWindowLayout = new QGridLayout( centralWidget(), 1, 1, 11, 0, "lignumCADMainWindowLayout"); 

    mainPixmapLabel = new QLabel( centralWidget(), "mainPixmapLabel" );
    mainPixmapLabel->setPaletteBackgroundColor( QColor( 0, 63, 64 ) );
    mainPixmapLabel->setFrameShape( QLabel::NoFrame );
    mainPixmapLabel->setFrameShadow( QLabel::Raised );
    mainPixmapLabel->setPixmap( uic_load_pixmap_lignumCADMainWindow( "splash.png" ) );
    mainPixmapLabel->setScaledContents( FALSE );
    mainPixmapLabel->setAlignment( int( QLabel::AlignBottom | QLabel::AlignHCenter ) );

    lignumCADMainWindowLayout->addWidget( mainPixmapLabel, 0, 0 );

    titleLabel = new QLabel( centralWidget(), "titleLabel" );
    titleLabel->setPaletteForegroundColor( QColor( 255, 255, 0 ) );
    titleLabel->setPaletteBackgroundColor( QColor( 0, 63, 64 ) );
    QFont titleLabel_font(  titleLabel->font() );
    titleLabel_font.setPointSize( 10 );
    titleLabel_font.setBold( TRUE );
    titleLabel->setFont( titleLabel_font ); 
    titleLabel->setText( trUtf8( "\n"
"lignumCAD Version x.y" ) );
    titleLabel->setAlignment( int( QLabel::WordBreak | QLabel::AlignTop | QLabel::AlignHCenter ) );

    lignumCADMainWindowLayout->addWidget( titleLabel, 1, 0 );

    // actions
    fileNewAction = new QAction( this, "fileNewAction" );
    fileNewAction->setIconSet( QIconSet( uic_load_pixmap_lignumCADMainWindow( "new_file.png" ) ) );
    fileNewAction->setText( trUtf8( "New" ) );
    fileNewAction->setMenuText( trUtf8( "&New" ) );
    fileNewAction->setWhatsThis( trUtf8( "<p><b>File|New</b></p><p>Click this button (or press <b>Ctrl+N</b>) to create a new model.</p><p>The current model will be cleared from memory</p>" ) );
    fileNewAction->setAccel( 4194382 );
    fileOpenAction = new QAction( this, "fileOpenAction" );
    fileOpenAction->setIconSet( QIconSet( uic_load_pixmap_lignumCADMainWindow( "open_file.png" ) ) );
    fileOpenAction->setText( trUtf8( "Open" ) );
    fileOpenAction->setMenuText( trUtf8( "&Open..." ) );
    fileOpenAction->setWhatsThis( trUtf8( "<p><b>File|Open</b></p><p>Click this button (or press <b>Ctrl+O</b>) to load an existing file.</p><p>The current model is cleared from memory.</p>" ) );
    fileOpenAction->setAccel( 4194383 );
    fileSaveAction = new QAction( this, "fileSaveAction" );
    fileSaveAction->setIconSet( QIconSet( uic_load_pixmap_lignumCADMainWindow( "save_file.png" ) ) );
    fileSaveAction->setText( trUtf8( "Save" ) );
    fileSaveAction->setMenuText( trUtf8( "&Save" ) );
    fileSaveAction->setWhatsThis( trUtf8( "<p><b>File|Save</b></p><p>Click this button (or press <b>Ctrl+S</b>) to save the current model to disk. If the model has not been assigned a file, you will be prompted for a file name with the 'Save As' file requestor.</p>" ) );
    fileSaveAction->setAccel( 4194387 );
    fileSaveAsAction = new QAction( this, "fileSaveAsAction" );
    fileSaveAsAction->setIconSet( QIconSet( uic_load_pixmap_lignumCADMainWindow( "saveas_file.png" ) ) );
    fileSaveAsAction->setText( trUtf8( "Save As" ) );
    fileSaveAsAction->setMenuText( trUtf8( "Save &As..." ) );
    fileSaveAsAction->setWhatsThis( trUtf8( "<p><b>File|Save As</b></p><p>Click this button to save the model under a new file name.</p><p>Note: you might want to change the model's name in the <b>File|Model Information</b> dialog.</p>" ) );
    fileSaveAsAction->setAccel( 0 );
    filePrintAction = new QAction( this, "filePrintAction" );
    filePrintAction->setIconSet( QIconSet( uic_load_pixmap_lignumCADMainWindow( "print.png" ) ) );
    filePrintAction->setText( trUtf8( "Print" ) );
    filePrintAction->setMenuText( trUtf8( "&Print..." ) );
    filePrintAction->setWhatsThis( trUtf8( "<p><b>File|Print</b></p><p>Click this button (or press <b>Ctrl+P</b>)  to print the model. The print dialog will appear to allow you to specify the destination printer and its attributes.</p>" ) );
    filePrintAction->setAccel( 4194384 );
    fileExitAction = new QAction( this, "fileExitAction" );
    fileExitAction->setIconSet( QIconSet( uic_load_pixmap_lignumCADMainWindow( "exit.png" ) ) );
    fileExitAction->setText( trUtf8( "Exit" ) );
    fileExitAction->setMenuText( trUtf8( "E&xit" ) );
    fileExitAction->setWhatsThis( trUtf8( "<p><b>File|Exit</b></p><p>Click this button to exit <i>lignumCAD</i>. If there are any unsaved changes to the model, you will be prompted whether to discard the changes or save the model (or cancel exiting).</p>" ) );
    fileExitAction->setAccel( 0 );
    editUndoAction = new QAction( this, "editUndoAction" );
    editUndoAction->setIconSet( QIconSet( uic_load_pixmap_lignumCADMainWindow( "undo.png" ) ) );
    editUndoAction->setText( trUtf8( "Undo" ) );
    editUndoAction->setMenuText( trUtf8( "&Undo" ) );
    editUndoAction->setWhatsThis( trUtf8( "<p><b>Edit|Undo</b></p><p>Click this button (or press <b>Ctrl+Z</b>) to undo the last user action. A brief statement of the action to be undone is given in the menu choice text or in the tool button's ToolTip.</p>" ) );
    editUndoAction->setAccel( 4194394 );
    editRedoAction = new QAction( this, "editRedoAction" );
    editRedoAction->setIconSet( QIconSet( uic_load_pixmap_lignumCADMainWindow( "redo.png" ) ) );
    editRedoAction->setText( trUtf8( "Redo" ) );
    editRedoAction->setMenuText( trUtf8( "&Redo" ) );
    editRedoAction->setWhatsThis( trUtf8( "<p><b>Edit|Redo</b></p><p>Click this button (or press <b>Ctrl+Y</b>) to redo the last user action. A brief statement of the action to be redone is given in the menu choice text or in the tool button's ToolTip.</p>" ) );
    editRedoAction->setAccel( 4194393 );
    editCutAction = new QAction( this, "editCutAction" );
    editCutAction->setIconSet( QIconSet( uic_load_pixmap_lignumCADMainWindow( "edit_cut.png" ) ) );
    editCutAction->setText( trUtf8( "Cut" ) );
    editCutAction->setMenuText( trUtf8( "&Cut" ) );
    editCutAction->setWhatsThis( trUtf8( "<p><b>Edit|Cut</b></p><p>Click this button (or press <b>Ctrl+X</b>) to delete the activated object and copy it to the clipboard.</p>" ) );
    editCutAction->setAccel( 4194392 );
    editCopyAction = new QAction( this, "editCopyAction" );
    editCopyAction->setIconSet( QIconSet( uic_load_pixmap_lignumCADMainWindow( "edit_copy.png" ) ) );
    editCopyAction->setText( trUtf8( "Copy" ) );
    editCopyAction->setMenuText( trUtf8( "C&opy" ) );
    editCopyAction->setWhatsThis( trUtf8( "<p><b>Edit|Copy</b></p><p>Click this button (or press <b>Ctrl+C</b>) to copy the activated object to the clipboard.</p>" ) );
    editCopyAction->setAccel( 4194371 );
    editPasteAction = new QAction( this, "editPasteAction" );
    editPasteAction->setIconSet( QIconSet( uic_load_pixmap_lignumCADMainWindow( "edit_paste.png" ) ) );
    editPasteAction->setText( trUtf8( "Paste" ) );
    editPasteAction->setMenuText( trUtf8( "&Paste" ) );
    editPasteAction->setWhatsThis( trUtf8( "<p><b>Edit|Paste</b></p><p>Click this button (or press <b>Ctrl+V</b>) to insert the item (if any) in the clipboard into the current page.</p><p>Note that not all potential clipboard items can be pasted into <i>lignumCAD</i>. For example 2D figures cannot generally be pasted into 3D pages.</p>" ) );
    editPasteAction->setAccel( 4194390 );
    editFindAction = new QAction( this, "editFindAction" );
    editFindAction->setIconSet( QIconSet( uic_load_pixmap_lignumCADMainWindow( "find.png" ) ) );
    editFindAction->setText( trUtf8( "Find" ) );
    editFindAction->setMenuText( trUtf8( "&Find..." ) );
    editFindAction->setWhatsThis( trUtf8( "<p><b>Edit|Find</b></p><p>Click this button (or press <b>Ctrl+F</b>) in order to find an geometry item.</p>" ) );
    editFindAction->setAccel( 4194374 );
    helpContentsAction = new QAction( this, "helpContentsAction" );
    helpContentsAction->setIconSet( QIconSet( uic_load_pixmap_lignumCADMainWindow( "help_contents.png" ) ) );
    helpContentsAction->setText( trUtf8( "Contents" ) );
    helpContentsAction->setMenuText( trUtf8( "&Contents..." ) );
    helpContentsAction->setWhatsThis( trUtf8( "<p><b>Help|Contents</b></p><p>Click this button (or press <b>F1</b>) to invoke the on-line manual for the current context.</p>" ) );
    helpContentsAction->setAccel( 4144 );
    helpAboutAction = new QAction( this, "helpAboutAction" );
    helpAboutAction->setIconSet( QIconSet( uic_load_pixmap_lignumCADMainWindow( "lignumCAD.png" ) ) );
    helpAboutAction->setText( trUtf8( "About lignumCAD" ) );
    helpAboutAction->setMenuText( trUtf8( "&About lignumCAD..." ) );
    helpAboutAction->setWhatsThis( trUtf8( "<p><b>Help|About lignumCAD</b></p><p>Click this button to see the \"About <i>lignumCAD</i>\" dialog which gives the version number and licensing information.</p>" ) );
    helpAboutAction->setAccel( 0 );
    insertSketchAction = new QAction( this, "insertSketchAction" );
    insertSketchAction->setIconSet( QIconSet( uic_load_pixmap_lignumCADMainWindow( "new_sketch.png" ) ) );
    insertSketchAction->setText( trUtf8( "&Sketch..." ) );
    insertSketchAction->setToolTip( trUtf8( "Insert a new Sketch" ) );
    insertSketchAction->setWhatsThis( trUtf8( "<p><b>Insert|Sketch</b></p><p>Click this button to insert a new page on which you can sketch the general two-dimensional outlines of the model.</p>" ) );
    insertPartAction = new QAction( this, "insertPartAction" );
    insertPartAction->setIconSet( QIconSet( uic_load_pixmap_lignumCADMainWindow( "new_part.png" ) ) );
    insertPartAction->setText( trUtf8( "&Part..." ) );
    insertPartAction->setToolTip( trUtf8( "Insert a new Part" ) );
    insertPartAction->setWhatsThis( trUtf8( "<p><b>Insert|Part</b></p><p>Click this button to create a new part. You will be prompted to specify the basic size of the stock from which a single piece of the model will be \"milled\".</p>" ) );
    insertAssemblyAction = new QAction( this, "insertAssemblyAction" );
    insertAssemblyAction->setIconSet( QIconSet( uic_load_pixmap_lignumCADMainWindow( "new_assembly.png" ) ) );
    insertAssemblyAction->setText( trUtf8( "&Assembly..." ) );
    insertAssemblyAction->setToolTip( trUtf8( "Insert a new Assembly" ) );
    insertAssemblyAction->setWhatsThis( trUtf8( "<p><b>Insert|Assembly</b></p><p>Click this button to create a new assembly. An assembly is a collection of parts and subassemblies which can be manipulated as a group.</p>" ) );
    insertDrawingAction = new QAction( this, "insertDrawingAction" );
    insertDrawingAction->setIconSet( QIconSet( uic_load_pixmap_lignumCADMainWindow( "new_drawing.png" ) ) );
    insertDrawingAction->setText( trUtf8( "&Drawing..." ) );
    insertDrawingAction->setToolTip( trUtf8( "Insert a new Drawing" ) );
    insertDrawingAction->setWhatsThis( trUtf8( "<p><b>Insert|Drawing</b></p><p>Click this button to create a new drawing. The drawing will contain the final dimensions needed to fabricate a part. (More than one part can also appear on a drawing.)</p>" ) );
    toolRectangleAction = new QAction( this, "toolRectangleAction" );
    toolRectangleAction->setIconSet( QIconSet( uic_load_pixmap_lignumCADMainWindow( "new_rectangle.png" ) ) );
    toolRectangleAction->setText( trUtf8( "Rectangle..." ) );
    toolRectangleAction->setMenuText( trUtf8( "&Rectangle..." ) );
    toolRectangleAction->setToolTip( trUtf8( "Create a new Rectangle" ) );
    toolRectangleAction->setWhatsThis( trUtf8( "<p><b>Tools|Rectangle</b></p><p>Click this button to create a rectangle.</p>" ) );
    toolDimensionAction = new QAction( this, "toolDimensionAction" );
    toolDimensionAction->setIconSet( QIconSet( uic_load_pixmap_lignumCADMainWindow( "new_dimension.png" ) ) );
    toolDimensionAction->setText( trUtf8( "Dimension..." ) );
    toolDimensionAction->setMenuText( trUtf8( "&Dimension..." ) );
    toolDimensionAction->setToolTip( trUtf8( "Create a new Dimension" ) );
    toolDimensionAction->setWhatsThis( trUtf8( "<p><b>Tools|Dimension</b></p><p>Click this button to create a dimension. Generally, you will pick two geometry items to relate with an offset constraint. However, if the first object picked is a centerline, then the next two geometries will be centered about the centerline.</p>" ) );
    toolReferenceLineAction = new QAction( this, "toolReferenceLineAction" );
    toolReferenceLineAction->setIconSet( QIconSet( uic_load_pixmap_lignumCADMainWindow( "new_reference_line.png" ) ) );
    toolReferenceLineAction->setText( trUtf8( "Reference Line..." ) );
    toolReferenceLineAction->setMenuText( trUtf8( "Reference &Line..." ) );
    toolReferenceLineAction->setToolTip( trUtf8( "Create a new Reference Line" ) );
    toolReferenceLineAction->setWhatsThis( trUtf8( "<p><b>Tools|Reference Line</b></p><p>Click this button to create a reference line</p>" ) );
    toolCenterlineAction = new QAction( this, "toolCenterlineAction" );
    toolCenterlineAction->setIconSet( QIconSet( uic_load_pixmap_lignumCADMainWindow( "new_centerline.png" ) ) );
    toolCenterlineAction->setText( trUtf8( "Centerline..." ) );
    toolCenterlineAction->setMenuText( trUtf8( "&Centerline..." ) );
    toolCenterlineAction->setToolTip( trUtf8( "Create a new Centerline" ) );
    toolCenterlineAction->setWhatsThis( trUtf8( "<p><b>Tools|Centerline</b></p><p>Click this button to create a centerline</p>" ) );
    deletePageAction = new QAction( this, "deletePageAction" );
    deletePageAction->setIconSet( QIconSet( uic_load_pixmap_lignumCADMainWindow( "delete_page.png" ) ) );
    deletePageAction->setText( trUtf8( "Delete Page" ) );
    deletePageAction->setToolTip( trUtf8( "Delete the current Page" ) );
    deletePageAction->setWhatsThis( trUtf8( "<p><b>Delete Page</b></p><p>Click this button to delete the <i>current</i> page.</p>" ) );
    renamePageAction = new QAction( this, "renamePageAction" );
    renamePageAction->setIconSet( QIconSet( uic_load_pixmap_lignumCADMainWindow( "rename_page.png" ) ) );
    renamePageAction->setText( trUtf8( "Rename Page..." ) );
    renamePageAction->setMenuText( trUtf8( "Re&name Page..." ) );
    renamePageAction->setToolTip( trUtf8( "Rename the current Page" ) );
    renamePageAction->setWhatsThis( trUtf8( "<p><b>Rename Page</b></p><p>Click this button to rename the <i>current</i> page.</p>" ) );
    toolStraightCutAction = new QAction( this, "toolStraightCutAction" );
    toolStraightCutAction->setIconSet( QIconSet( uic_load_pixmap_lignumCADMainWindow( "new_straight_cut.png" ) ) );
    toolStraightCutAction->setText( trUtf8( "Straight Cut..." ) );
    toolStraightCutAction->setToolTip( trUtf8( "Create a Straight Cut" ) );
    toolStraightCutAction->setWhatsThis( trUtf8( "<p><b>Tools|Straight Cut</b></p><p>Click this button to create a straight cut. The cut can be either <b>through</b> (i.e., all the way through the part) or <b>blind</b> (i.e., of fixed depth).</p>" ) );
    toolHoleAction = new QAction( this, "toolHoleAction" );
    toolHoleAction->setIconSet( QIconSet( uic_load_pixmap_lignumCADMainWindow( "new_hole.png" ) ) );
    toolHoleAction->setText( trUtf8( "Hole..." ) );
    toolHoleAction->setToolTip( trUtf8( "Create a Circular Hole" ) );
    toolHoleAction->setWhatsThis( trUtf8( "<p><b>Tools|Circular Hole</b></p><p>Click this button to create a circular hole. The hole can either be <b>through</b> (i.e. all the way through the part) or <b>blind</b> (i.e., of fixed depth).</p>" ) );
    toolFreeCutAction = new QAction( this, "toolFreeCutAction" );
    toolFreeCutAction->setIconSet( QIconSet( uic_load_pixmap_lignumCADMainWindow( "new_free_cut.png" ) ) );
    toolFreeCutAction->setText( trUtf8( "Free Cut..." ) );
    toolFreeCutAction->setToolTip( trUtf8( "Create a Free Cut" ) );
    toolFreeCutAction->setWhatsThis( trUtf8( "<p><b>Tools|Free Cut</b></p><p>Click this button to create a freehand cut. The cut can either be <b>through</b> (i.e., all the way through the part) or <b>blind</b> (i.e., of fixed depth).</p>" ) );
    toolAddModelAction = new QAction( this, "toolAddModelAction" );
    toolAddModelAction->setIconSet( QIconSet( uic_load_pixmap_lignumCADMainWindow( "add_model.png" ) ) );
    toolAddModelAction->setText( trUtf8( "&Add Model..." ) );
    toolAddModelAction->setToolTip( trUtf8( "Add Model (Part or Subassembly) to Assembly" ) );
    toolAddModelAction->setWhatsThis( trUtf8( "<p><b>Tools|Add Model</b></p><p>Click this button to add a part or subassembly to this assembly. You will be prompted to select a defined part or subassembly.</p>" ) );
    toolViewAction = new QAction( this, "toolViewAction" );
    toolViewAction->setIconSet( QIconSet( uic_load_pixmap_lignumCADMainWindow( "add_view.png" ) ) );
    toolViewAction->setText( trUtf8( "Add View..." ) );
    toolViewAction->setToolTip( trUtf8( "Add a View of a Model (Part or Assembly)" ) );
    toolViewAction->setWhatsThis( trUtf8( "<p><b>Tools|Add View</b></p><p>Click this button to add part or subassembly to this drawing. You will be prompted to specify a defined part or subassembly.</p>" ) );
    exportPageAction = new QAction( this, "exportPageAction" );
    exportPageAction->setIconSet( QIconSet( uic_load_pixmap_lignumCADMainWindow( "export_page.png" ) ) );
    exportPageAction->setText( trUtf8( "&Export..." ) );
    exportPageAction->setToolTip( trUtf8( "Export Page as an Interface file" ) );
    exportPageAction->setWhatsThis( trUtf8( "<p><b>File|Export</b></p><p>Click this button to export the <i>current</i> page in a form suitable for use in another application. You can specify the type of the exported file by selecting the appropriate file name extension in the export dialog.</p>" ) );
    modelInfoAction = new QAction( this, "modelInfoAction" );
    modelInfoAction->setIconSet( QIconSet( uic_load_pixmap_lignumCADMainWindow( "model_info.png" ) ) );
    modelInfoAction->setText( trUtf8( "Model &Information..." ) );
    modelInfoAction->setToolTip( trUtf8( "View and Edit the Model Information" ) );
    modelInfoAction->setWhatsThis( trUtf8( "<p><b>File|Model Information</b></p><p>Click this button to display the model information dialog. Here, you can change the name of the model, its version and revision and add any explanatory text.</p>" ) );
    toolEdgeTreatmentAction = new QAction( this, "toolEdgeTreatmentAction" );
    toolEdgeTreatmentAction->setIconSet( QIconSet( uic_load_pixmap_lignumCADMainWindow( "new_edge_treatment.png" ) ) );
    toolEdgeTreatmentAction->setText( trUtf8( "Edge Treatment..." ) );
    toolEdgeTreatmentAction->setToolTip( trUtf8( "Add an Edge Treatment " ) );
    toolEdgeTreatmentAction->setWhatsThis( trUtf8( "<p><b>Tools|Edge Treatment</b></p><p>Click this button to add an edge treatment to an edge of the part.</p>" ) );
    toolMaterialAction = new QAction( this, "toolMaterialAction" );
    toolMaterialAction->setIconSet( QIconSet( uic_load_pixmap_lignumCADMainWindow( "new_material.png" ) ) );
    toolMaterialAction->setText( trUtf8( "Material..." ) );
    toolMaterialAction->setMenuText( trUtf8( "&Material..." ) );
    toolMaterialAction->setToolTip( trUtf8( "Set the Material of the Part" ) );
    toolMaterialAction->setWhatsThis( trUtf8( "<p><b>Tools|Material</b></p><p>Click this button to set the material of the part. You will be presented the Material dialog.</p>" ) );
    toolJointAction = new QAction( this, "toolJointAction" );
    toolJointAction->setIconSet( QIconSet( uic_load_pixmap_lignumCADMainWindow( "new_joint.png" ) ) );
    toolJointAction->setText( trUtf8( "&Joint..." ) );
    toolJointAction->setToolTip( trUtf8( "Add a Joint between two parts" ) );
    toolJointAction->setWhatsThis( trUtf8( "<p><b>Tools|Add Joint</b></p><p>Click this button to create a joint between two adjacent parts. You will be presented with the <b>Joint Wizard(TM)</b> from which to choose a joint.</p>" ) );
    viewRestoreAction = new QAction( this, "viewRestoreAction" );
    viewRestoreAction->setIconSet( QIconSet( uic_load_pixmap_lignumCADMainWindow( "restore_view2.png" ) ) );
    viewRestoreAction->setText( trUtf8( "Restore view" ) );
    viewRestoreAction->setToolTip( trUtf8( "Restore the view to its default scale and position" ) );
    viewRestoreAction->setWhatsThis( trUtf8( "<p><b>Restore View</b></p><p>Click this button to restore the view to its default position and scale.</p>" ) );
    editPreferencesAction = new QAction( this, "editPreferencesAction" );
    editPreferencesAction->setIconSet( QIconSet( uic_load_pixmap_lignumCADMainWindow( "edit_properties.png" ) ) );
    editPreferencesAction->setText( trUtf8( "Pr&eferences..." ) );
    editPreferencesAction->setToolTip( trUtf8( "Edit the visible interface attributes" ) );
    editPreferencesAction->setWhatsThis( trUtf8( "<p><b>Edit|Preferences</b></p><p>Click this button to edit the application defaults for <i>lignumCAD</i>.</p><p>Defaults include:<ul><li>Business information</li><li>Presentation units</li><li>Page color scheme</li><li>Rendering attributes</li></ul></p>" ) );
    toolAlignmentAction = new QAction( this, "toolAlignmentAction" );
    toolAlignmentAction->setIconSet( QIconSet( uic_load_pixmap_lignumCADMainWindow( "new_alignment.png" ) ) );
    toolAlignmentAction->setText( trUtf8( "Alignment..." ) );
    toolAlignmentAction->setMenuText( trUtf8( "Al&ignment..." ) );
    toolAlignmentAction->setToolTip( trUtf8( "Create an Alignment" ) );
    toolAlignmentAction->setWhatsThis( trUtf8( "<p><b>Tools|Alignment</b></p><p>Click this button to create a coincident constraint between two geometry items.</p>" ) );
    cancelReferenceLineAction = new QAction( this, "cancelReferenceLineAction" );
    cancelReferenceLineAction->setIconSet( QIconSet( uic_load_pixmap_lignumCADMainWindow( "cancel_reference_line.png" ) ) );
    cancelReferenceLineAction->setText( trUtf8( "Cancel Reference Line Creation" ) );
    cancelReferenceLineAction->setWhatsThis( trUtf8( "<p><b>Cancel Reference Line Creation</b></p><p>Click this button (or press <b>ESC</b>) to cancel the creation of this reference line.</p>" ) );
    cancelRectangleAction = new QAction( this, "cancelRectangleAction" );
    cancelRectangleAction->setIconSet( QIconSet( uic_load_pixmap_lignumCADMainWindow( "cancel_rectangle.png" ) ) );
    cancelRectangleAction->setText( trUtf8( "Cancel Rectangle Creation" ) );
    cancelRectangleAction->setWhatsThis( trUtf8( "<p><b>Cancel Rectangle Creation</b></p><p>Click this button (or press <b>ESC</b>) to cancel the creation of this rectangle.</p>" ) );
    cancelCenterlineAction = new QAction( this, "cancelCenterlineAction" );
    cancelCenterlineAction->setIconSet( QIconSet( uic_load_pixmap_lignumCADMainWindow( "cancel_centerline.png" ) ) );
    cancelCenterlineAction->setText( trUtf8( "Cancel Centerline Creation" ) );
    cancelCenterlineAction->setWhatsThis( trUtf8( "<p><b>Cancel Centerline Creation</b></p><p>Click this button (or press <b>ESC</b>) to cancel the creation of this centerline.</p>" ) );
    cancelDimensionAction = new QAction( this, "cancelDimensionAction" );
    cancelDimensionAction->setIconSet( QIconSet( uic_load_pixmap_lignumCADMainWindow( "cancel_dimension.png" ) ) );
    cancelDimensionAction->setText( trUtf8( "Cancel Dimension Creation" ) );
    cancelDimensionAction->setWhatsThis( trUtf8( "<p><b>Cancel Dimension Creation</b></p><p>Click this button (or press <b>ESC</b>) to cancel the creation of this offset constraint.</p>" ) );
    cancelAlignmentAction = new QAction( this, "cancelAlignmentAction" );
    cancelAlignmentAction->setIconSet( QIconSet( uic_load_pixmap_lignumCADMainWindow( "cancel_alignment.png" ) ) );
    cancelAlignmentAction->setText( trUtf8( "Cancel Alignment Creation" ) );
    cancelAlignmentAction->setWhatsThis( trUtf8( "<p><b>Cancel Alignment Creation</b></p><p>Click this button (or press <b>ESC</b>) to cancel the creation of this coincident constraint.</p>" ) );
    toolAnnotationAction = new QAction( this, "toolAnnotationAction" );
    toolAnnotationAction->setIconSet( QIconSet( uic_load_pixmap_lignumCADMainWindow( "new_annotation.png" ) ) );
    toolAnnotationAction->setText( trUtf8( "Annotation..." ) );
    toolAnnotationAction->setMenuText( trUtf8( "&Annotation..." ) );
    toolAnnotationAction->setToolTip( trUtf8( "Create a new Annotation" ) );
    toolAnnotationAction->setWhatsThis( trUtf8( "<p><b>Tools|Annotation</b></p><p>Click this button to create an annotation.</p>" ) );
    cancelAnnotationAction = new QAction( this, "cancelAnnotationAction" );
    cancelAnnotationAction->setIconSet( QIconSet( uic_load_pixmap_lignumCADMainWindow( "cancel_annotation.png" ) ) );
    cancelAnnotationAction->setText( trUtf8( "Cancel Annotation" ) );
    cancelAnnotationAction->setWhatsThis( trUtf8( "<p><b>Cancel Annotation Creation</b></p><p>Click this button (or press <b>ESC</b>) to cancel the creation of this annotation.</p>" ) );
    toolConstraintDeleteAction = new QAction( this, "toolConstraintDeleteAction" );
    toolConstraintDeleteAction->setIconSet( QIconSet( uic_load_pixmap_lignumCADMainWindow( "delete_constraint.png" ) ) );
    toolConstraintDeleteAction->setText( trUtf8( "D&elete Constraint..." ) );
    toolConstraintDeleteAction->setToolTip( trUtf8( "Delete a Constraint (Dimension or Alignment)" ) );
    toolConstraintDeleteAction->setWhatsThis( trUtf8( "<p><b>Tools|Delete Constraint</b></p><p>Click this button to delete a constraint. Use the cursor to pick the geometry from which to remove the constraint.</p>" ) );
    cancelConstraintDeleteAction = new QAction( this, "cancelConstraintDeleteAction" );
    cancelConstraintDeleteAction->setIconSet( QIconSet( uic_load_pixmap_lignumCADMainWindow( "delete_constraint.png" ) ) );
    cancelConstraintDeleteAction->setText( trUtf8( "Cancel Constraint Deletion" ) );
    cancelConstraintDeleteAction->setToolTip( trUtf8( "Cancel Deletion of Constraint" ) );
    cancelConstraintDeleteAction->setWhatsThis( trUtf8( "<p><b>Cancel Constraint Deletion</b></p><p>Click this button (or press <b>ESC</b>) to cancel the deletion of a constraint.</p>" ) );
    whatsThisAction = new QAction( this, "whatsThisAction" );
    whatsThisAction->setText( trUtf8( "What's This?" ) );
    whatsThisAction->setMenuText( trUtf8( "&What's This?" ) );
    whatsThisAction->setWhatsThis( trUtf8( "<p><b>Help|What's This?</b></p><p>Clicking this button invokes \"What's This?\" mode, indicated by a question mark attached to the cursor. Clicking on a user interface element will cause a popup box to appear with more information about that feature.</p><p>Also, even if this button is not available, you can still press <b>Shift+F1</b> to get the \"What's This?\" message for a widget which has the keyboard focus.</p>" ) );
    whatsThisAction->setAccel( 2101296 );
    viewLeftAction = new QAction( this, "viewLeftAction" );
    viewLeftAction->setToggleAction( FALSE );
    viewLeftAction->setIconSet( QIconSet( uic_load_pixmap_lignumCADMainWindow( "view_left2.png" ) ) );
    viewLeftAction->setText( trUtf8( "View Left" ) );
    viewLeftAction->setStatusTip( trUtf8( "View the model from the left side" ) );
    viewRightAction = new QAction( this, "viewRightAction" );
    viewRightAction->setToggleAction( FALSE );
    viewRightAction->setIconSet( QIconSet( uic_load_pixmap_lignumCADMainWindow( "view_right2.png" ) ) );
    viewRightAction->setText( trUtf8( "View Right" ) );
    viewRightAction->setStatusTip( trUtf8( "View the model from the right side" ) );
    viewFrontAction = new QAction( this, "viewFrontAction" );
    viewFrontAction->setToggleAction( FALSE );
    viewFrontAction->setIconSet( QIconSet( uic_load_pixmap_lignumCADMainWindow( "view_front2.png" ) ) );
    viewFrontAction->setText( trUtf8( "View Front" ) );
    viewFrontAction->setStatusTip( trUtf8( "View the model from the front" ) );
    viewBackAction = new QAction( this, "viewBackAction" );
    viewBackAction->setToggleAction( FALSE );
    viewBackAction->setIconSet( QIconSet( uic_load_pixmap_lignumCADMainWindow( "view_back2.png" ) ) );
    viewBackAction->setText( trUtf8( "View Back" ) );
    viewBackAction->setStatusTip( trUtf8( "View the model from behind" ) );
    viewBottomAction = new QAction( this, "viewBottomAction" );
    viewBottomAction->setToggleAction( FALSE );
    viewBottomAction->setIconSet( QIconSet( uic_load_pixmap_lignumCADMainWindow( "view_bottom2.png" ) ) );
    viewBottomAction->setText( trUtf8( "View Bottom" ) );
    viewBottomAction->setStatusTip( trUtf8( "View the model from below" ) );
    viewTopAction = new QAction( this, "viewTopAction" );
    viewTopAction->setToggleAction( FALSE );
    viewTopAction->setIconSet( QIconSet( uic_load_pixmap_lignumCADMainWindow( "view_top2.png" ) ) );
    viewTopAction->setText( trUtf8( "View Top" ) );
    viewTopAction->setStatusTip( trUtf8( "View the model from above" ) );
    toggleCSysAction = new QAction( this, "toggleCSysAction" );
    toggleCSysAction->setToggleAction( TRUE );
    toggleCSysAction->setOn( TRUE );
    toggleCSysAction->setText( trUtf8( "Show Origin CSys" ) );
    toggleCSysAction->setStatusTip( trUtf8( "Toggle the display of the origin coordinate system." ) );
    cancelAddModelAction = new QAction( this, "cancelAddModelAction" );
    cancelAddModelAction->setIconSet( QIconSet( uic_load_pixmap_lignumCADMainWindow( "cancel_add_model.png" ) ) );
    cancelAddModelAction->setText( trUtf8( "Cancel Model Addition" ) );
    cancelAddModelAction->setToolTip( trUtf8( "Cancel Adding Model to Assembly" ) );
    cancelAssemblyConstraintAction = new QAction( this, "cancelAssemblyConstraintAction" );
    cancelAssemblyConstraintAction->setIconSet( QIconSet( uic_load_pixmap_lignumCADMainWindow( "cancel_assembly_constraint.png" ) ) );
    cancelAssemblyConstraintAction->setText( trUtf8( "Cancel constraint" ) );
    cancelAssemblyConstraintAction->setToolTip( trUtf8( "Cancel adding assembly constraint" ) );
    toolDeleteModelAction = new QAction( this, "toolDeleteModelAction" );
    toolDeleteModelAction->setIconSet( QIconSet( uic_load_pixmap_lignumCADMainWindow( "cancel_add_model.png" ) ) );
    toolDeleteModelAction->setText( trUtf8( "Delete Model" ) );
    toolDeleteModelAction->setMenuText( trUtf8( "&Delete Model..." ) );
    toolDeleteModelAction->setToolTip( trUtf8( "Select a model to delete from the assembly" ) );
    toolDeleteModelAction->setWhatsThis( trUtf8( "<p><b>Delete Model</b></p><p>Click on a model (subassembly) to delete it from the assembly. This does not delete the model from the project; it only removes its placement in this assembly. However, if other subassemblies depend on the selected assembly, then the subassembly will not be removed from the assembly.</p>" ) );
    zoomInAction = new QAction( this, "zoomInAction" );
    zoomInAction->setIconSet( QIconSet( uic_load_pixmap_lignumCADMainWindow( "zoom_in4.png" ) ) );
    zoomInAction->setText( trUtf8( "Zoom In" ) );
    zoomInAction->setMenuText( trUtf8( "Zoom &In" ) );
    zoomInAction->setWhatsThis( trUtf8( "<p><b>Zoom In</b></p><p>Increase the scale by a step</p>" ) );
    zoomInAction->setAccel( 4119 );
    zoomOutAction = new QAction( this, "zoomOutAction" );
    zoomOutAction->setIconSet( QIconSet( uic_load_pixmap_lignumCADMainWindow( "zoom_out4.png" ) ) );
    zoomOutAction->setText( trUtf8( "Zoom Out" ) );
    zoomOutAction->setMenuText( trUtf8( "Zoom &Out" ) );
    zoomOutAction->setWhatsThis( trUtf8( "<p><b>Zoom Out</b></p><p>Decrease the scale by a step.</p>" ) );
    zoomOutAction->setAccel( 4118 );


    // toolbars
    toolBar = new QToolBar( "", this, DockTop ); 

    toolBar->setLabel( trUtf8( "Tools" ) );
    fileNewAction->addTo( toolBar );
    fileOpenAction->addTo( toolBar );
    fileSaveAction->addTo( toolBar );
    filePrintAction->addTo( toolBar );
    toolBar->addSeparator();
    editUndoAction->addTo( toolBar );
    editRedoAction->addTo( toolBar );
    toolBar->addSeparator();
    editCutAction->addTo( toolBar );
    editCopyAction->addTo( toolBar );
    editPasteAction->addTo( toolBar );
    toolBar->addSeparator();
    helpContentsAction->addTo( toolBar );
    insertToolbar = new QToolBar( "", this, DockTop ); 

    insertToolbar->setLabel( trUtf8( "Insert" ) );


    // menubar
    menubar = new QMenuBar( this, "menubar" );

    fileMenu = new QPopupMenu( this ); 
    fileNewAction->addTo( fileMenu );
    fileMenu->insertSeparator();
    fileOpenAction->addTo( fileMenu );
    fileSaveAction->addTo( fileMenu );
    fileSaveAsAction->addTo( fileMenu );
    exportPageAction->addTo( fileMenu );
    fileMenu->insertSeparator();
    modelInfoAction->addTo( fileMenu );
    fileMenu->insertSeparator();
    filePrintAction->addTo( fileMenu );
    fileMenu->insertSeparator();
    fileExitAction->addTo( fileMenu );
    menubar->insertItem( trUtf8( "&File" ), fileMenu );

    editMenu = new QPopupMenu( this ); 
    editUndoAction->addTo( editMenu );
    editRedoAction->addTo( editMenu );
    editMenu->insertSeparator();
    editCutAction->addTo( editMenu );
    editCopyAction->addTo( editMenu );
    editPasteAction->addTo( editMenu );
    editMenu->insertSeparator();
    renamePageAction->addTo( editMenu );
    deletePageAction->addTo( editMenu );
    editFindAction->addTo( editMenu );
    editMenu->insertSeparator();
    editPreferencesAction->addTo( editMenu );
    menubar->insertItem( trUtf8( "&Edit" ), editMenu );

    PopupMenu = new QPopupMenu( this ); 
    viewRestoreAction->addTo( PopupMenu );
    PopupMenu->insertSeparator();
    zoomInAction->addTo( PopupMenu );
    zoomOutAction->addTo( PopupMenu );
    menubar->insertItem( trUtf8( "&View" ), PopupMenu );

    insertMenu = new QPopupMenu( this ); 
    menubar->insertItem( trUtf8( "&Insert" ), insertMenu );

    toolMenu = new QPopupMenu( this ); 
    menubar->insertItem( trUtf8( "&Tools" ), toolMenu );

    helpMenu = new QPopupMenu( this ); 
    helpContentsAction->addTo( helpMenu );
    helpMenu->insertSeparator();
    helpAboutAction->addTo( helpMenu );
    helpMenu->insertSeparator();
    whatsThisAction->addTo( helpMenu );
    menubar->insertItem( trUtf8( "&Help" ), helpMenu );



    // signals and slots connections
    connect( fileNewAction, SIGNAL( activated() ), this, SLOT( fileNew() ) );
    connect( fileOpenAction, SIGNAL( activated() ), this, SLOT( fileOpen() ) );
    connect( fileSaveAction, SIGNAL( activated() ), this, SLOT( fileSave() ) );
    connect( fileSaveAsAction, SIGNAL( activated() ), this, SLOT( fileSaveAs() ) );
    connect( filePrintAction, SIGNAL( activated() ), this, SLOT( filePrint() ) );
    connect( fileExitAction, SIGNAL( activated() ), this, SLOT( fileExit() ) );
    connect( editUndoAction, SIGNAL( activated() ), this, SLOT( editUndo() ) );
    connect( editRedoAction, SIGNAL( activated() ), this, SLOT( editRedo() ) );
    connect( editCutAction, SIGNAL( activated() ), this, SLOT( editCut() ) );
    connect( editCopyAction, SIGNAL( activated() ), this, SLOT( editCopy() ) );
    connect( editPasteAction, SIGNAL( activated() ), this, SLOT( editPaste() ) );
    connect( editFindAction, SIGNAL( activated() ), this, SLOT( editFind() ) );
    connect( helpContentsAction, SIGNAL( activated() ), this, SLOT( helpContents() ) );
    connect( helpAboutAction, SIGNAL( activated() ), this, SLOT( helpAbout() ) );
    connect( exportPageAction, SIGNAL( activated() ), this, SLOT( fileExport() ) );
    connect( modelInfoAction, SIGNAL( activated() ), this, SLOT( fileModelInfo() ) );
    connect( whatsThisAction, SIGNAL( activated() ), this, SLOT( whatsThis() ) );
    init();
}

/*  
 *  Destroys the object and frees any allocated resources
 */
lignumCADMainWindow::~lignumCADMainWindow()
{
    // no need to delete child widgets, Qt does it all for us
}

