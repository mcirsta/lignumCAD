/*
 * parttemplates.cpp
 *
 * Standard part templates included in lignumCAD.
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
#include <qdom.h>

#include "constants.h"
#include "lcdefaultlengthconstraint.h"
#include "ocsolid.h"
#include "part.h"

namespace {
  class BlankBoard : public PartMetadata   {
  public:
    BlankBoard ( void )
      : PartMetadata( lC::STR::BLANKS, lC::STR::BOARD, "part_board2.png",
		      lC::STR::RPP )
    {
      addParameter( lC::STR::LENGTH_GRAIN );
      addParameter( lC::STR::WIDTH );
      addParameter( lC::STR::THICKNESS );
    }
    bool valid ( const QHash<int,lCDefaultLengthConstraint>& initial_values ) const
    {
      if ( initial_values[lC::STR::LENGTH_GRAIN] == 0 ||
	   initial_values[lC::STR::WIDTH] == 0 ||
	   initial_values[lC::STR::THICKNESS] == 0 )
	return false;

      if ( initial_values[lC::STR::LENGTH_GRAIN]->specifiedLength() > 0 &&
	   initial_values[lC::STR::WIDTH]->specifiedLength() > 0 &&
	   initial_values[lC::STR::THICKNESS]->specifiedLength() > 0 )
	return true;

      return false;
    }
    Space3D::OCSolid* create ( const QString& name,
                   const QHash<int,lCDefaultLengthConstraint>& initial_values,
			       Part* parent ) const
    {
      double length = initial_values[lC::STR::LENGTH_GRAIN]->specifiedLength();
      double width = initial_values[lC::STR::WIDTH]->specifiedLength();
      double thickness = initial_values[lC::STR::THICKNESS]->specifiedLength();
      return new Space3D::OCSolidRPP( parent->newID(), name,
				      length, width, thickness, parent );
    }

    Space3D::OCSolid* create ( const QString& name, const QDomElement& xml_rep,
			       Part* parent ) const
    {
      return new Space3D::OCSolidRPP( parent->newID(), name, xml_rep, parent );
    }
  };

  class BlankPGP : public PartMetadata   {
    // This wholy for debugging.
  public:
    BlankPGP ( void )
      : PartMetadata( lC::STR::BLANKS, "Parallelogram", "part_board2.png", "pgp" )
    {
      addParameter( lC::STR::LENGTH_GRAIN );
      addParameter( lC::STR::WIDTH );
      addParameter( lC::STR::THICKNESS );
    }
    bool valid ( const QHash<int,lCDefaultLengthConstraint>& initial_values ) const
    {
      if ( initial_values[lC::STR::LENGTH_GRAIN] == 0 ||
	   initial_values[lC::STR::WIDTH] == 0 ||
	   initial_values[lC::STR::THICKNESS] == 0 )
	return false;

      if ( initial_values[lC::STR::LENGTH_GRAIN]->specifiedLength() > 0 &&
	   initial_values[lC::STR::WIDTH]->specifiedLength() > 0 &&
	   initial_values[lC::STR::THICKNESS]->specifiedLength() > 0 )
	return true;

      return false;
    }
    Space3D::OCSolid* create ( const QString& name,
                   const QHash<int,lCDefaultLengthConstraint>& initial_values,
			       Part* parent ) const
    {
      double length = initial_values[lC::STR::LENGTH_GRAIN]->specifiedLength();
      double width = initial_values[lC::STR::WIDTH]->specifiedLength();
      double thickness = initial_values[lC::STR::THICKNESS]->specifiedLength();
      return new Space3D::OCSolidPGP( parent->newID(), name,
				      length, width, thickness, parent );
    }

    Space3D::OCSolid* create ( const QString& name, const QDomElement& xml_rep,
			       Part* parent ) const
    {
      return new Space3D::OCSolidPGP( parent->newID(), name, xml_rep, parent );
    }
  };

  class BlankTurning : public PartMetadata {
  public:
    BlankTurning ( void )
      : PartMetadata ( lC::STR::BLANKS, lC::STR::TURNING, "part_turning2.png",
		       lC::STR::CYL )
    {
      addParameter( lC::STR::LENGTH_GRAIN );
      addParameter( lC::STR::DIAMETER );
    }
    bool valid ( const QHash<int,lCDefaultLengthConstraint>& initial_values ) const
    {
      if ( initial_values[lC::STR::LENGTH_GRAIN] == 0 ||
	   initial_values[lC::STR::DIAMETER] == 0 )
	return false;
      if ( initial_values[lC::STR::LENGTH_GRAIN]->specifiedLength() > 0 &&
	   initial_values[lC::STR::DIAMETER]->specifiedLength() > 0 )
	return true;
      return false;
    }
    Space3D::OCSolid* create ( const QString& name,
                   const QHash<int,lCDefaultLengthConstraint>& initial_values,
			       Part* parent ) const
    {
      double length = initial_values[lC::STR::LENGTH_GRAIN]->specifiedLength();
      double diameter = initial_values[lC::STR::DIAMETER]->specifiedLength();
      return new Space3D::OCSolidCYL( parent->newID(), name,
				      length, diameter, parent );
    }

    Space3D::OCSolid* create ( const QString& name, const QDomElement& xml_rep,
			       Part* parent ) const
    {
      return new Space3D::OCSolidCYL( parent->newID(), name, xml_rep, parent );
    }
  };

  class TaperedLeg : public PartMetadata {
  public:
    TaperedLeg ( void )
      : PartMetadata ( lC::STR::LEGS, lC::STR::TAPERED, "part_tapered_leg2.png",
		       lC::STR::RPPPYR )
    {
      addParameter( lC::STR::LENGTH_GRAIN );
      addParameter( lC::STR::TOP_LENGTH );
      addParameter( lC::STR::TOP_WIDTH );
      addParameter( lC::STR::BOTTOM_WIDTH );
    }
    bool valid ( const QHash<int,lCDefaultLengthConstraint>& initial_values ) const
    {
      if ( initial_values[lC::STR::LENGTH_GRAIN] == 0 ||
	   initial_values[lC::STR::TOP_LENGTH] == 0 ||
	   initial_values[lC::STR::TOP_WIDTH] == 0 ||
	   initial_values[lC::STR::BOTTOM_WIDTH] == 0 )
	return false;
      if ( initial_values[lC::STR::LENGTH_GRAIN]->specifiedLength() > 0 &&
	   initial_values[lC::STR::TOP_LENGTH]->specifiedLength() > 0 &&
	   initial_values[lC::STR::TOP_LENGTH]->specifiedLength() <
	   initial_values[lC::STR::LENGTH_GRAIN]->specifiedLength() &&
	   initial_values[lC::STR::TOP_WIDTH]->specifiedLength() > 0 &&
	   initial_values[lC::STR::BOTTOM_WIDTH]->specifiedLength() > 0 &&
	   initial_values[lC::STR::BOTTOM_WIDTH]->specifiedLength() <
	   initial_values[lC::STR::TOP_WIDTH]->specifiedLength() )
	return true;
      return false;
    }
    Space3D::OCSolid* create ( const QString& name,
                   const QHash<int,lCDefaultLengthConstraint>& initial_values,
			       Part* parent ) const
    {
      double length = initial_values[lC::STR::LENGTH_GRAIN]->specifiedLength();
      double top_length = initial_values[lC::STR::TOP_LENGTH]->specifiedLength();
      double top_width = initial_values[lC::STR::TOP_WIDTH]->specifiedLength();
      double bottom_width = initial_values[lC::STR::BOTTOM_WIDTH]->specifiedLength();
      return new Space3D::OCSolidRPPPYR( parent->newID(), name,
					 length, top_length, top_width,
					 bottom_width, parent );
    }

    Space3D::OCSolid* create ( const QString& name, const QDomElement& xml_rep,
			       Part* parent ) const
    {
      return new Space3D::OCSolidRPPPYR( parent->newID(), name, xml_rep, parent );
    }
  };

  class TurnedLeg : public PartMetadata {
  public:
    TurnedLeg ( void )
      : PartMetadata ( lC::STR::LEGS, lC::STR::TURNED, "part_turned_leg2.png", "rppcon" )
    {
      addParameter( lC::STR::LENGTH_GRAIN );
      addParameter( lC::STR::TOP_LENGTH );
      addParameter( lC::STR::TOP_WIDTH );
      addParameter( lC::STR::TURNING_LENGTH );
      addParameter( lC::STR::TURNING_TOP_DIAMETER );
      addParameter( lC::STR::TURNING_BOTTOM_DIAMETER );
    }
    bool valid ( const QHash<int,lCDefaultLengthConstraint>& initial_values ) const
    {
      if ( initial_values[lC::STR::LENGTH_GRAIN] == 0 ||
	   initial_values[lC::STR::TOP_LENGTH] == 0 ||
	   initial_values[lC::STR::TOP_WIDTH] == 0 ||
	   initial_values[lC::STR::TURNING_LENGTH] == 0 ||
	   initial_values[lC::STR::TURNING_TOP_DIAMETER] == 0 ||
	   initial_values[lC::STR::TURNING_BOTTOM_DIAMETER] == 0 )
	return false;
      if ( initial_values[lC::STR::LENGTH_GRAIN]->specifiedLength() > 0 &&
	   initial_values[lC::STR::TOP_LENGTH]->specifiedLength() > 0 &&
	   initial_values[lC::STR::TOP_LENGTH]->specifiedLength() <
	   initial_values[lC::STR::LENGTH_GRAIN]->specifiedLength() &&
	   initial_values[lC::STR::TOP_WIDTH]->specifiedLength() > 0 &&
	   initial_values[lC::STR::TURNING_LENGTH]->specifiedLength() > 0 &&
	   initial_values[lC::STR::TURNING_LENGTH]->specifiedLength() <
	   ( initial_values[lC::STR::LENGTH_GRAIN]->specifiedLength() -
	     initial_values[lC::STR::TOP_LENGTH]->specifiedLength() ) &&
	   initial_values[lC::STR::TURNING_TOP_DIAMETER]->specifiedLength() > 0 &&
	   initial_values[lC::STR::TURNING_TOP_DIAMETER]->specifiedLength() <
	   initial_values[lC::STR::TOP_WIDTH]->specifiedLength() &&
	   initial_values[lC::STR::TURNING_BOTTOM_DIAMETER]->specifiedLength() > 0 &&
	   initial_values[lC::STR::TURNING_BOTTOM_DIAMETER]->specifiedLength() <
	   initial_values[lC::STR::TURNING_TOP_DIAMETER]->specifiedLength() )
	return true;
      return false;
    }
    Space3D::OCSolid* create ( const QString& name,
                   const QHash<int,lCDefaultLengthConstraint>& initial_values,
			       Part* parent ) const
    {
      double length = initial_values[lC::STR::LENGTH_GRAIN]->specifiedLength();
      double top_length = initial_values[lC::STR::TOP_LENGTH]->specifiedLength();
      double top_width = initial_values[lC::STR::TOP_WIDTH]->specifiedLength();
      double turning_length = initial_values[lC::STR::TURNING_LENGTH]->specifiedLength();
      double turning_top_diameter =
	initial_values[lC::STR::TURNING_TOP_DIAMETER]->specifiedLength();
      double turning_bottom_diameter =
	initial_values[lC::STR::TURNING_BOTTOM_DIAMETER]->specifiedLength();

      return new Space3D::OCSolidRPPCON( parent->newID(), name,
					 length, top_length, top_width,
					 turning_length, turning_top_diameter,
					 turning_bottom_diameter, parent );
    }

    Space3D::OCSolid* create ( const QString& name, const QDomElement& xml_rep,
			       Part* parent ) const
    {
      return new Space3D::OCSolidRPPCON( parent->newID(), name, xml_rep, parent );
    }
  };

  class TurnedKnob : public PartMetadata {
  public:
    TurnedKnob ( void )
      : PartMetadata ( lC::STR::KNOBS, lC::STR::TURNED, "part_turned_knob2.png",
		       lC::STR::BSPCYL )
    {
      addParameter( lC::STR::LENGTH_GRAIN );
      addParameter( lC::STR::DIAMETER );
      addParameter( lC::STR::BASE_DIAMETER );
      addParameter( lC::STR::BASE_LENGTH );
    }
    bool valid ( const QHash<int,lCDefaultLengthConstraint>& initial_values ) const
    {
      if ( initial_values[lC::STR::LENGTH_GRAIN] == 0 ||
	   initial_values[lC::STR::DIAMETER] == 0 ||
	   initial_values[lC::STR::BASE_DIAMETER] == 0 ||
	   initial_values[lC::STR::BASE_LENGTH] == 0 )
	return false;
      if ( initial_values[lC::STR::LENGTH_GRAIN]->specifiedLength() > 0 &&
	   initial_values[lC::STR::DIAMETER]->specifiedLength() > 0 &&
	   initial_values[lC::STR::BASE_DIAMETER]->specifiedLength() > 0 &&
	   initial_values[lC::STR::BASE_LENGTH]->specifiedLength() > 0 )
	return true;
      return false;
    }
    Space3D::OCSolid* create ( const QString& name,
                   const QHash<int,lCDefaultLengthConstraint>& initial_values,
			       Part* parent ) const
    {
      double length = initial_values[lC::STR::LENGTH_GRAIN]->specifiedLength();
      double diameter = initial_values[lC::STR::DIAMETER]->specifiedLength();
      double base_diameter = initial_values[lC::STR::BASE_DIAMETER]->specifiedLength();
      double base_length = initial_values[lC::STR::BASE_LENGTH]->specifiedLength();

      return new Space3D::OCSolidBSPCYL( parent->newID(), name,
					 length, diameter, base_diameter,
					 base_length, parent );
    }

    Space3D::OCSolid* create ( const QString& name, const QDomElement& xml_rep,
			       Part* parent ) const
    {
      return new Space3D::OCSolidBSPCYL( parent->newID(), name, xml_rep, parent );
    }
  };

  BlankBoard board;
#if 0
  BlankPGP parallelogram;	// For debugging only...
#endif
  BlankTurning turning;
  TurnedKnob turned_knob;
  TaperedLeg tapered_leg;
  TurnedLeg turned_leg;
} // End of unnamed namespace
