/* -*- c++ -*-
 * line.h
 *
 * Header for the (straight) Line classes
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
#ifndef LINE_H
#define LINE_H

#include "constants.h"
#include "constrainthistory.h"
#include "vectoralgebra.h"
#include "curve.h"

class QDomElement;

namespace Space2D {

  class Line : public Curve {
    Q_OBJECT
  protected:
    Point o_;
    Vector e_;
  public:
    Line ( uint id, const QString& name, Figure* parent )
      : Curve( id, name, lC::STR::LINE, parent )
    {}

    Line ( uint id, const QString& name, Point o, Vector e )
      : Curve( id, name, lC::STR::LINE, 0 ), o_( o ), e_( e )
    {}

    virtual ~Line ( void ) {}

    Point o ( void ) const { return o_; }
    Vector e ( void ) const { return e_; }

    Point intersect ( const Curve* c ) const;

    bool isHorizontal ( void ) const;
    bool isVertical ( void ) const;

    Vector normal ( void ) const;

    virtual bool dependsOn( const Line* line ) const = 0;
    virtual std::ostream& edit ( std::ostream& o ) const = 0;

  signals:
    void destroyedLine ();
    //    void modifiedPosition ();
  };

  std::ostream& operator<< ( std::ostream& o, Line* line );

  class Axis : public Line {
    Q_OBJECT
  public:
    Axis ( uint id, const QString& name, Point o, Vector e )
      : Line( id, name, o, e )
    {}
    ~Axis ( void ) {}

    bool dependsOn ( const Line* ) const { return false; }
    std::ostream& edit ( std::ostream& ) const;

    void write ( QDomElement& xml_rep ) const;
  };

  extern Axis X_AXIS;
  extern Axis Y_AXIS;

  class ConstrainedLine;

  class Constraint {
  public:
    virtual ~Constraint ( void ) {}
    virtual bool isAdjustable ( void ) const = 0;
    virtual bool isFree ( void ) const = 0;
    virtual Point o ( void ) const = 0;
    virtual Vector e ( void ) const = 0;
    // This seems to be the blemish on this design. Everything except
    // the Offset constraint now has an offset method.
    virtual double offset ( void ) const = 0;
    virtual Line* reference ( void ) const = 0;
    virtual Point setOffset ( double offset ) = 0;
    virtual void setReference ( const Point& position, Line* reference ) = 0;
    virtual void resolvedReference ( Line* reference ) = 0;
    virtual Point setPosition ( const Point& position ) = 0;
    virtual void referenceModified ( void ) = 0;
    virtual bool dependsOn ( const Line* ) const = 0;
    virtual std::ostream& edit ( std::ostream& o ) const = 0;
    virtual QString detail ( void ) const = 0;
    virtual QString note ( void ) const = 0;
    virtual void write ( QDomElement& xml_rep ) const = 0;
  };
#if 0
  class Fixed : public Constraint {
  public:
    Coincident ( Line* reference );
    Coincident ( const QDomElement& xml_rep, ConstrainedLine* parent );
    ~Coincident ( void ) {}

    bool isAdjustable ( void ) const { return false; }
    bool isFree ( void ) const { return false; }
    Point o ( void ) const;
    Vector e ( void ) const;
    Point setOffset ( double /*offset*/ );

    bool dependsOn( const Line* line ) const;

    double offset ( void ) const { return 0.; }
    Line* reference() const { return from_; }
    void setReference ( const Point& position, Line* reference );
    Point setPosition ( const Point& /*position*/ );
    void resolvedReference ( Line* reference );
    void referenceModified ( void ) {}

    ostream& edit ( ostream& o ) const;
    QString detail ( void ) const;
    QString note ( void ) const { return QString::null; }
    void write ( QDomElement& xml_rep ) const;
  };
#endif
  class Coincident : public Constraint {
    Line* from_;

  public:
    Coincident ( Line* reference );
    Coincident ( const QDomElement& xml_rep, ConstrainedLine* parent );
    ~Coincident ( void ) {}

    bool isAdjustable ( void ) const { return false; }
    bool isFree ( void ) const { return false; }
    Point o ( void ) const;
    Vector e ( void ) const;
    Point setOffset ( double /*offset*/ ) { return from_->o(); }

    bool dependsOn( const Line* line ) const;

    double offset ( void ) const { return 0.; }
    Line* reference() const { return from_; }
    void setReference ( const Point& position, Line* reference );
    Point setPosition ( const Point& /*position*/ ) { return from_->o(); }
    void resolvedReference ( Line* reference );
    void referenceModified ( void ) {}

    std::ostream& edit ( std::ostream& o ) const;
    QString detail ( void ) const;
    QString note ( void ) const { return QString::null; }
    void write ( QDomElement& xml_rep ) const;
  };

  class Offset : public Constraint {
    double offset_;
    Line* from_;

    Vector normal_;

  public:
    Offset ( Line* reference );
    Offset ( const Point& position, Line* reference );
    Offset ( const QDomElement& xml_rep, ConstrainedLine* parent );
    ~Offset ( void ) {}

    bool isAdjustable ( void ) const { return true; }
    bool isFree ( void ) const
    {
      if ( from_ == &X_AXIS || from_ == &Y_AXIS )
	return true;
      return false;
    }
    Point o ( void ) const;
    Vector e ( void ) const;

    bool dependsOn( const Line* line ) const;

    double offset ( void ) const { return offset_; };
    Line* reference ( void ) const { return from_; }

    Point setPosition ( const Point& position );
    Point setOffset ( double offset );

    void setReference ( const Point& position, Line* reference );
    void resolvedReference ( Line* reference );

    void referenceModified ( void );

    std::ostream& edit ( std::ostream& o ) const;
    QString detail ( void ) const;
    QString note ( void ) const { return QString::null; }
    void write ( QDomElement& xml_rep ) const;
  };

  /*!
   * Centered describes the second partner of two centered lines.
   * Note that the reference here is required to be a ConstrainedLine.
   */
  class Centered : public Constraint {
    double offset_;
    ConstrainedLine* from_;

    Vector normal_;

    static QString centerline_symbol_;

  public:
    Centered ( Line* reference );
    Centered ( const Point& position, Line* reference );
    Centered ( const QDomElement& xml_rep, ConstrainedLine* parent );
    ~Centered ( void ) {}

    bool isAdjustable ( void ) const { return true; }
    bool isFree ( void ) const { return false; }
    Point o ( void ) const;
    Vector e ( void ) const;

    bool dependsOn( const Line* line ) const;

    double offset ( void ) const { return offset_; };
    Line* reference ( void ) const;

    Point setPosition ( const Point& position );
    Point setOffset ( double offset );

    void setReference ( const Point& position, Line* reference );
    void resolvedReference ( Line* reference );

    void referenceModified ( void );

    std::ostream& edit ( std::ostream& o ) const;
    QString detail ( void ) const;
    QString note ( void ) const;
    void write ( QDomElement& xml_rep ) const;
  };

  class ConstrainedLine : public Line {
    Q_OBJECT
    Constraint* constraint_;
  public:
    ConstrainedLine ( uint id, const QString& name, Figure* parent,
		      Constraint* constraint = 0 );
    ConstrainedLine ( uint id, const QDomElement& xml_rep, Figure* parent );
    ~ConstrainedLine ( void );

    void setConstraint ( Constraint* constraint );
    void setConstraint ( const QDomElement& xml_rep );

    Constraint* constraint ( void ) const { return constraint_; }

    void resolved ( const QString& db_path, ModelItem* item );

    bool isAdjustable ( void ) const { return constraint_->isAdjustable(); }
    bool isFree ( void ) const { return constraint_->isFree(); }
    bool needsDimensionView ( void ) const
    {
      return constraint_->isAdjustable() && !constraint_->isFree();
    }

    double offset ( void ) const { return constraint_->offset(); }
    Line* reference ( void ) const { return constraint_->reference(); }

    void setOffset ( double offset );
    void setReference ( const Point& position, Line* _reference );
    void setPosition ( const Point& position );

    bool dependsOn( const Line* line ) const
    {
      return constraint_->dependsOn( line );
    }

    std::ostream& edit ( std::ostream& o ) const { return constraint_->edit( o ); }

    QString detail ( void ) const;
    // This is an (optional) annotation of the dimension string.
    QString note ( void ) const;

    void write ( QDomElement& xml_rep ) const;

  public slots:
    void setName ( const QString& name ) { Line::setName( name ); }
    void referenceModified ( void );
    void referenceDestroyed ( void );

  signals:
    //    void modifiedPosition ( void );
    void modifiedConstraint ( void );
  };

  inline Line* Centered::reference ( void ) const { return from_; }
} // End of Space2D namespace

namespace Space3D {

  class Line : public Curve {
    Q_OBJECT
  protected:
    Point o_;
    Vector e_;
  public:
    Line ( uint id, const QString& name, Figure* parent )
      : Curve( id, name, "line", parent )
    {}

    Line ( uint id, const QString& name, Point o, Vector e )
      : Curve( id, name, "line", 0 ), o_( o ), e_( e )
    {}

    virtual ~Line ( void ) {}

    Point o ( void ) const { return o_; }
    Vector e ( void ) const { return e_; }

    Point intersect ( const Curve* c ) const;
    /*!
     * Find the point which corresponds to this parameter.
     * \param t line parameter (exactly: distance from line origin).
     * \return point in space corresponding to t.
     */
    Point evaluate ( double t ) const;
    /*!
     * Find the parameter which corresponds to this point in space.
     * \param p point on line.
     * \return parametric coordinate of p.
     */
    double t ( const Point& p ) const;

    Vector normal ( void ) const;

    bool dependsOn( const Line* /*line*/ ) const { return false; }
    std::ostream& edit ( std::ostream& o ) const
    { return o << "Line: " << "origin: " << o_ << ", direction: " << e_; }

    void write ( QDomElement& /*xml_rep*/ ) const { }
  signals:
    void destroyedLine ();
    //    void modifiedPosition ();
  };
} // end of Space3D namespace
#endif // LINE_H
