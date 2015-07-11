/* -*- c++ -*-
 * assemblyconstraint.h
 *
 * Header for the AssemblyConstraint class
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
#ifndef ASSEMBLYCONSTRAINT_H
#define ASSEMBLYCONSTRAINT_H

#include <gp_Ax2.hxx>

#include <qstringlist.h>

#include <memory>

class Subassembly;
class Handle( Geom_Surface );

//! Possible statuses as a result of adding a surface to the current constraint
typedef enum {
  OK,				//!< Surface 0 OK, continue to pick them.
  Invalid,			//!< Surface Not OK, continue to pick them.
  ConstraintComplete,		//!< Current constraint is complete.
  PlacementComplete		//!< The current constraint is complete and the
				//!< placement of the subassembly is completely
				//!< constrained.
} AssemblyConstraintStatus;

class AssemblyConstraint;
/*!
 * Well, we really don't know which surfaces the user is going to pick
 * when the constraint type is selected. Not sure how much this interface
 * is going to intertwine with the specific types of constraints.
 */
struct SurfacePair {
  SurfacePair ( const AssemblyConstraint* parent )
    : parent_( parent )
  {}
  virtual ~SurfacePair ( void ) {}
  virtual SurfacePair* clone ( void ) const = 0;
#if 0
  virtual AssemblyConstraintStatus addSurface( const QVector<uint>& surface_id ) = 0;
#endif
  virtual QVector<uint> reference0 ( void ) const = 0;
  virtual QVector<uint> reference1 ( void ) const = 0;

  virtual double offset ( void ) const = 0;
  virtual Space3D::Point end0 ( void ) const = 0;
  virtual Space3D::Point end1 ( void ) const = 0;
  virtual Space3D::Vector normal ( void ) const = 0;

  virtual AssemblyConstraintStatus validate0 ( const QVector<uint>& surface0_id ) = 0;
  virtual AssemblyConstraintStatus validate1 ( const QVector<uint>& surface1_id ) = 0;
  virtual void transform ( void ) = 0;
  virtual gp_Ax2 characteristic ( void ) const = 0;
  virtual void recompute ( void ) = 0;
  virtual void removeLastReference ( void ) = 0;
  virtual void setOffset ( double offset ) = 0;
  virtual void updateOffset ( double offset ) = 0;
  virtual void updateDimension ( void ) = 0;
  virtual void write ( QDomElement& xml_rep ) const = 0;
protected:
  const AssemblyConstraint* parent_;
};

/*!
 * This interface describes a constraint applied to two subassemblies
 * in an assembly. Should do two (three?) things: validate that the
 * constrained surfaces are comensurate and that their orientations
 * are suitable at this stage of the placement, and compute the
 * transformation of the new subassembly.
 */
class AssemblyConstraint {
public:
  /*!
   * \param subassembly parent subassembly of constraint.
   * \param characteristic characteristic coordinate system of the previous
   * constraint. Give gp::XOY for the (arbitrary) first constraint.
   */
  AssemblyConstraint ( Subassembly* subassembly, const gp_Ax2& characteristic )
    : subassembly_( subassembly ), characteristic_( characteristic ),
      surfaces_( 0 )
  {}
  //! Release the surfaces structure (if we have one).
  virtual ~AssemblyConstraint ( void ) { if ( surfaces_ != 0 ) delete surfaces_; }
  //! \return a string representing the type of the constraint.
  virtual QString type ( void ) const = 0;
  //! \return the phase during which this constraint is made.
  virtual int phase ( void ) const = 0;
  //! \return a deep copy of this object.
  virtual AssemblyConstraint* clone ( void ) const = 0;
  //! \return the model reference.
  Model* model ( void ) const;
  //! \return the subassembly reference.
  Subassembly* subassembly ( void ) const { return subassembly_; }
  //! \return a string representing the constraint's first reference (may
  //! or may not be defined).
  QVector<uint> reference0 ( void ) const {
    if ( surfaces_ != 0 )
      return surfaces_->reference0();
    return QVector<uint>();
  }
  //! \return a string representing the constraint's second reference (may
  //! or may not be defined).
  QVector<uint> reference1 ( void ) const
  {
    if ( surfaces_ != 0 )
      return surfaces_->reference1();
    return QVector<uint>();
  }
  //! \return the offset of the constraint.
  double offset ( void ) const
  {
    if ( surfaces_ != 0 )
      return surfaces_->offset();
    return 0;
  }
  /*!
   * If a surface pair is defined, then this constraint phase will have a
   * characteristic direction which subsequent constraints will have
   * to obey. For now, this appears adequate...
   */
  gp_Ax2 phaseCharacteristic ( void ) const
  {
    if ( surfaces_ != 0 )
      return surfaces_->characteristic();
    return gp_Ax2();
  }
  //! \return the characteristic of the previous phase.
  gp_Ax2 characteristic ( void ) const { return characteristic_; }
#if 0
  /*!
   * Attempt to add a surface to this constraint. Return value indicates
   * whether the attempt was successful and whether to keep adding surfaces.
   * \param surface_id id path to surface.
   */
  virtual AssemblyConstraintStatus addSurface ( const QVector<uint>&
						surface_id ) = 0;
#else
  virtual AssemblyConstraintStatus validate ( const QVector<uint>& surface_id ) = 0;
#endif
  /*!
   * The surface pair has been validated; now perform the transformation of
   * the subassembly.
   */
  void transform ( void ) const
  {
    if ( surfaces_ != 0 )
      surfaces_->transform();
  }
  /*!
   * Examine the XML representation and update the constraint to its
   * given state. Used for undo mostly, I guess.
   * \param xml_rep XML representation of a partial constraint.
   */
  virtual void update ( const QDomElement& xml_rep ) = 0;
  /*!
   * Remove the first selected surface.
   */
  void removeFirstReference ( void );
  /*!
   * Remove the last selected surface. Constraint is now incomplete.
   */
  void removeLastReference ( void );
  /*!
   * Some constraints have an extra offset. Set that here. Note that
   * this method just stores the value of the new offset; it does not
   * update the transformation of the subassembly.
   * \param offset new offset.
   */
  void setOffset ( double offset ) { surfaces_->setOffset( offset ); }
  /*!
   * Some constraints have an extra offset. Set that here. Note that
   * this method immediately updates the transformation of the subassembly.
   * \param offset new offset.
   */
  void updateOffset ( double offset ) { surfaces_->updateOffset( offset ); }
  //! \return the first end point of a dimension to represent this constraint
  //! (if it is an offset constraint).
  Space3D::Point end0 ( void ) const;
  //! \return the second end point of a dimension to represent this constraint
  //! (if it is an offset constraint).
  Space3D::Point end1 ( void ) const;
  //! \return the reference normal of a dimension to represent this constraint
  //! (if it is an offset constraint).
  Space3D::Vector normal ( void ) const;
  /*!
   * If the solid is modified, then we have to recompute our location.
   * \param characteristic previous constraint's characteristic.
   */
  void recompute ( const gp_Ax2& characteristic )
  {
    characteristic_ = characteristic;
    if ( surfaces_ != 0 )
      surfaces_->recompute();
  }
  /*!
   * Subsequent constraints will move the references around, so the dimension
   * end points have to more as well.
   */
  void updateDimension ( void );
  /*!
   * Serialize constraint to XML representation.
   * \param xml_rep Manager XML representation to append to.
   */
  void write ( QDomElement& xml_rep ) const;

protected:
  //! The subassembly being constrained.
  Subassembly* subassembly_;
  //! The characteristic locator of the PREVIOUS constraint.
  gp_Ax2 characteristic_;
  //! The actual pair of surfaces involved in the constraint.
  SurfacePair* surfaces_;
};


/*!
 * This class controls the  constraints made as an
 * assembly is constructed. The activity required for any constraint
 * will be specific to the selected surfaces. Try to provide an
 * interface for incrementally accepting constraints.
 */
class AssemblyConstraintManager {
public:
  /*!
   * The constraint manager has a reference to its parent subassembly
   * so that it can notify the subassembly when constaints are changed.
   * \param parent parent subassembly.
   */
  AssemblyConstraintManager ( Subassembly* parent )
    : parent_( parent )
  {
  }
  //! \return a pointer to the current constraint.
  const AssemblyConstraint* current ( void ) const { return currentConstraint; }
  /*!
   * Return a pointer to the constraint at the given phase. (Should not
   * change the "current" constraint.)
   * \param phase pick the phase-th constraint.
   * \return the specified constraint.
   */
  AssemblyConstraint* constraint ( uint phase ) const;

  //! \return an iterator over the defined constaints.
  QListIterator< std::shared_ptr<AssemblyConstraint>> constraints ( void ) const
  { return QListIterator<std::shared_ptr<AssemblyConstraint>>( constraints_ ); }
  int getCount() { return constraints_.size(); }
  //! \return the status of the current constraint.
  AssemblyConstraintStatus status ( void ) const;
  /*!
   * Add a new mate constraint and prepare to evaluate its argument surfaces
   * as they are selected.
   */
  void addMate ( void );
  /*!
   * Add a mate constraint based on the XML representation.
   * \param xml_rep XML representation of constraint.
   */
  void addMate ( const QDomElement& xml_rep );

  /*!
   * Add a new align constraint and prepare to evaluate its argument surfaces
   * as they are selected.
   */
  void addAlign ( void );
  /*!
   * Add an align constraint based on the XML representation.
   * \param xml_rep XML representation of constraint.
   */
  void addAlign ( const QDomElement& xml_rep );
  /*!
   * Add a new mate offset constraint and prepare to evaluate its argument surfaces
   * as they are selected.
   */
  void addMateOffset ( void );
  /*!
   * Add a mate offset constraint based on the XML representation.
   * \param xml_rep XML representation of constraint.
   */
  void addMateOffset ( const QDomElement& xml_rep );

  /*!
   * Add a new align offset constraint and prepare to evaluate its argument surfaces
   * as they are selected.
   */
  void addAlignOffset ( void );
  /*!
   * Add an align offset constraint based on the XML representation.
   * \param xml_rep XML representation of constraint.
   */
  void addAlignOffset ( const QDomElement& xml_rep );
  /*!
   * Apply the new value of the offset to the current constraint (may
   * be ignored if not an offset-style constraint).
   * \param offset new offset.
   */
  void setOffset ( double offset );
  /*!
   * Apply the new value of the offset to the given constraint (may
   * be ignored if not an offset-style constraint).
   * \param phase phase to set the offset for.
   * \param offset new offset.
   */
  void setOffset ( uint phase, double offset );
  /*!
   * Validate the surface against the current state of the constraint.
   * \param surface_id id path to selected surface.
   */
  AssemblyConstraintStatus validate ( const QVector<uint>& surface_id );
  /*!
   * We seem to need to separate the validation from the actual application
   * of the surface to the current constraint. This is so the constraintChanged
   * signal can be delivered at the appropriate time, namely, after the
   * user releases the mouse button. This function should probably be called
   * apply0 since it only applies to the first constraint reference. The
   * transform function below does a similiar thing for the second reference.
   */
  void apply ( void );
  /*!
   * We seem to need to separate validation from the actual application
   * of the surface to the current constaint. Calling this function
   * causes the current constraint to compute the transformation of the
   * subassembly by the current constraint; the constraintChanged signal
   * is then emitted. This is to make the mouse release complete the constraint
   * rather than the mouse press (but the correct surface must be under the
   * mouse when it is pressed; thus the division of responsibility).
   */
  void transform ( void );
  /*!
   * For every action there is an equal and opposite reaction.
   * Can the current constraint altogether.
   */
  void cancelCurrent ( void );
  /*!
   * Remove the first reference from the current constraint.
   * Perhaps this should be "clear the constraint", since there should be
   * no more references.
   */
  void removeFirstReference ( void );
  /*!
   * Remove the last reference from the last constraint. Assumes
   * that the last constraint has been completed but is still
   * the "current" entry in the constraint list.
   */
  void removeLastReference ( void );
  /*!
   * If the solid is modified, then we have to recompute our location.
   */
  void recompute ( void );
  /*!
   * Serialize constraints to XML representation.
   * \param xml_rep subassembly XML representation to append to.
   */
  void write ( QDomElement& xml_rep ) const;
  /*!
   * Unserialize a constraint from the XML representation.
   * \param xml_rep XML representation of a constraint.
   */
  void addConstraint ( const QDomElement& xml_rep );
  /*!
   * Update the current constraint from the XML representation.
   * \param xml_rep XML representation of a constraint.
   */
  void updateConstraint ( const QDomElement& xml_rep );
  /*!
   * Unserialize the constraints from the XML representation.
   * \param xml_rep XML representation of the constraints.
   */
  void addConstraints ( const QDomElement& xml_rep );
private:
  //! There are some methods of the parent we need to invoke.
  Subassembly* parent_;
  //! List of constraints defining this subassembly.
  QList<std::shared_ptr<AssemblyConstraint>> constraints_;
  //! Current constraint before change.
  AssemblyConstraint* old_constraint_;
  //TODO set this
  AssemblyConstraint* currentConstraint;
};

#endif // ASSEMBLYCONSTRAINT_H
