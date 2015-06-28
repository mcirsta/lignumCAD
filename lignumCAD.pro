SOURCES	+= main.cpp designbookview.cpp sketchview.cpp openglview.cpp tabbarcontext.cpp partview.cpp assemblyview.cpp drawingview.cpp model.cpp lcfilechooser/lcfilechooser.cpp line.cpp geometry.cpp sketch.cpp part.cpp assembly.cpp drawing.cpp figure.cpp pageview.cpp referenceline.cpp referencelineview.cpp page.cpp handle.cpp centerlineview.cpp centerline.cpp OGLFT.cpp rectangle.cpp rectangleview.cpp dimensionview.cpp arrow.cpp figureview.cpp lcfontchooser/lcfontchooser.cpp lccolorchooser/lccolorchooser.cpp modelitem.cpp alignmentview.cpp listviewitem.cpp font/lCSymbols.c command.cpp cursorfactory.cpp constrainthistory.cpp gl2ps.c gl2ps_qt.cpp xftname.c annotation.cpp annotationview.cpp oglpaintdevice.cpp dimension.cpp openglexample.cpp openglbase.cpp units.cpp lcdefaultfilechooser/lcdefaultfilechooser.cpp lcdefaultcombobox/lcdefaultcombobox.cpp systemdependencies.cpp lcdefaultlengthspinbox/lcdefaultlengthspinbox.cpp lcdefaultratiospinbox/lcdefaultratiospinbox.cpp viewdata.cpp openglprinter.cpp libemf.cpp lcconstraintchooser/lcconstraintchooser.cpp alignment.cpp lcdefaultspinbox/lcdefaultspinbox.cpp dburl.cpp businessinfo.cpp constraintview.cpp style.cpp material.cpp ocsolid.cpp ocsolidview.cpp parttemplates.cpp lcdefaultlengthconstraint/lcdefaultlengthconstraint.cpp axis.cpp constructiondatumview.cpp axisview.cpp octexturefunction.cpp ocsoliddraw.cpp assemblyconstraint.cpp subassembly.cpp subassemblyview.cpp ocsubassemblydraw.cpp globaldata.cpp pagefactory.cpp ochiddendraw.cpp view.cpp 
HEADERS	+= designbookview.h graphics.h sketchview.h pageview.h openglview.h tabbarcontext.h partview.h assemblyview.h drawingview.h model.h lcfilechooser/lcfilechooser.h vectoralgebra.h line.h geometry.h sketch.h part.h assembly.h drawing.h figure.h referenceline.h referencelineview.h page.h handle.h centerlineview.h centerline.h OGLFT.h rectangle.h rectangleview.h dimensionview.h arrow.h figureview.h lcfontchooser/lcfontchooser.h lccolorchooser/lccolorchooser.h modelitem.h alignmentview.h listviewitem.h command.h cursorfactory.h constrainthistory.h gl2ps.h annotation.h annotationview.h oglpaintdevice.h dimension.h openglexample.h openglbase.h units.h lcdefaultfilechooser/lcdefaultfilechooser.h lcdefaultcombobox/lcdefaultcombobox.h drawingdefaults.h systemdependencies.h lcdefaultlengthspinbox/lcdefaultlengthspinbox.h lcdefaultratiospinbox/lcdefaultratiospinbox.h viewdata.h openglprinter.h libemf.h lcconstraintchooser/lcconstraintchooser.h alignment.h selectednames.h lcdefaultspinbox/lcdefaultspinbox.h dburl.h businessinfo.h usersettings.h constraintview.h lcdebug.h style.h constants.h ratio.h view.h emf.h xftint.h printer.h configuration.h material.h ocsolid.h ocsolidview.h lcdefaultlengthconstraint/lcdefaultlengthconstraint.h curve.h axis.h constructiondatum.h constructiondatumview.h axisview.h octexturefunction.h ocsoliddraw.h assemblyconstraint.h subassembly.h subassemblyview.h ocsubassemblydraw.h globaldata.h pagefactory.h ochiddendraw.h 
# The lignumCAD qmake project file
# Copyright (C) 2002 lignum Computing, Inc. <lignumcad@lignumcomputing.com>
unix {
  UI_DIR = .ui
  MOC_DIR = .moc
  OBJECTS_DIR = .obj
}
QT += widgets xml
TRANSLATIONS = lignumCAD_en.ts lignumCAD_de.ts
SUBDIRS = font
DISTFILES += README COPYING COPYING.LIB AUTHORS INSTALL *.ui.h .ui/* images/* wine/*.h doc/en/graphics/* doc/en/HTML/*.html doc/en/HTML/*.css doc/en/HTML/SOHTML/*.html doc/en/HTML/SOHTML/generated/* doc/de/graphics/* doc/de/HTML/SOHTML/*.html doc/de/HTML/SOHTML/*.png *qm materials/*.xml materials/images/* cursors/*
#unix:QMAKE_LIBS_QT_THREAD = -Wl,-Bstatic -lqt-mt -lXft -lXrender -Wl,-Bdynamic /usr/lib/libstdc++-3-libc6.2-2-2.10.0.a -lgcc -lc
unix:QMAKE_LIBS_QT_THREAD = -lqt-mt
VERSION_MAJOR = 0
VERSION_MINOR = 2
en_documentation.path = /opt/lignumCAD/v$${VERSION_MAJOR}.$${VERSION_MINOR}/doc/en/HTML
en_documentation.files = doc/en/HTML/*.html doc/en/HTML/*.css
en_documentation_graphics.path = /opt/lignumCAD/v$${VERSION_MAJOR}.$${VERSION_MINOR}/doc/en/graphics
en_documentation_graphics.files = doc/en/graphics/*
en_so_documentation.path = /opt/lignumCAD/v$${VERSION_MAJOR}.$${VERSION_MINOR}/doc/en/HTML/SOHTML
en_so_documentation.files = doc/en/HTML/SOHTML/*.html
en_so_documentation_graphics.path = /opt/lignumCAD/v$${VERSION_MAJOR}.$${VERSION_MINOR}/doc/en/HTML/SOHTML/generated
en_so_documentation_graphics.files = doc/en/HTML/SOHTML/generated/*
de_documentation_graphics.path = /opt/lignumCAD/v$${VERSION_MAJOR}.$${VERSION_MINOR}/doc/de/graphics
de_documentation_graphics.files = doc/de/graphics/*
de_so_documentation.path = /opt/lignumCAD/v$${VERSION_MAJOR}.$${VERSION_MINOR}/doc/de/HTML/SOHTML
de_so_documentation.files = doc/de/HTML/SOHTML/*.html doc/de/HTML/SOHTML/*.png
translations.path = /opt/lignumCAD/v$${VERSION_MAJOR}.$${VERSION_MINOR}/translations
translations.files = lignumCAD_*qm qt_*qm
materials.path = /opt/lignumCAD/v$${VERSION_MAJOR}.$${VERSION_MINOR}/materials
materials.files = materials/*.xml
material_images.path = /opt/lignumCAD/v$${VERSION_MAJOR}.$${VERSION_MINOR}/materials/images
material_images.files = materials/images/*
target.path = /opt/lignumCAD/v$${VERSION_MAJOR}.$${VERSION_MINOR}/bin
INSTALLS += target en_documentation en_documentation_graphics en_so_documentation en_so_documentation_graphics de_documentation_graphics de_so_documentation translations materials material_images
#unix:LIBS	+= -nodefaultlibs -Wl,--export-dynamic -lGL -Wl,-Bstatic -L/usr/local/lib -lpng -lfreetype -lTKHLR -lTKBool -lTKBO -lTKPrim -lTKTopAlgo -lTKBRep -lTKGeomAlgo -lTKGeomBase -lTKG3d -lTKG2d -lTKMath -lTKernel -L/usr/local/src/Mesa/lib -lGLU -Wl,-Bdynamic -lXmu -ljpeg -ldl -lz
FORMS	= lignumcadmainwindow.ui aboutdialog.ui pageinfodialog.ui modelinfodialog.ui newmodelwizard.ui referencelineinfodialog.ui centerlineinfodialog.ui dimensioninfodialog.ui annotationinfodialog.ui preferencesdialog.ui rectangleinfodialog.ui materialdialog.ui newpartwizard.ui partinfodialog.ui parameterinfodialog.ui assemblyconstraintform.ui assemblyconfigdialog.ui assemblyadddialog.ui offsetinfodialog.ui 
IMAGES	= images/view_top2.png images/view_bottom2.png images/view_right2.png images/view_left2.png images/view_orientation2.png images/view_front2.png images/view_back2.png images/restore_view2.png images/part_board2.png images/part_turned_knob2.png images/part_turning2.png images/part_tapered_leg2.png images/part_turned_leg2.png images/new_sketch.png images/new_part.png images/new_assembly.png images/new_drawing.png images/new_rectangle.png images/new_dimension.png images/new_reference_line.png images/new_centerline.png images/new_file.png images/open_file.png images/save_file.png images/saveas_file.png images/delete_page.png images/rename_page.png images/print.png images/undo.png images/redo.png images/edit_cut.png images/edit_copy.png images/edit_paste.png images/help_contents.png images/lignumCAD.png images/exit.png images/find.png images/new_free_cut.png images/new_hole.png images/add_model.png images/add_view.png images/export_page.png images/model_info.png images/new_edge_treatment.png images/new_material.png images/new_straight_cut.png images/restore_view.png images/assembly.png images/drawing.png images/part.png images/sketch.png images/edit_properties.png images/default_active.png images/default_inactive.png images/new_alignment.png images/handle.png images/not_allowed.png images/cancel_reference_line.png images/cancel_rectangle.png images/cancel_centerline.png images/cancel_dimension.png images/cancel_alignment.png images/new_annotation.png images/cancel_annotation.png images/splash.png images/edge_solid.png images/edge_dash.png images/edge_dot.png images/edge_dashdot.png images/edge_dashdotdot.png images/cherry.png images/shop.png images/machines.png images/new_joint.png images/paper.png images/delete_constraint.png images/cancel_add_model.png images/cancel_assembly_constraint.png images/mate_example2.png images/align_example.png images/zoom_in4.png images/zoom_out4.png 
TEMPLATE	=app
CONFIG	+= qt warn_on debug thread
DEFINES	+= OGLFT_NO_SOLID GL2PS_USE_QT GL2PS_USE_EMF
INCLUDEPATH	+= lcfilechooser lcfontchooser lccolorchooser lcdefaultfilechooser lcdefaultcombobox lcdefaultlengthspinbox lcdefaultratiospinbox lcconstraintchooser lcdefaultspinbox lcdefaultlengthconstraint ./
unix:INCLUDEPATH	+= /usr/local/include/freetype2 /usr/local/src/Mesa/include /usr/include/X11/Xft /usr/local/include/OpenCASCADE
unix:LIBS	+= -L/usr/local/lib -lfreetype -L/usr/local/src/Mesa/lib -lGLU -lTKHLR
LANGUAGE	= C++
