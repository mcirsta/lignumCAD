/* -*- c++ -*-
 * material.h
 *
 * Header for the Material classes
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
#ifndef MATERIAL_H
#define MATERIAL_H

#include <qimage.h>
#include <qcolor.h>

class Material;
class QDomElement;

class MaterialDatabase {
public:
  static MaterialDatabase& instance ( void );
  /*!
   * Add the given material to the database.
   * \param material reference to the material. The database takes responsiblity
   * for the memory.
   */
  void insertMaterial ( Material* material );
  /*!
   * Retrieve the material record for the material with given LOCALIZED
   * common name.
   * \param name localized common name of material to retrieve.
   * \return selected material record.
   */
  Material* material ( const QString& name );
  /*!
   * Retrieve the material record for the material with standardized
   * common name.
   * \param name standardized common name of material to retrieve.
   * \return selected material record.
   */
  Material* materialCommon ( const QString& name );
  //! \return an iterator over all material records.
  QDictIterator<Material> materials ( void ) const
  { return QDictIterator<Material>( materials_ ); }
private:
  static MaterialDatabase material_database_;

  QDict< Material > materials_;
protected:
  MaterialDatabase ( void );
  //  ~MaterialDatabase ( void );
};

class Material {
public:
  Material ( const QDomElement& xml_rep );
  //! \return the (unique) common name of the material.
  QString commonName ( void ) const { return common_name_; }
  //! \return the class of the material.
  QString materialClass( void ) const { return class_; }
  //! \return the (common) name of the material.
  QString name ( void ) const { return name_; }
  //! \return the botanical name of the material (if it has one) formatted
  //! as HTML.
  QString botanical ( void ) const { return botanical_; }
  //! \return a list of other (usually local) names.
  QString otherNames ( void ) const { return other_names_; }
  //! \return the specific gravity of the material.
  float specificGravity ( void ) const { return specific_gravity_; }
  //! \return the compression strenth for fiber stress at proportional limit
  //! parallel to grain (PSI).
  float compStrengthFSPLPar ( void ) const { return cmp_fspl_l_; }
  //! \return the compression strenth for maximum crushing strength
  //! parallel to grain (PSI).
  float compStrengthMCSPar ( void ) const { return cmp_mcs_l_; }
  //! \return the compression strenth for fiber stress at proportional limit
  //! perpendicular to grain (PSI).
  float compStrengthFSPLPerp ( void ) const { return cmp_fspl_r_; }
  //! \return the tension strenth for maximum tensile strength
  //! perpendicular to grain (PSI).
  float tenStrengthMTSPerp ( void ) const { return ten_mts_r_; }
  //! \return the shear strenth for maximum shear strength
  //! parallel to grain (PSI).
  float shearStrengthMSSPar ( void ) const { return shr_mss_l_; }
  //! \return static bending strength for fiber stress at proportional limit (PSI)
  float bendFSPL ( void ) const { return bend_fspl_; }
  //! \return static bending strength for modulus of rupture (PSI)
  float bendMR ( void ) const { return bend_mr_; }
  //! \return static bending strength for modulus of elasticity (10E6 PSI)
  float bendE ( void ) const { return bend_e_; }
  //! \return solid color of material.
  QColor color ( void ) const { return color_; }
  //! \return the face grain texture image file name.
  QString faceGrainFile ( void ) const { return face_grain_file_; }
  //! \return the end grain texture image file name.
  QString endGrainFile ( void ) const { return end_grain_file_; }
  //! \return the edge grain texture image name.
  QString edgeGrainFile ( void ) const { return edge_grain_file_; }

private:
  QString common_name_;
  QString class_;
  QString name_;
  QString botanical_;
  QString other_names_;
  float specific_gravity_;
  float cmp_fspl_l_;
  float cmp_mcs_l_;
  float cmp_fspl_r_;
  float ten_mts_r_;
  float shr_mss_l_;
  float bend_fspl_;
  float bend_mr_;
  float bend_e_;
  QColor color_;
  QString face_grain_file_;
  QString end_grain_file_;
  QString edge_grain_file_;
};

#endif // MATERIAL_H
