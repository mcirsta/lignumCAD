cmake_minimum_required(VERSION 3.21)

project(lignumCAD
  VERSION 0.3
  LANGUAGES C CXX
)

include(GNUInstallDirs)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS
  Widgets
  OpenGL
  OpenGLWidgets
  Xml
  PrintSupport
  Svg
)

find_package(Freetype REQUIRED)
find_package(Fontconfig REQUIRED)
find_package(OpenGL REQUIRED)
find_package(PkgConfig QUIET)
if(TARGET OpenGL::GLU)
  set(LIGNUMCAD_GLU_TARGET OpenGL::GLU)
else()
  find_package(PkgConfig REQUIRED)
  pkg_check_modules(GLU REQUIRED IMPORTED_TARGET glu)
  set(LIGNUMCAD_GLU_TARGET PkgConfig::GLU)
endif()
find_package(ZLIB REQUIRED)

if(PkgConfig_FOUND)
  pkg_check_modules(GL2PS QUIET IMPORTED_TARGET gl2ps)
endif()

if(TARGET PkgConfig::GL2PS)
  set(LIGNUMCAD_GL2PS_TARGET PkgConfig::GL2PS)
else()
  find_path(GL2PS_INCLUDE_DIR NAMES gl2ps.h REQUIRED)
  find_library(GL2PS_LIBRARY NAMES gl2ps REQUIRED)
  add_library(GL2PS::GL2PS UNKNOWN IMPORTED)
  set_target_properties(GL2PS::GL2PS PROPERTIES
    IMPORTED_LOCATION "${GL2PS_LIBRARY}"
    INTERFACE_INCLUDE_DIRECTORIES "${GL2PS_INCLUDE_DIR}"
  )
  set(LIGNUMCAD_GL2PS_TARGET GL2PS::GL2PS)
endif()

set(LIGNUMCAD_OPENCASCADE_MODULES
  FoundationClasses
  ModelingData
  ModelingAlgorithms
)

find_package(OpenCASCADE CONFIG QUIET COMPONENTS
  ${LIGNUMCAD_OPENCASCADE_MODULES}
)

if(OpenCASCADE_FOUND)
  foreach(_module IN LISTS LIGNUMCAD_OPENCASCADE_MODULES)
    list(APPEND LIGNUMCAD_OPENCASCADE_LIBRARIES
      ${OpenCASCADE_${_module}_LIBRARIES}
    )
  endforeach()
else()
  find_path(OpenCASCADE_INCLUDE_DIR
    NAMES Standard_Version.hxx
    PATHS /usr/include /usr/local/include
    PATH_SUFFIXES opencascade OpenCASCADE
  )
  if(NOT OpenCASCADE_INCLUDE_DIR)
    message(FATAL_ERROR "Could not find OpenCascade headers")
  endif()

  set(_lignumcad_opencascade_library_names
    TKernel
    TKMath
    TKG2d
    TKG3d
    TKGeomBase
    TKBRep
    TKGeomAlgo
    TKTopAlgo
    TKPrim
    TKBO
    TKShHealing
    TKBool
    TKHLR
    TKFillet
    TKOffset
    TKFeat
    TKMesh
    TKXMesh
  )

  foreach(_library_name IN LISTS _lignumcad_opencascade_library_names)
    find_library(OpenCASCADE_${_library_name}_LIBRARY
      NAMES ${_library_name}
      PATHS /usr/lib /usr/local/lib
    )
    if(NOT OpenCASCADE_${_library_name}_LIBRARY)
      message(FATAL_ERROR "Could not find OpenCascade library ${_library_name}")
    endif()
    list(APPEND LIGNUMCAD_OPENCASCADE_LIBRARIES
      ${OpenCASCADE_${_library_name}_LIBRARY}
    )
  endforeach()
endif()

list(REMOVE_DUPLICATES LIGNUMCAD_OPENCASCADE_LIBRARIES)

set(LIGNUMCAD_INSTALL_DATA_DIR
  "${CMAKE_INSTALL_DATADIR}/lignumCAD/v${PROJECT_VERSION}"
)
set(LIGNUMCAD_INSTALL_FULL_DATA_DIR
  "${CMAKE_INSTALL_FULL_DATADIR}/lignumCAD/v${PROJECT_VERSION}"
)
set(LIGNUMCAD_DEVELOPMENT_DATA_DIR
  "${CMAKE_CURRENT_SOURCE_DIR}"
)
option(LIGNUMCAD_ENABLE_SOURCE_DATA_FALLBACK
  "Allow uninstalled builds to load runtime data from the source tree"
  ON
)
if(LIGNUMCAD_ENABLE_SOURCE_DATA_FALLBACK)
  set(LIGNUMCAD_ENABLE_DEVELOPMENT_DATA_DIR true)
else()
  set(LIGNUMCAD_ENABLE_DEVELOPMENT_DATA_DIR false)
endif()

configure_file(
  configuration.h.in
  "${CMAKE_CURRENT_BINARY_DIR}/generated/configuration.h"
  @ONLY
)

set(LIGNUMCAD_SOURCES
  main.cpp
  runtimepaths.cpp
  uiformwrappers.cpp
  designbookview.cpp
  sketchview.cpp
  openglview.cpp
  tabbarcontext.cpp
  partview.cpp
  assemblyview.cpp
  drawingview.cpp
  model.cpp
  lcfilechooser/lcfilechooser.cpp
  line.cpp
  geometry.cpp
  sketch.cpp
  part.cpp
  assembly.cpp
  drawing.cpp
  figure.cpp
  pageview.cpp
  referenceline.cpp
  referencelineview.cpp
  page.cpp
  handle.cpp
  centerlineview.cpp
  centerline.cpp
  OGLFT.cpp
  rectangle.cpp
  rectangleview.cpp
  dimensionview.cpp
  arrow.cpp
  figureview.cpp
  lcfontchooser/lcfontchooser.cpp
  lccolorchooser/lccolorchooser.cpp
  modelitem.cpp
  alignmentview.cpp
  listviewitem.cpp
  font/lCSymbols.c
  command.cpp
  cursorfactory.cpp
  constrainthistory.cpp
  annotation.cpp
  annotationview.cpp
  oglpaintdevice.cpp
  dimension.cpp
  openglexample.cpp
  openglbase.cpp
  units.cpp
  lcdefaultfilechooser/lcdefaultfilechooser.cpp
  lcdefaultcombobox/lcdefaultcombobox.cpp
  systemdependencies.cpp
  lcdefaultlengthspinbox/lcdefaultlengthspinbox.cpp
  lcdefaultratiospinbox/lcdefaultratiospinbox.cpp
  viewdata.cpp
  openglprinter.cpp
  lcconstraintchooser/lcconstraintchooser.cpp
  alignment.cpp
  lcdefaultspinbox/lcdefaultspinbox.cpp
  dburl.cpp
  businessinfo.cpp
  constraintview.cpp
  style.cpp
  material.cpp
  ocsolid.cpp
  ocsolidview.cpp
  parttemplates.cpp
  lcdefaultlengthconstraint/lcdefaultlengthconstraint.cpp
  axis.cpp
  constructiondatumview.cpp
  axisview.cpp
  octexturefunction.cpp
  ocsoliddraw.cpp
  assemblyconstraint.cpp
  subassembly.cpp
  subassemblyview.cpp
  ocsubassemblydraw.cpp
  globaldata.cpp
  pagefactory.cpp
  ochiddendraw.cpp
  view.cpp
)

set(LIGNUMCAD_HEADERS
  aboutdialog.h
  annotationinfodialog.h
  assemblyadddialog.h
  assemblyconfigdialog.h
  assemblyconstraintform.h
  centerlineinfodialog.h
  dimensioninfodialog.h
  lignumcadmainwindow.h
  materialdialog.h
  modelinfodialog.h
  newmodelwizard.h
  newpartwizard.h
  offsetinfodialog.h
  pageinfodialog.h
  parameterinfodialog.h
  partinfodialog.h
  preferencesdialog.h
  rectangleinfodialog.h
  referencelineinfodialog.h
  designbookview.h
  graphics.h
  sketchview.h
  pageview.h
  openglview.h
  tabbarcontext.h
  partview.h
  assemblyview.h
  drawingview.h
  model.h
  lcfilechooser/lcfilechooser.h
  vectoralgebra.h
  line.h
  geometry.h
  sketch.h
  part.h
  assembly.h
  drawing.h
  figure.h
  referenceline.h
  referencelineview.h
  page.h
  handle.h
  centerlineview.h
  centerline.h
  OGLFT.h
  rectangle.h
  rectangleview.h
  dimensionview.h
  arrow.h
  figureview.h
  lcfontchooser/lcfontchooser.h
  lccolorchooser/lccolorchooser.h
  modelitem.h
  alignmentview.h
  listviewitem.h
  command.h
  cursorfactory.h
  constrainthistory.h
  annotation.h
  annotationview.h
  oglpaintdevice.h
  dimension.h
  openglexample.h
  openglbase.h
  units.h
  lcdefaultfilechooser/lcdefaultfilechooser.h
  lcdefaultcombobox/lcdefaultcombobox.h
  drawingdefaults.h
  systemdependencies.h
  lcdefaultlengthspinbox/lcdefaultlengthspinbox.h
  lcdefaultratiospinbox/lcdefaultratiospinbox.h
  viewdata.h
  openglprinter.h
  lcconstraintchooser/lcconstraintchooser.h
  alignment.h
  selectednames.h
  lcdefaultspinbox/lcdefaultspinbox.h
  dburl.h
  businessinfo.h
  usersettings.h
  runtimepaths.h
  constraintview.h
  lcdebug.h
  style.h
  constants.h
  ratio.h
  view.h
  printer.h
  ${CMAKE_CURRENT_BINARY_DIR}/generated/configuration.h
  material.h
  ocsolid.h
  ocsolidview.h
  lcdefaultlengthconstraint/lcdefaultlengthconstraint.h
  curve.h
  axis.h
  constructiondatum.h
  constructiondatumview.h
  axisview.h
  octexturefunction.h
  ocsoliddraw.h
  assemblyconstraint.h
  subassembly.h
  subassemblyview.h
  ocsubassemblydraw.h
  globaldata.h
  pagefactory.h
  ochiddendraw.h
)

set(LIGNUMCAD_FORMS
  lignumcadmainwindow.ui
  aboutdialog.ui
  pageinfodialog.ui
  modelinfodialog.ui
  newmodelwizard.ui
  referencelineinfodialog.ui
  centerlineinfodialog.ui
  dimensioninfodialog.ui
  annotationinfodialog.ui
  preferencesdialog.ui
  rectangleinfodialog.ui
  materialdialog.ui
  newpartwizard.ui
  partinfodialog.ui
  parameterinfodialog.ui
  assemblyconstraintform.ui
  assemblyconfigdialog.ui
  assemblyadddialog.ui
  offsetinfodialog.ui
)

set(LIGNUMCAD_IMAGES
  images/view_top2.png
  images/view_bottom2.png
  images/view_right2.png
  images/view_left2.png
  images/view_orientation2.png
  images/view_front2.png
  images/view_back2.png
  images/restore_view2.png
  images/part_board2.png
  images/part_turned_knob2.png
  images/part_turning2.png
  images/part_tapered_leg2.png
  images/part_turned_leg2.png
  images/new_sketch.png
  images/new_part.png
  images/new_assembly.png
  images/new_drawing.png
  images/new_rectangle.png
  images/new_dimension.png
  images/new_reference_line.png
  images/new_centerline.png
  images/new_file.png
  images/open_file.png
  images/save_file.png
  images/saveas_file.png
  images/delete_page.png
  images/rename_page.png
  images/print.png
  images/undo.png
  images/redo.png
  images/edit_cut.png
  images/edit_copy.png
  images/edit_paste.png
  images/help_contents.png
  images/lignumCAD.png
  images/dimensioninfodialog.png
  images/exit.png
  images/find.png
  images/new_free_cut.png
  images/new_hole.png
  images/add_model.png
  images/add_view.png
  images/export_page.png
  images/model_info.png
  images/new_edge_treatment.png
  images/new_material.png
  images/new_straight_cut.png
  images/restore_view.png
  images/assembly.png
  images/drawing.png
  images/part.png
  images/sketch.png
  images/edit_properties.png
  images/default_active.png
  images/default_inactive.png
  images/new_alignment.png
  images/handle.png
  images/not_allowed.png
  images/cancel_reference_line.png
  images/cancel_rectangle.png
  images/cancel_centerline.png
  images/cancel_dimension.png
  images/cancel_alignment.png
  images/new_annotation.png
  images/cancel_annotation.png
  images/splash.png
  images/edge_solid.png
  images/edge_dash.png
  images/edge_dot.png
  images/edge_dashdot.png
  images/edge_dashdotdot.png
  images/cherry.png
  images/shop.png
  images/machines.png
  images/new_joint.png
  images/paper.png
  images/delete_constraint.png
  images/cancel_add_model.png
  images/cancel_assembly_constraint.png
  images/mate_example2.png
  images/align_example.png
  images/zoom_in4.png
  images/zoom_out4.png
)

add_executable(lignumCAD
  ${LIGNUMCAD_SOURCES}
  ${LIGNUMCAD_HEADERS}
  ${LIGNUMCAD_FORMS}
)

qt_add_resources(lignumCAD "lignumcad_images"
  PREFIX "/"
  FILES
    ${LIGNUMCAD_IMAGES}
)

target_compile_definitions(lignumCAD PRIVATE
  OGLFT_NO_SOLID
)

target_include_directories(lignumCAD PRIVATE
  ${CMAKE_CURRENT_BINARY_DIR}/generated
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/lcfilechooser
  ${CMAKE_CURRENT_SOURCE_DIR}/lcfontchooser
  ${CMAKE_CURRENT_SOURCE_DIR}/lccolorchooser
  ${CMAKE_CURRENT_SOURCE_DIR}/lcdefaultfilechooser
  ${CMAKE_CURRENT_SOURCE_DIR}/lcdefaultcombobox
  ${CMAKE_CURRENT_SOURCE_DIR}/lcdefaultlengthspinbox
  ${CMAKE_CURRENT_SOURCE_DIR}/lcdefaultratiospinbox
  ${CMAKE_CURRENT_SOURCE_DIR}/lcconstraintchooser
  ${CMAKE_CURRENT_SOURCE_DIR}/lcdefaultspinbox
  ${CMAKE_CURRENT_SOURCE_DIR}/lcdefaultlengthconstraint
  ${OpenCASCADE_INCLUDE_DIR}
)

target_link_libraries(lignumCAD PRIVATE
  Freetype::Freetype
  Fontconfig::Fontconfig
  OpenGL::GL
  ${LIGNUMCAD_GLU_TARGET}
  ZLIB::ZLIB
  Qt6::Widgets
  Qt6::OpenGL
  Qt6::OpenGLWidgets
  Qt6::Xml
  Qt6::PrintSupport
  Qt6::Svg
  ${LIGNUMCAD_GL2PS_TARGET}
  ${LIGNUMCAD_OPENCASCADE_LIBRARIES}
)

install(TARGETS lignumCAD
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(DIRECTORY materials
  DESTINATION ${LIGNUMCAD_INSTALL_DATA_DIR}
)

install(DIRECTORY doc
  DESTINATION ${LIGNUMCAD_INSTALL_DATA_DIR}
)

install(DIRECTORY images
  DESTINATION ${LIGNUMCAD_INSTALL_DATA_DIR}
)

install(FILES
  lignumCAD_en.qm
  lignumCAD_de.qm
  qt_de.qm
  DESTINATION ${LIGNUMCAD_INSTALL_DATA_DIR}/translations
)
