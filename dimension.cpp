/*
 * dimension.cpp
 *
 * Dimension classes
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
#include "OGLFT.h"
#include "ratio.h"
#include "handle.h"
#include "dimension.h"

#include "openglview.h"

namespace Space2D {
  Dimension::Dimension ( OpenGLBase* view )
    : view_( view )
  {
    init();
  }

  Dimension::Dimension ( const Point& end0, const Point& end1, const Vector& normal,
			 OpenGLBase* view, const QString& note )
    : end0_( end0 ), end1_( end1 ), normal_( normal ), view_( view ), note_( note )
  {
    init();
    computeLayout();
  }

  void Dimension::init ( void )
  {
    extension_offset_ = 0.;
    dimension_offset_ = 0.;

    dimension_value_.placement = INTERIOR;

    extension_lines_[END0].use_ = true;
    extension_lines_[END1].use_ = true;

    dimension_value_.face_data_.horizontal_justification_ = lC::CENTER;
    dimension_value_.face_data_.vertical_justification_ = lC::MIDDLE;
  }

  void Dimension::setEndsNormal ( const Point& end0, const Point& end1,
				  const Vector& normal, OpenGLBase* view,
				  const QString& note )
  {
    end0_ = end0;
    end1_ = end1;
    normal_ = normal;
    view_ = view;
    note_ = note;

    computeLayout();
  }

  void Dimension::setUseExtensionLine ( bool use_end0, bool use_end1 )
  {
    extension_lines_[END0].use_ = use_end0;
    extension_lines_[END1].use_ = use_end1;
  }

  void Dimension::setDimensionAttributes ( double extension_offset,
					   double dimension_offset,
					   enum Placement placement )
  {
    extension_offset_ = extension_offset;
    dimension_offset_ = dimension_offset;
    dimension_value_.placement = placement;

    computeLayout();
  }

  void Dimension::adjustExtension ( const Vector& delta )
  {
    extension_offset_ += ( delta * normal_ ) / view_->scale();

    computeLayout();
  }
  void Dimension::adjustDimension ( const Vector& delta )
  {
    extension_offset_ += ( delta * normal_ ) / view_->scale();

    Vector e = normal_;
    e.rotate( -90 );
    Vector dl = end1_ - end0_;
    e = ( dl * e ) * e;
    e.normalize();
    // If the dimension text is INTERIOR, then its dimension offset is
    // stored in scale inches. Otherwise, its offset is stored in
    // paper inches. Why? As the view is rescaled, an INTERIOR
    // dimension retains its relative position between the extension
    // lines (e.g, it might be 25% of the way from END0 to END1).  On
    // the other hand, an EXTERIOR dimension text maintains the same
    // paper distance from its END (e.g. it's always 1/4" from the
    // END0 extension line).
    switch ( dimension_value_.placement ) {
    case INTERIOR:
      dimension_offset_ += delta * e;
      break;
    case EXTERIOR_END0: case EXTERIOR_END1:
      dimension_offset_ += ( delta * e ) / view_->scale();
    }

    computeLayout();
  }

  void Dimension::draw ( lC::Render::Mode mode ) const
  {
    QColor color = view_->constraintPrimaryColor();

    if ( mode == lC::Render::HIGHLIGHTED || mode == lC::Render::ACTIVATED )
      color = color.light();

    // Avoid changing dimension state so this method remains "const".
    FaceData face_data = dimension_value_.face_data_;
    face_data.color_ = color.rgb();

    view_->font(face_data)->draw( dimension_value_.position[X],
				  dimension_value_.position[Y],
				  dimension_value_.text );

    glColor3ubv( lC::qCubv( color ) );

    if ( locator_.use_ ) {
      glBegin( GL_LINES );
      glVertex2dv( locator_.from_ );
      glVertex2dv( locator_.to_ );
      glEnd();
    }

    arrow_.draw( arrows_[END1].from_, arrows_[END1].to_ );

    if ( extension_lines_[END1].use_ ) {
      glBegin( GL_LINES );

      glVertex2dv( extension_lines_[END1].from_ );
      glVertex2dv( extension_lines_[END1].to_ );

      glEnd();
    }

    if ( mode == lC::Render::HIGHLIGHTED || mode == lC::Render::ACTIVATED ) {
      color = view_->constraintSecondaryColor().light();
      glColor3ubv( lC::qCubv( color ) );
    }

    if ( extension_lines_[END0].use_ ) {
      glBegin( GL_LINES );

      glVertex2dv( extension_lines_[END0].from_ );
      glVertex2dv( extension_lines_[END0].to_ );

      glEnd();
    }

    arrow_.draw( arrows_[END0].from_, arrows_[END0].to_ );

    if ( mode == lC::Render::HIGHLIGHTED || mode == lC::Render::ACTIVATED ) {
      ResizeHandle::draw( view_, mode, arrows_[END0].to_ );
      ResizeHandle::draw( view_, mode, arrows_[END1].to_ );
    }
  }

  void Dimension::select ( lC::Render::Mode mode ) const
  {
    glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
    glPushMatrix();
    glTranslated( dimension_value_.position[X], dimension_value_.position[Y], 0 );
    glRotated( dimension_value_.face_data_.orientation_, 0, 0, 1 );
    glPushName( 1 );
    glRectdv( -0.5 * dimension_value_.bbox, 0.5 * dimension_value_.bbox );
    glPopName();
    glPopMatrix();

    arrow_.select( arrows_[END0].from_, arrows_[END0].to_ );
    arrow_.select( arrows_[END1].from_, arrows_[END1].to_ );

    glBegin( GL_LINES );

    if ( extension_lines_[END0].use_ ) {
      glVertex2dv( extension_lines_[END0].from_ );
      glVertex2dv( extension_lines_[END0].to_ );
    }

    if ( extension_lines_[END1].use_ ) {
      glVertex2dv( extension_lines_[END1].from_ );
      glVertex2dv( extension_lines_[END1].to_ );
    }

    if ( locator_.use_ ) {
      glVertex2dv( locator_.from_ );
      glVertex2dv( locator_.to_ );
    }

    glEnd();

    if ( mode == lC::Render::ACTIVATED ) {
      ResizeHandle::select( view_, arrows_[END0].to_ );
      ResizeHandle::select( view_, arrows_[END1].to_ );
    }
  }

  void Dimension::computeLayout ( void )
  {
    // This confuses even me. In the design documentation, there are
    // some notes about the vector computations involved here.

    // The delta vector; obviously, the vector from the first end to
    // the second end
    Vector dl = end1_ - end0_;

    // e is the unit vector in the direction of the dimension line,
    // i.e.  it is perpendicular to the extension line direction.
    Vector e = normal_;
    e.rotate( -90 );

    // The displayed value of the dimension is the projection of the
    // delta vector onto e.
    dimension_value_.value = fabs( dl * e );

    // If e is not parallel to dl, then there will be a difference in
    // the length of the extension lines. That difference is just
    // equal to the projection of the delta vector onto the extension
    // line direction.
    double extra = dl * normal_;
    double d0 = 0., d1 = 0.;

    // The sign of the extra length determines whether it applies to
    // the first or second end. Ultimately, the extension line lengths
    // are positive.
    if ( extra > 0 )
      d0 = extra;
    else
      d1 = -extra;

    // Convert the delta vector into the dimension line vector. This
    // computation, (dl*e)*e, basically picks the component of the
    // delta vector in the direction of e but it also corrects the
    // sign of e which the rotate(-90) above might have miscomputed.
    dl = ( dl * e ) * e;
    if ( dimension_value_.value != 0 ) {
      e = dl;
      e.normalize();
    }

    // Compute the metrics of the dimension string. Start by asking the
    // LengthUnit basis to format the dimension value.
    dimension_value_.text = view_->format( dimension_value_.value);

    if ( !note_.isEmpty() )
      dimension_value_.text += " " + note_;

    dimension_value_.face_data_.font_ = view_->dimensionFont();
    // Measure the value string in its unrotated state
    dimension_value_.face_data_.orientation_ = 0;

    OGLFT::BBox bbox =
      view_->font(dimension_value_.face_data_)->measure( dimension_value_.text );

    // Use (half) a zero's width as extra spacing around the value
    // in the dimension line
    bbox += view_->font(dimension_value_.face_data_)->measure( '0' );

    // A bbox vector is somewhat easier to use than the OGLFT BBox.
    dimension_value_.bbox = ( Point( bbox.x_max_, bbox.y_max_ ) -
			      Point( bbox.x_min_, bbox.y_min_ ) );

    // The actual orientation at which the text is drawn is determined
    // by the normal vector. See the programming notes for an explanation.
    double angle = rint( normal_.orientation() );
    if ( angle < 0 ) angle += 360.;

    double text_rotation;
    if ( angle > 0 && angle <= 180. )
      text_rotation = angle - 90.;
    else
      text_rotation = angle + 90.;

    dimension_value_.face_data_.orientation_ = (uint)text_rotation;

    // Now I think we have enough information to compute the length of
    // each extension line, which is the height of the dimension text,
    // plus a bit of clearance at either end, plus any unevenness in
    // the reference line alignments, plus any aesthetic modification
    // made by the user.

    dimension_line_.clearance = view_->clearance();

    double end0_length = dimension_value_.bbox[Y] +
      view_->scale() * ( 2. * dimension_line_.clearance + extension_offset_ ) +
      d0;
    double end1_length = dimension_value_.bbox[Y] +
      view_->scale() * ( 2. * dimension_line_.clearance + extension_offset_ ) +
      d1;

    extension_lines_[END0].offset = extension_lines_[END1].offset
      = view_->extensionLineOffset();

    extension_lines_[END0].from_ = end0_ +
      view_->scale() * extension_lines_[END0].offset * normal_;
    extension_lines_[END1].from_ = end1_ +
      view_->scale() * extension_lines_[END1].offset * normal_;
    extension_lines_[END0].to_ =
      extension_lines_[END0].from_ + end0_length * normal_;
    extension_lines_[END1].to_ =
      extension_lines_[END1].from_ + end1_length * normal_;

    arrows_[END0].to_ = extension_lines_[END0].to_ -
      ( view_->scale() * dimension_line_.clearance + dimension_value_.bbox[Y]/2.) *
      normal_;
    arrows_[END1].to_ = extension_lines_[END1].to_ -
      ( view_->scale() * dimension_line_.clearance + dimension_value_.bbox[Y]/2.) *
      normal_;

    // The size of an arrow head depends on the scale of the OpenGL view,
    // so adjust that accordingly.
    double arrow_head_length = view_->arrowHeadLength() * view_->scale();

    arrow_.setLength( arrow_head_length );
    arrow_.setStyle( view_->arrowHeadStyle() );
    arrow_.setWidthRatio( view_->arrowHeadWidthRatio() );

    locator_.use_ = false;

    // Now we have to fit the dimension text and choose a position

    if ( dimension_value_.bbox[X] + 2.*arrow_.length() <  dimension_value_.value ) {

      switch ( dimension_value_.placement ) {
      case INTERIOR:
	if ( fabs( dimension_offset_ ) <= dimension_value_.value / 2. ) {

	  if ( fabs( dimension_offset_ ) <=
	       ( ( dimension_value_.value - dimension_value_.bbox[X] ) / 2 -
		 arrow_.length() ) ) {
	    // Interior dimension, interior arrows
	    arrows_[END0].from_ = arrows_[END0].to_ +
	      0.5 * ( dl - ( dimension_value_.bbox[X] - 2. * dimension_offset_ )*e);
	    arrows_[END1].from_ = arrows_[END1].to_ -
	      0.5 * ( dl - ( dimension_value_.bbox[X] + 2. * dimension_offset_ )*e);

	    dimension_value_.position = arrows_[END0].to_ + 0.5 * dl +
	      dimension_offset_ * e;
	  }
	  else if ( dimension_offset_ < 0 ) {
	    arrows_[END0].from_ = arrows_[END0].to_ + arrow_.length() * e;
	    arrows_[END1].from_ = arrows_[END1].to_ -
	      ( dl - ( dimension_value_.bbox[X] + arrow_.length() ) * e );

	    dimension_value_.position = arrows_[END0].to_ +
	      ( arrow_.length() + 0.5 * dimension_value_.bbox[X] ) * e;
	  }
	  else {
	    arrows_[END0].from_ = arrows_[END0].to_ + 
	      ( dl - ( dimension_value_.bbox[X] + arrow_.length() ) * e );
	    arrows_[END1].from_ = arrows_[END1].to_ - arrow_.length() * e;

	    dimension_value_.position = arrows_[END1].to_ -
	      ( arrow_.length() + 0.5 * dimension_value_.bbox[X] ) * e;
	  }
	}
	else {
	  // Exterior dimension, interior arrows
	  if ( dimension_offset_ < 0 ) {
	    dimension_value_.placement = EXTERIOR_END0;
	    dimension_offset_ = 0.;

	    dimension_value_.position = arrows_[END0].to_ -
	      0.5 * dimension_value_.bbox[X] * e;
	  }
	  else {
	    dimension_value_.placement = EXTERIOR_END1;
	    dimension_offset_ = 0.;

	    dimension_value_.position = arrows_[END1].to_ +
	      0.5 * dimension_value_.bbox[X] * e;
	  }
	  // The arrows fill the interior.
	  arrows_[END0].from_ = arrows_[END0].to_ + 0.5 * dl;
	  arrows_[END1].from_ = arrows_[END1].to_ - 0.5 * dl;
	}
	break;
      case EXTERIOR_END0:
	if ( dimension_offset_ <= 0 ) {
	  // The arrows fill the interior.
	  arrows_[END0].from_ = arrows_[END0].to_ + 0.5 * dl;
	  arrows_[END1].from_ = arrows_[END1].to_ - 0.5 * dl;

	  dimension_value_.position = arrows_[END0].to_ -
	    ( 0.5 * dimension_value_.bbox[X] -
	      view_->scale() * dimension_offset_ ) * e;
	  locator_.use_ = true;
	  locator_.to_ = arrows_[END0].to_;
	  locator_.from_ = locator_.to_ + view_->scale() * dimension_offset_ * e;
	}
	else {
	  // If the dimension text switches back to an INTERIOR position,
	  // align it with END0 arrow.
	  dimension_value_.placement = INTERIOR;
	  dimension_offset_ = -dimension_value_.value / 2;

	  arrows_[END0].from_ = arrows_[END0].to_ +
	    0.5 * ( dl - ( dimension_value_.bbox[X] - 2. * dimension_offset_ )*e);
	  arrows_[END1].from_ = arrows_[END1].to_ -
	    0.5 * ( dl - ( dimension_value_.bbox[X] + 2. * dimension_offset_ )*e);

	  dimension_value_.position = arrows_[END0].to_ + 0.5 * dl +
	    dimension_offset_ * e;
	}
	break;
      case EXTERIOR_END1:
	if ( dimension_offset_ >= 0 ) {
	  // The arrows fill the interior.
	  arrows_[END0].from_ = arrows_[END0].to_ + 0.5 * dl;
	  arrows_[END1].from_ = arrows_[END1].to_ - 0.5 * dl;

	  dimension_value_.position = arrows_[END1].to_ +
	    ( 0.5 * dimension_value_.bbox[X] +
	      view_->scale() * dimension_offset_ ) * e;

	  locator_.use_ = true;
	  locator_.to_ = arrows_[END1].to_;
	  locator_.from_ = locator_.to_ + view_->scale() * dimension_offset_ * e;
	}
	else {
	  // If the dimension text switches back to an INTERIOR position,
	  // align it with END0 arrow.
	  dimension_value_.placement = INTERIOR;
	  dimension_offset_ = dimension_value_.value / 2;

	  arrows_[END0].from_ = arrows_[END0].to_ +
	    0.5 * ( dl - ( dimension_value_.bbox[X] - 2. * dimension_offset_ )*e);
	  arrows_[END1].from_ = arrows_[END1].to_ -
	    0.5 * ( dl - ( dimension_value_.bbox[X] + 2. * dimension_offset_ )*e);

	  dimension_value_.position = arrows_[END0].to_ + 0.5 * dl +
	    dimension_offset_ * e;
	}
	break;
      }
    }
    else if ( dimension_value_.bbox[X] < dimension_value_.value ) {
      // Interior dimension, exterior arrows: assume twice the
      // arrow head length for length of arrow
      arrows_[END0].from_ =
	arrows_[END0].to_ - 2. * arrow_.length() * e;
      arrows_[END1].from_ =
	arrows_[END1].to_ + 2. * arrow_.length() * e;

      dimension_value_.position = arrows_[END0].to_ + 0.5 * dl;
    }
    else {
      // Exterior dimension, exterior arrows: assume twice the
      // arrow head length for length of arrow
      arrows_[END0].from_ =
	arrows_[END0].to_ - 2. * arrow_.length() * e;
      arrows_[END1].from_ =
	arrows_[END1].to_ + 2. * arrow_.length() * e;
      // The end at which the dimension shows up should be selectable
      // by the user...for now we'll try to keep it to the left/below
      // (This is not wholly accurate...)
      if ( dl * e >= 0 )
	dimension_value_.position = arrows_[END0].from_ -
	  0.5 * dimension_value_.bbox[X] * e;
      else
	dimension_value_.position = arrows_[END1].from_ -
	  0.5 * dimension_value_.bbox[X] * e;
    }
  }
} // End of Space2D namespace

namespace Space3D {
  Dimension::Dimension ( OpenGLBase* view )
    : view_( view ), mode_( lC::Render::INVISIBLE )
  {
    init();
  }

  Dimension::Dimension ( const Point& end0, const Point& end1, const Vector& normal,
			 OpenGLBase* view, const QString& note )
    : end0_( end0 ), end1_( end1 ), normal_( normal ), view_( view ), note_( note ),
      mode_( lC::Render::INVISIBLE )
  {
    init();
    computeLayout();
  }

  void Dimension::init ( void )
  {
    extension_offset_ = 0.;
    dimension_offset_ = 0.;

    dimension_value_.placement = INTERIOR;

    extension_lines_[END0].use_ = true;
    extension_lines_[END1].use_ = true;

    dimension_value_.face_data_.horizontal_justification_ = lC::CENTER;
    dimension_value_.face_data_.vertical_justification_ = lC::MIDDLE;
  }

  void Dimension::setEndsNormal ( const Point& end0, const Point& end1,
				  const Vector& normal, OpenGLBase* view,
				  const QString& note )
  {
    end0_ = end0;
    end1_ = end1;
    normal_ = normal;
    view_ = view;
    note_ = note;

    computeLayout();
  }

  void Dimension::setUseExtensionLine ( bool use_end0, bool use_end1 )
  {
    extension_lines_[END0].use_ = use_end0;
    extension_lines_[END1].use_ = use_end1;
  }

  void Dimension::setDimensionAttributes ( double extension_offset,
					   double dimension_offset,
					   enum Placement placement )
  {
    extension_offset_ = extension_offset;
    dimension_offset_ = dimension_offset;
    dimension_value_.placement = placement;

    computeLayout();
  }

  void Dimension::setMode ( lC::Render::Mode mode )
  {
    mode_ = mode;
  }

  void Dimension::setViewNormal ( const Vector& view_normal )
  {
    arrow_.setViewNormal( view_normal );

    dimension_value_.bb_ll_ =
      view_->unproject3D( Space2D::Point( dimension_value_.bb_min_[X],
					  dimension_value_.bb_min_[Y] ) );
    dimension_value_.bb_lr_ =
      view_->unproject3D( Space2D::Point( dimension_value_.bb_max_[X],
					  dimension_value_.bb_min_[Y] ) );
    dimension_value_.bb_ur_ =
      view_->unproject3D( Space2D::Point( dimension_value_.bb_max_[X],
					  dimension_value_.bb_max_[Y] ) );
    dimension_value_.bb_ul_ =
      view_->unproject3D( Space2D::Point( dimension_value_.bb_min_[X],
					  dimension_value_.bb_max_[Y] ) );
  }

  void Dimension::adjustExtension ( const Vector& delta )
  {
    extension_offset_ += ( delta * normal_ ) / view_->scale();

    computeLayout();
  }
  void Dimension::adjustDimension ( const Vector& delta )
  {
    extension_offset_ += ( delta * normal_ ) / view_->scale();

    Vector dl = end1_ - end0_;
    Vector e = normal_;
    Vector z = e.cross( dl );
    e.rotate( -90, z );
    e = ( dl * e ) * e;
    e.normalize();
    // If the dimension text is INTERIOR, then its dimension offset is
    // stored in scale inches. Otherwise, its offset is stored in
    // paper inches. Why? As the view is rescaled, an INTERIOR
    // dimension retains its relative position between the extension
    // lines (e.g, it might be 25% of the way from END0 to END1).  On
    // the other hand, an EXTERIOR dimension text maintains the same
    // paper distance from its END (e.g. it's always 1/4" from the
    // END0 extension line).
    switch ( dimension_value_.placement ) {
    case INTERIOR:
      dimension_offset_ += delta * e;
      break;
    case EXTERIOR_END0: case EXTERIOR_END1:
      dimension_offset_ += ( delta * e ) / view_->scale();
    }

    computeLayout();
  }

  void Dimension::draw ( void ) const
  {
    if ( mode_ == lC::Render::INVISIBLE ) return;

    QColor color = view_->constraintPrimaryColor();

    if ( mode_ == lC::Render::HIGHLIGHTED || mode_ == lC::Render::ACTIVATED )
      color = color.light();

    // Avoid changing dimension state so this method remains "const".
    FaceData face_data = dimension_value_.face_data_;
    face_data.color_ = color.rgb();

    view_->font(face_data)->draw( dimension_value_.position[X],
				  dimension_value_.position[Y],
				  dimension_value_.position[Z],
				  dimension_value_.text );

    glColor3ubv( lC::qCubv( color ) );

    if ( locator_.use_ ) {
      glBegin( GL_LINES );
      glVertex3dv( locator_.from_ );
      glVertex3dv( locator_.to_ );
      glEnd();
    }

    arrow_.draw( arrows_[END1].from_, arrows_[END1].to_ );

    if ( extension_lines_[END1].use_ ) {
      glBegin( GL_LINES );

      glVertex3dv( extension_lines_[END1].from_ );
      glVertex3dv( extension_lines_[END1].to_ );

      glEnd();
    }

    if ( mode_ == lC::Render::HIGHLIGHTED || mode_ == lC::Render::ACTIVATED ) {
      color = view_->constraintSecondaryColor().light();
      glColor3ubv( lC::qCubv( color ) );
    }

    if ( extension_lines_[END0].use_ ) {
      glBegin( GL_LINES );

      glVertex3dv( extension_lines_[END0].from_ );
      glVertex3dv( extension_lines_[END0].to_ );

      glEnd();
    }

    arrow_.draw( arrows_[END0].from_, arrows_[END0].to_ );

    if ( mode_ == lC::Render::HIGHLIGHTED || mode_ == lC::Render::ACTIVATED ) {
      ResizeHandle::draw( view_, mode_, arrows_[END0].to_ );
      ResizeHandle::draw( view_, mode_, arrows_[END1].to_ );
    }
  }

  void Dimension::select ( void ) const
  {
    if ( mode_ == lC::Render::INVISIBLE ) return;

    glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

    glPushMatrix();
    // Drawn position.
    glTranslated( dimension_value_.position[X], dimension_value_.position[Y],
		  dimension_value_.position[Z] );
    // Adjust for centering.
    Point center = dimension_value_.bb_ll_ +
      0.5 * ( dimension_value_.bb_ur_ - dimension_value_.bb_ll_ );
    glTranslated( -center[X], -center[Y], -center[Z] );

    glPushName( 1 );
    
    glBegin( GL_QUADS );
    glVertex3dv( dimension_value_.bb_ll_ );
    glVertex3dv( dimension_value_.bb_lr_ );
    glVertex3dv( dimension_value_.bb_ur_ );
    glVertex3dv( dimension_value_.bb_ul_ );
    glEnd();

    glPopName();
    glPopMatrix();

    arrow_.select( arrows_[END0].from_, arrows_[END0].to_ );
    arrow_.select( arrows_[END1].from_, arrows_[END1].to_ );

    glBegin( GL_LINES );

    if ( extension_lines_[END0].use_ ) {
      glVertex3dv( extension_lines_[END0].from_ );
      glVertex3dv( extension_lines_[END0].to_ );
    }

    if ( extension_lines_[END1].use_ ) {
      glVertex3dv( extension_lines_[END1].from_ );
      glVertex3dv( extension_lines_[END1].to_ );
    }

    if ( locator_.use_ ) {
      glVertex3dv( locator_.from_ );
      glVertex3dv( locator_.to_ );
    }

    glEnd();

    if ( mode_ == lC::Render::ACTIVATED ) {
      ResizeHandle::select( view_, arrows_[END0].to_ );
      ResizeHandle::select( view_, arrows_[END1].to_ );
    }
  }

  void Dimension::computeLayout ( void )
  {
    // This confuses even me. In the design documentation, there are
    // some notes about the vector computations involved here.

    // The delta vector; obviously, the vector from the first end to
    // the second end
    Vector dl = end1_ - end0_;

    // e is the unit vector in the direction of the dimension line,
    // i.e.  it is perpendicular to the extension line direction.
    // Note that in 3D, you need to explicitly give the direction
    // about which rotation of a vector is to take place.
    Vector e = normal_;
    Vector z = e.cross( dl );
    // If the effective separation of the two end-points is 0 (which
    // is not unreasonable), then e is arbitrary (to the extent it
    // is at least normal to dl).
    if ( fabs( z * z ) < lC::EPSILON ) {
      z = dl;
      if ( fabs( z * z ) > lC::EPSILON ) {
	z.normalize();
	if ( fabs( fabs( z * Vector( 1, 0, 0 ) - 1. ) > lC::EPSILON ) )
	  e = Vector( 1, 0, 0 );
	else
	  e = Vector( 0, 1, 0 );
      }
      else
	e = Vector( 1, 0, 0 );
    }
    else {
      e.rotate( -90, z );
      e.normalize();
    }

    // The displayed value of the dimension is the projection of the
    // delta vector onto e.
    dimension_value_.value = fabs( dl * e );

    // If e is not parallel to dl, then there will be a difference in
    // the length of the extension lines. That difference is just
    // equal to the projection of the delta vector onto the extension
    // line direction.
    double extra = dl * normal_;
    double d0 = 0., d1 = 0.;

    // The sign of the extra length determines whether it applies to
    // the first or second end. Ultimately, the extension line lengths
    // are positive.
    if ( extra > 0 )
      d0 = extra;
    else
      d1 = -extra;

    // Convert the delta vector into the dimension line vector. This
    // computation, (dl*e)*e, basically picks the component of the
    // delta vector in the direction of e but it also corrects the
    // sign of e which the rotate(-90) above might have miscomputed.
    dl = ( dl * e ) * e;
    if ( dimension_value_.value != 0 ) {
      e = dl;
      e.normalize();
    }

    // Compute the metrics of the dimension string. Start by asking the
    // LengthUnit basis to format the dimension value.
    dimension_value_.text = view_->format( dimension_value_.value);

    if ( !note_.isEmpty() )
      dimension_value_.text += " " + note_;

    dimension_value_.face_data_.font_ = view_->dimensionFont();
    // Measure the value string in its unrotated state
    dimension_value_.face_data_.orientation_ = 0;

    OGLFT::BBox bbox =
      view_->font(dimension_value_.face_data_)->measureRaw( dimension_value_.text );

    // Use (half) a zero's width as extra spacing around the value
    // in the dimension line
    bbox += view_->font(dimension_value_.face_data_)->measureRaw( "0" );

    // The bounding box size is in pixels. Convert it to the current
    // modeling coordinates. Very complicated...
    dimension_value_.bb_min_ = Space2D::Point( bbox.x_min_, bbox.y_min_ );
    dimension_value_.bb_max_ = Space2D::Point( bbox.x_max_, bbox.y_max_ );

    dimension_value_.bb_ll_ = view_->unproject3D( Space2D::Point( bbox.x_min_,
								  bbox.y_min_ ) );
    dimension_value_.bb_lr_ = view_->unproject3D( Space2D::Point( bbox.x_max_,
								  bbox.y_min_ ) );
    dimension_value_.bb_ur_ = view_->unproject3D( Space2D::Point( bbox.x_max_,
								  bbox.y_max_ ) );
    dimension_value_.bb_ul_ = view_->unproject3D( Space2D::Point( bbox.x_min_,
								  bbox.y_max_ ) );

    double text_width =
      (dimension_value_.bb_lr_ - dimension_value_.bb_ll_).length();
    double text_height =
      (dimension_value_.bb_ul_ - dimension_value_.bb_ll_).length();

    // In 3D, I guess I'll just have the text drawn horizontally all the
    // time.
#if 0
    // The actual orientation at which the text is drawn is determined
    // by the normal vector. See the programming notes for an explanation.
    double angle = rint( normal_.orientation() );
    if ( angle < 0 ) angle += 360.;

    double text_rotation;
    if ( angle > 0 && angle <= 180. )
      text_rotation = angle - 90.;
    else
      text_rotation = angle + 90.;

    dimension_value_.face_data_.orientation_ = (uint)text_rotation;
#endif
    // Now I think we have enough information to compute the length of
    // each extension line, which is the height of the dimension text,
    // plus a bit of clearance at either end, plus any unevenness in
    // the reference line alignments, plus any aesthetic modification
    // made by the user.

    dimension_line_.clearance = view_->clearance();

    double end0_length = text_height +
      view_->scale() * ( 2. * dimension_line_.clearance + extension_offset_ ) +
      d0;
    double end1_length = text_height +
      view_->scale() * ( 2. * dimension_line_.clearance + extension_offset_ ) +
      d1;

    extension_lines_[END0].offset = extension_lines_[END1].offset
      = view_->extensionLineOffset();

    extension_lines_[END0].from_ = end0_ +
      view_->scale() * extension_lines_[END0].offset * normal_;
    extension_lines_[END1].from_ = end1_ +
      view_->scale() * extension_lines_[END1].offset * normal_;
    extension_lines_[END0].to_ =
      extension_lines_[END0].from_ + end0_length * normal_;
    extension_lines_[END1].to_ =
      extension_lines_[END1].from_ + end1_length * normal_;

    arrows_[END0].to_ = extension_lines_[END0].to_ -
      ( view_->scale() * dimension_line_.clearance + text_height/2.) *
      normal_;
    arrows_[END1].to_ = extension_lines_[END1].to_ -
      ( view_->scale() * dimension_line_.clearance + text_height/2.) *
      normal_;

    // The size of an arrow head depends on the scale of the OpenGL view,
    // so adjust that accordingly.
    double arrow_head_length = view_->arrowHeadLength() * view_->scale();

    arrow_.setLength( arrow_head_length );
    arrow_.setStyle( view_->arrowHeadStyle() );
    arrow_.setWidthRatio( view_->arrowHeadWidthRatio() );

    locator_.use_ = false;

    // Now we have to fit the dimension text and choose a position


    if ( text_width + 2.*arrow_.length() <  dimension_value_.value ) {

      switch ( dimension_value_.placement ) {
      case INTERIOR:
	if ( fabs( dimension_offset_ ) <= dimension_value_.value / 2. ) {

	  if ( fabs( dimension_offset_ ) <=
	       ( ( dimension_value_.value - text_width ) / 2 -
		 arrow_.length() ) ) {
	    // Interior dimension, interior arrows
	    arrows_[END0].from_ = arrows_[END0].to_ +
	      0.5 * ( dl - ( text_width - 2. * dimension_offset_ )*e);
	    arrows_[END1].from_ = arrows_[END1].to_ -
	      0.5 * ( dl - ( text_width + 2. * dimension_offset_ )*e);

	    dimension_value_.position = arrows_[END0].to_ + 0.5 * dl +
	      dimension_offset_ * e;
	  }
	  else if ( dimension_offset_ < 0 ) {
	    arrows_[END0].from_ = arrows_[END0].to_ + arrow_.length() * e;
	    arrows_[END1].from_ = arrows_[END1].to_ -
	      ( dl - ( text_width + arrow_.length() ) * e );

	    dimension_value_.position = arrows_[END0].to_ +
	      ( arrow_.length() + 0.5 * text_width ) * e;
	  }
	  else {
	    arrows_[END0].from_ = arrows_[END0].to_ + 
	      ( dl - ( text_width + arrow_.length() ) * e );
	    arrows_[END1].from_ = arrows_[END1].to_ - arrow_.length() * e;

	    dimension_value_.position = arrows_[END1].to_ -
	      ( arrow_.length() + 0.5 * text_width ) * e;
	  }
	}
	else {
	  // Exterior dimension, interior arrows
	  if ( dimension_offset_ < 0 ) {
	    dimension_value_.placement = EXTERIOR_END0;
	    dimension_offset_ = 0.;

	    dimension_value_.position = arrows_[END0].to_ -
	      0.5 * text_width * e;
	  }
	  else {
	    dimension_value_.placement = EXTERIOR_END1;
	    dimension_offset_ = 0.;

	    dimension_value_.position = arrows_[END1].to_ +
	      0.5 * text_width * e;
	  }
	  // The arrows fill the interior.
	  arrows_[END0].from_ = arrows_[END0].to_ + 0.5 * dl;
	  arrows_[END1].from_ = arrows_[END1].to_ - 0.5 * dl;
	}
	break;
      case EXTERIOR_END0:
	if ( dimension_offset_ <= 0 ) {
	  // The arrows fill the interior.
	  arrows_[END0].from_ = arrows_[END0].to_ + 0.5 * dl;
	  arrows_[END1].from_ = arrows_[END1].to_ - 0.5 * dl;

	  dimension_value_.position = arrows_[END0].to_ -
	    ( 0.5 * text_width -
	      view_->scale() * dimension_offset_ ) * e;

	  locator_.use_ = true;
	  locator_.to_ = arrows_[END0].to_;
	  locator_.from_ = locator_.to_ + view_->scale() * dimension_offset_ * e;
	}
	else {
	  // If the dimension text switches back to an INTERIOR position,
	  // align it with END0 arrow.
	  dimension_value_.placement = INTERIOR;
	  dimension_offset_ = -dimension_value_.value / 2;

	  arrows_[END0].from_ = arrows_[END0].to_ +
	    0.5 * ( dl - ( text_width - 2. * dimension_offset_ )*e);
	  arrows_[END1].from_ = arrows_[END1].to_ -
	    0.5 * ( dl - ( text_width + 2. * dimension_offset_ )*e);

	  dimension_value_.position = arrows_[END0].to_ + 0.5 * dl +
	    dimension_offset_ * e;
	}
	break;
      case EXTERIOR_END1:
	if ( dimension_offset_ >= 0 ) {
	  // The arrows fill the interior.
	  arrows_[END0].from_ = arrows_[END0].to_ + 0.5 * dl;
	  arrows_[END1].from_ = arrows_[END1].to_ - 0.5 * dl;

	  dimension_value_.position = arrows_[END1].to_ +
	    ( 0.5 * text_width +
	      view_->scale() * dimension_offset_ ) * e;

	  locator_.use_ = true;
	  locator_.to_ = arrows_[END1].to_;
	  locator_.from_ = locator_.to_ + view_->scale() * dimension_offset_ * e;
	}
	else {
	  // If the dimension text switches back to an INTERIOR position,
	  // align it with END0 arrow.
	  dimension_value_.placement = INTERIOR;
	  dimension_offset_ = dimension_value_.value / 2;

	  arrows_[END0].from_ = arrows_[END0].to_ +
	    0.5 * ( dl - ( text_width - 2. * dimension_offset_ )*e);
	  arrows_[END1].from_ = arrows_[END1].to_ -
	    0.5 * ( dl - ( text_width + 2. * dimension_offset_ )*e);

	  dimension_value_.position = arrows_[END0].to_ + 0.5 * dl +
	    dimension_offset_ * e;
	}
	break;
      }
    }
    else if ( text_width < dimension_value_.value ) {

      // Interior dimension, exterior arrows: assume twice the
      // arrow head length for length of arrow
      arrows_[END0].from_ =
	arrows_[END0].to_ - 2. * arrow_.length() * e;
      arrows_[END1].from_ =
	arrows_[END1].to_ + 2. * arrow_.length() * e;

      dimension_value_.position = arrows_[END0].to_ + 0.5 * dl;
    }
    else {
      // Exterior dimension, exterior arrows: assume twice the
      // arrow head length for length of arrow
      arrows_[END0].from_ =
	arrows_[END0].to_ - 2. * arrow_.length() * e;
      arrows_[END1].from_ =
	arrows_[END1].to_ + 2. * arrow_.length() * e;
      // The end at which the dimension shows up should be selectable
      // by the user...for now we'll try to keep it to the left/below
      // (This is not wholly accurate...)

      if ( dl * e >= 0 )
	dimension_value_.position = arrows_[END0].from_ -
	  0.5 * text_width * e;
      else
	dimension_value_.position = arrows_[END1].from_ -
	  0.5 * text_width * e;
    }

    // Finally, get the current view normal.
    setViewNormal( view_->viewNormal() );
  }
} // End of Space3D namespace
