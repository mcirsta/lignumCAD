# lignumCAD Sketch How-To

This is a practical guide to the current Sketch workflow in lignumCAD. It is
written for someone trying to design a wooden PC chassis, but the mechanics
apply to any 2D layout.

## What Sketch Mode Is

Sketch mode is not a general freehand drawing program. It is closer to a
constraint sketcher.

The core objects are:

- rectangles;
- reference lines;
- centerlines;
- annotations;
- dimensions, which create offset constraints;
- alignments, which create coincident constraints.

If your `Tools` menu shows only `Rectangle`, `Reference Line`, `Centerline`,
`Annotation`, `Dimension`, `Alignment`, and `Delete Constraint`, you are seeing
the complete Sketch toolset. Nothing is hidden.

So yes, the visible drawing vocabulary is small. The power is supposed to come
from relating edges to each other with dimensions and alignments.

For a PC chassis, that is actually fine. Most of the first design is boxes,
clearances, holes, airflow lanes, and labels.

## Start A New Sketch

1. Run lignumCAD.
2. If the New Model Wizard opens, enter a model name such as `BC-250 Wood
   Chassis`.
3. Go to the next wizard page.
4. Choose `Sketch` as the initial page type.
5. Finish the wizard.
6. When asked for the page name, enter `Component Envelope`.

If you already have a model open:

1. Choose `Insert | Sketch...`.
2. Name the page.
3. Make sure the new sketch tab is selected.

The page tab bar is important: the `Tools` menu changes depending on the page
type. On a Sketch page you should see sketch tools. On a Part or Assembly page
you will see different tools.

## Navigation

In a 2D Sketch page:

- left-drag on empty space pans the view;
- mouse wheel zooms;
- right-click opens the page context menu;
- `View | Restore View` resets the view;
- `Esc` cancels the current tool.

If you feel lost after zooming or panning, use `View | Restore View`.

## Set Units To Millimeters

For BC-250 work, use millimeters from the beginning.

1. Choose `Edit | Preferences...`.
2. Open the `Units` page.
3. In the length units field, choose millimeters, shown as `mm`.
4. Use decimal representation.
5. Pick a precision that matches your measurements:
   - `1 mm` for early enclosure layout;
   - `0.1 mm` for standoffs, rear I/O, and connector clearances.
6. Accept the dialog.

The code stores lengths internally in inches, so treat the unit choice as the
display and input format. Do not mix inches and millimeters inside one project.

## Create A Rectangle

Use rectangles for:

- the BC-250 board envelope;
- heatsink envelope;
- fan square;
- PSU envelope;
- wooden side panel;
- cutout clearance zone;
- screw access zone.

Steps:

1. Choose `Tools | Rectangle...`.
2. Move the mouse into the sketch area. The cursor should change.
3. Press the left mouse button where one corner should be.
4. Drag to the opposite corner.
5. Release the mouse button.
6. The rectangle becomes active and shows resize handles and dimensions.

Do not worry about exact size during the drag. Create it roughly, then edit
the dimensions.

After you release the mouse, the new rectangle stays selected. If the cursor
changes when you hover over the rectangle body or edges, that is modify mode.
An open hand means the rectangle can be moved. Diagonal, horizontal, or
vertical resize cursors mean you are over a resize handle. A closed hand means
you are panning the page from empty grid space.

Click empty grid space to deselect the rectangle. `Esc` cancels while you are
drawing the rectangle, but it is not the normal deselect command after the
rectangle has already been created.

## Set Rectangle Size

After creating a rectangle:

1. Double-click the width dimension text.
2. In the dimension dialog, enter the real width.
3. Accept the dialog.
4. Double-click the height dimension text.
5. Enter the real height.
6. Accept the dialog.

You can enter decimal dimensions. If you are using millimeters, keep your
project consistently in millimeters and label annotations explicitly, for
example `board envelope 247 mm x 180 mm`.

If dimension text is hard to select, hover until the dimension highlights, then
double-click.

## Rename A Rectangle

1. Double-click the rectangle body or edge, not the dimension text.
2. The rectangle information dialog opens.
3. Change the name to something meaningful:
   - `BC-250 PCB`;
   - `Heatsink`;
   - `Fan 120 intake`;
   - `Wood bottom panel`;
   - `PSU clearance`.
4. Accept the dialog.

Names matter because the model hierarchy becomes your object list.

## Move Or Resize A Rectangle

After selecting a rectangle:

- drag an edge to move the rectangle;
- drag a corner or side handle to resize it;
- double-click dimensions for exact sizes.

For precise design, prefer editing dimensions over dragging handles. Dragging
is good for rough placement.

## Create A Reference Line

Reference lines are construction lines. They help define important positions
without being physical parts.

Use them for:

- bottom of chassis;
- rear panel plane;
- airflow center;
- fan center;
- board datum edge;
- wood thickness boundary.

Steps:

1. Choose `Tools | Reference Line...`.
2. Click in the sketch.
3. To make it horizontal, click and release with little movement.
4. To make it vertical, click, drag mostly up/down, then release.
5. Double-click the line to rename it, for example `Rear panel inside face`.

Only horizontal and vertical reference lines are supported.

## Create A Centerline

Centerlines are like reference lines, but semantically mean symmetry or center.

Use them for:

- fan centers;
- front panel symmetry;
- board center;
- heatsink centerline.

Steps:

1. Choose `Tools | Centerline...`.
2. Click and release for the default orientation.
3. Drag during creation if you need the other orientation.
4. Rename it after creation.

Centerlines become more useful with the Dimension tool: if you pick a
centerline first, lignumCAD expects two more edge picks and centers those edges
around the centerline.

## Create An Annotation

Annotations are text boxes.

Use them aggressively. They are how you keep assumptions visible.

Steps:

1. Choose `Tools | Annotation...`.
2. Click and drag a text box.
3. Release the mouse.
4. The annotation dialog opens.
5. Enter text such as `Measure actual BC-250 standoff locations`.
6. Accept the dialog.

Good annotation examples:

- `Leave 20 mm cable clearance at 8-pin power`.
- `120 mm fan blows across passive heatsink`.
- `Do not cut wood until cardboard mockup fits`.
- `Rear I/O shield must be measured from actual board`.

## Create A Dimension Between Edges

The Dimension tool creates an offset relationship between parallel lines.

Use it to say:

- this fan is 20 mm from the front panel;
- this panel edge is 12 mm outside the internal air volume;
- this board edge is 15 mm from the bottom;
- this clearance zone stays 25 mm from the power connector.

Steps:

1. Choose `Tools | Dimension...`.
2. Click the first edge. This is the edge that will become constrained.
3. Click the second parallel edge. This is the reference edge.
4. A dimension is created.
5. Double-click the dimension text to set the exact offset.

Rules:

- The two picked edges must be parallel.
- Do not pick the same edge twice.
- If lignumCAD refuses the operation, it is usually avoiding a circular
  constraint or non-parallel geometry.

Important mental model: the pick order matters. The first edge is the one that
moves or depends on the second.

## Create An Alignment

The Alignment tool makes two parallel edges coincident.

Use it to say:

- left side of fan cutout is flush with a reference line;
- bottom of board tray is flush with a datum;
- one panel edge is exactly against another;
- two rectangles share an edge.

Steps:

1. Choose `Tools | Alignment...`.
2. Click the edge that should move.
3. Click the target parallel edge.
4. The first edge snaps to the target edge.

Rules:

- The two edges must be parallel.
- lignumCAD will not align two edges from the same rectangle.
- Pick order matters: first edge moves, second edge is the reference.

## Delete A Constraint

If a dimension or alignment has made the layout hard to edit:

1. Choose `Tools | Delete Constraint...`.
2. Click the constrained geometry.
3. The constraint is removed or reset to an axis reference.

Use this when a rectangle will not move the way you expect.

## A First BC-250 Sketch

Do this as your first useful exercise.

### Page: Component Envelope

1. Create a rectangle named `BC-250 PCB`.
2. Set its width and height to your measured board outline.
3. Create a rectangle named `Passive heatsink`.
4. Place it over the board where the heatsink sits.
5. Create a rectangle named `I/O shield`.
6. Place it on the rear side of the board.
7. Create a rectangle named `8-pin power clearance`.
8. Make it a clearance zone around the power connector, not a physical part.
9. Create one or more `Fan 120` rectangles.
10. Use annotations to mark airflow direction.

### Page: Side Clearance

1. Create a new Sketch page named `Side Clearance`.
2. Draw the board as a thin rectangle.
3. Draw the heatsink height as a rectangle rising from the board.
4. Draw the fan thickness.
5. Draw the wooden side/top panel.
6. Dimension the gap between heatsink/fan and wood.

This is the page that prevents the case from being too shallow.

### Page: Rear Panel

1. Create a new Sketch page named `Rear Panel`.
2. Draw the rear wood panel.
3. Draw the I/O shield opening from actual measurements.
4. Draw screw or insert locations.
5. Annotate every cutout.

Do not trust generic ATX or Mini-ITX rear templates for the BC-250 unless your
actual board/tray proves they match.

### Page: Fan And Airflow

1. Create a new Sketch page named `Airflow`.
2. Draw the board and heatsink.
3. Draw the intake fan square.
4. Draw the exhaust fan square.
5. Annotate the air path.

For the BC-250, treat the passive heatsink as needing forced airflow. The
public hands-on reports describe using a standard 120 mm fan to cool it.

## What lignumCAD Is Not Giving You Yet

In this port and this old codebase, do not expect:

- circles as first-class sketch entities;
- arbitrary freehand curves;
- modern parametric sketch solver behavior;
- reliable generated construction drawings;
- automatic stock layout;
- production-ready 3D case modeling.

For fan holes, draw a 120 mm square envelope and annotate the circular cutout.
For screw holes, use centerlines/reference lines and annotations. You can add
precise circles later in another CAD/CAM tool if needed.

## Practical Rule

Use lignumCAD for what it is good at right now:

- rectangular envelopes;
- relationships between edges;
- dimensions;
- labels;
- thinking through the build.

Then use cardboard, calipers, and a drill template before touching good wood.

## Source Notes

Local code references checked:

- Sketch tools are installed in `SketchView::show()` in `sketchview.cpp`.
- Rectangle creation is handled by `RectangleCreateInput` in
  `rectangleview.cpp`.
- Reference line and centerline creation are handled by
  `referencelineview.cpp` and `centerlineview.cpp`.
- Dimension and alignment tools are handled by `dimensionview.cpp` and
  `alignmentview.cpp`.

BC-250 public reference:

- Tom's Hardware, 2025-11-09: describes the BC-250 unit as a server-style
  motherboard with large passive heatsink, I/O ports, fan headers, I/O shield,
  standard 8-pin PCIe power, and use of a 120 mm fan for active cooling:
  https://www.tomshardware.com/video-games/playstation/amds-rare-playstation-5-apu-based-bc-250-mining-board-resurfaces-for-usd120-and-can-actually-run-cyberpunk-2077
