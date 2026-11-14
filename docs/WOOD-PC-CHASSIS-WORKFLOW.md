# Wood PC Chassis Workflow

This note is for a Qt developer who wants to use lignumCAD, or at least the
lignumCAD way of thinking, to design a wooden PC chassis without already being
a woodworker.

The short version: do not start by trying to model a perfect wooden computer
case. Start by designing a safe, measurable enclosure around real PC parts,
then split that enclosure into wooden panels and hardware interfaces.

## 1. The Mental Model

lignumCAD thinks like a design notebook.

- A model is the whole project.
- A sketch page is a 2D view: top, front, side, rear, airflow diagram, or panel
  layout.
- A part page is one physical piece: side panel, base, front frame, top panel,
  fan bracket, motherboard tray adapter, foot, or spacer.
- An assembly page is where parts are positioned relative to each other.
- Drawing, stock layout, and rendering were planned features, but the old 0.2
  documentation says they were not implemented yet.

For this project, treat lignumCAD less like finished production CAD and more
like a constrained design notebook. Its strongest fit is the early thinking:
rectangles, dimensions, offsets, relationships, and simple board-like parts.

## 2. The Project Goal

Build a wooden PC chassis that is:

- safe around electronics and mains power;
- easy enough for a woodworking beginner;
- dimensionally predictable;
- serviceable after assembly;
- thermally sane;
- visually pleasing because the wood is doing real structural work, not just
  decoration.

The recommended first version is a wood shell around a known metal PC hardware
interface.

That means: salvage or buy a metal motherboard tray / rear I/O / PCI slot area
from a cheap case, then build the wooden chassis around it. Do not make your
first build depend on hand-cutting the motherboard standoff pattern, I/O shield
opening, and PCI bracket alignment from scratch. Those interfaces are fussy in
the same way binary ABIs are fussy: close is not good enough.

## 3. Recommended First Build Scope

For generic PC hardware, use a small, low-risk layout:

- Mini-ITX motherboard, commonly 170 mm x 170 mm.
- SFX or external-brick power supply if possible.
- One CPU cooler with a published height.
- No discrete GPU for the first enclosure, or a short GPU only after the first
  box is proven.
- Two 120 mm fans: one intake, one exhaust.
- Wood panels of uniform thickness, for example 6 mm to 12 mm plywood or
  hardwood panels.
- Metal standoffs and metal threaded inserts for anything screwed repeatedly.

Avoid these in version 1:

- Full ATX board.
- Large modern GPU.
- Liquid cooling.
- Complex joinery.
- All-wood PCI bracket support.
- Hidden cable channels that require impossible assembly order.

For an AMD BC-250, do not assume Mini-ITX. Treat it as a custom board.

Public hands-on reports describe the BC-250 unit as a server-style mining
motherboard with a large passive heatsink, I/O ports, fan headers, I/O shield,
standard 8-pin PCIe power, no normal DDR memory slots, and active cooling from
a 120 mm fan during testing. That means the first design datum is not an ATX
template. It is the actual board, heatsink, I/O shield, power connector, fan
position, and standoff pattern you can measure.

For the BC-250 first version:

- measure the board outline yourself;
- measure the heatsink length, width, and height;
- measure the I/O shield opening from the actual shield;
- measure the 8-pin power connector and cable bend clearance;
- measure fan header positions and decide where the fan cable will route;
- design around forced airflow across the passive heatsink;
- leave a removable panel over the board and power connector;
- avoid decorative wood blocking the rear I/O or exhaust path.

## 4. Hardware Facts To Start From

Common motherboard envelopes:

- ATX: 305 mm x 244 mm, also written 12 in x 9.6 in.
- microATX: 244 mm x 244 mm, also written 9.6 in x 9.6 in.
- Mini-ITX: 170 mm x 170 mm, also written 6.7 in x 6.7 in.

Treat those as envelope dimensions only. For real build work, measure or import
the exact board, PSU, cooler, fans, and GPU you will actually use.

Create a project table before modeling:

| Item | Model | Critical dimensions | Notes |
| --- | --- | --- | --- |
| Board | AMD BC-250 | outline, standoffs, I/O shield, 8-pin power | Measure actual unit |
| Heatsink | BC-250 passive heatsink | length, width, height, fin direction | Needs forced airflow |
| PSU | TBD | length, width, height, cable exit side | 8-pin PCIe power required |
| Fan | TBD | 120 mm square, hole pattern, thickness | Aim across heatsink fins |
| Storage | TBD | SSD location and cable exits | Keep cables out of airflow |
| Wood | TBD | actual thickness after sanding | Do not trust nominal thickness |

For the BC-250, add a second table for measured points. Pick one corner of the
PCB as `(0, 0)` and measure every critical feature from that corner.

| Feature | X from datum | Y from datum | Width/height | Notes |
| --- | --- | --- | --- | --- |
| PCB outline | 0 | 0 | TBD | Measure with calipers/ruler |
| Standoff 1 | TBD | TBD | screw size TBD | Do not drill until verified |
| Standoff 2 | TBD | TBD | screw size TBD |  |
| I/O shield opening | TBD | TBD | TBD | Rear panel cutout |
| 8-pin power connector | TBD | TBD | TBD | Add cable bend clearance |
| 120 mm fan center | TBD | TBD | 120 mm fan | Align to heatsink airflow |
| SSD mount | TBD | TBD | TBD | Optional |

## 5. Safety Rules

Wood can be a good chassis material, but it changes the problem.

- Do not open or modify an ATX power supply.
- Keep mains voltage inside certified PSU hardware.
- Use metal motherboard standoffs. The motherboard should not sit directly on
  wood.
- Use metal threaded inserts where screws will be removed more than once.
- Keep wood away from hot exhaust zones and high-wattage parts.
- Make the airflow path obvious: cool air in, warm air out, no dead pocket.
- Assume wood moves slightly with humidity. Leave clearance around precise
  metal interfaces.
- Plan dust filters as removable parts.
- Plan a real power button, front I/O, and cable strain relief.
- Test thermals with the side open first, then closed.

Wood is an electrical insulator, which is useful, but it is not an EMI shield
and it is not a heatsink. A metal motherboard tray and rear I/O region are
therefore helpful even if the visible chassis is wood.

## 6. Translate The Case Into Pages

Make these sketch pages first.

### Page 1: Component Envelope

Draw rectangles for the real hardware:

- motherboard;
- cooler clearance;
- PSU;
- fan squares;
- cable clearance zones;
- optional GPU envelope.

This page answers: "How large does the empty air volume need to be?"

### Page 2: Airflow

Draw the same box, but now annotate:

- intake fan;
- exhaust fan;
- CPU cooler direction;
- PSU intake and exhaust;
- hot zones;
- dust filter access.

This page answers: "Can air move through the chassis without fighting itself?"

### Page 3: Wooden Panels

Draw each panel as a board:

- left side;
- right side;
- top;
- bottom;
- front;
- rear;
- removable access panel.

This page answers: "What parts would I actually cut?"

### Page 4: Hardware Interfaces

Draw only the precise interfaces:

- motherboard tray mounting points;
- rear I/O opening;
- PCI bracket area if used;
- PSU mounting area;
- fan holes;
- button and port cutouts.

This page answers: "Where does wood need to meet metal?"

### Page 5: Assembly Order

Draw or list the sequence:

1. Install threaded inserts.
2. Mount metal tray to wood base or side.
3. Install motherboard.
4. Install PSU.
5. Route cables.
6. Install fans and filters.
7. Close panels.

This page answers: "Can I physically assemble and service it?"

## 7. A Beginner-Friendly Physical Design

The simplest useful structure is a U-shaped wooden shell plus removable side
panel:

- bottom panel is the base;
- two side rails or side panels carry the tray and top;
- top panel screws down;
- one side panel is removable;
- rear metal tray handles motherboard I/O and PCI alignment;
- front is mostly wood with fan intake, button, and maybe USB.

Use butt joints or simple rabbets for v1. Fancy joinery can come later.

Good beginner materials:

- Baltic birch plywood, 6 mm to 12 mm;
- hardwood only after the layout is proven;
- acrylic or perforated aluminum only as inserts, not structural assumptions.

Good beginner fastening:

- threaded inserts in wood;
- M3 motherboard screws into metal standoffs;
- M4 or #6 screws for wood panels, depending on local hardware;
- machine screws where panels will be opened often.

## 8. How This Maps To lignumCAD

Use Sketch pages for almost everything at first.

- Rectangle tool: panels, hardware envelopes, fan openings.
- Dimensions: motherboard envelope, cooler height, PSU clearance.
- Reference lines: airflow centerline, panel boundaries, symmetry axes.
- Centerlines: fan centers, front panel symmetry, motherboard tray center.
- Annotation: label assumptions and unknowns.
- Part pages: only after sketches feel stable.
- Assembly pages: only after you know the panel list and interface points.

Do not wait for perfect 3D. A good 2D plan with dimensions beats a pretty 3D
model with wrong clearances.

## 9. Concrete lignumCAD Session

Here is the flow I would use in the current app for the BC-250.

1. Create a new model named `BC-250 Wood Chassis`.
2. Start with a Sketch page named `Component Envelope`.
3. Draw the BC-250 PCB as a rectangle at the measured real size.
4. Add rectangles for the passive heatsink, I/O shield, 8-pin power clearance,
   PSU, storage, and fans.
5. Use dimensions for every size that comes from a real component.
6. Use annotations for assumptions, for example `10 mm cable clearance`.
7. Add a second Sketch page named `Airflow`.
8. Copy or redraw the same outer box and annotate intake, exhaust, and hot
   zones.
9. Add a third Sketch page named `Panels`.
10. Draw each wooden panel as a rectangle with actual material thickness.
11. Add a fourth Sketch page named `Interfaces`.
12. Draw only the precise cutouts and mounting regions.
13. Export or print the sketches.
14. Build a cardboard mockup before creating Part or Assembly pages.

Only after the cardboard mockup makes sense should you create Part pages for
the wooden pieces. In the original lignumCAD concept, Part pages are where the
bill of materials begins to become real. For this project, a part should map to
something you could label and cut: `Left Side Panel`, `Bottom Panel`, `Front
Frame`, `Top Panel`, `Fan Bracket`, and so on.

Use Assembly pages late. An assembly is useful when you already know your
parts and want to check how they sit together. It is not the best place to
discover the basic size of the case.

## 10. Qt Developer Translation Layer

Think of the chassis like a UI layout problem:

- Components are widgets with minimum sizes.
- Airflow is the event loop: if it blocks, everything suffers.
- Mounting holes are API contracts.
- Wood thickness is padding and border, but it changes the outside size.
- Cable bend radius is hidden layout pressure.
- Removable panels are public maintenance APIs.
- The rear I/O area is not a custom widget you want to write first.

The same discipline helps: define constraints, identify fixed interfaces,
mock the simplest layout, then iterate.

## 11. First Design Exercise

Before cutting anything, make this first model:

1. Measure the BC-250 PCB outline.
2. Measure the passive heatsink height and fin direction.
3. Measure the I/O shield and rear port position.
4. Measure the 8-pin PCIe power connector and cable exit direction.
5. Pick a real PSU strategy.
6. Pick one or two real 120 mm fans.
7. Sketch a rectangular internal air volume around those parts.
8. Add 10 mm to 20 mm clearance around cable exits and fingers.
9. Wrap that volume in wood thickness.
10. Mark which panel is removable.
11. Mark every screw that must be reached with a screwdriver.
12. Export or print the 2D sketches and inspect them at full scale if possible.

Only after that should you decide whether the final design wants to be:

- vertical tower;
- horizontal desktop;
- open-frame test bench with wood panels;
- wood sleeve around a metal core.

## 12. What To Measure On The Bench

For each physical component, measure:

- overall length, width, and height;
- protrusions beyond the official board or case envelope;
- connector locations;
- cable exit directions;
- screw hole center positions;
- required airflow direction;
- service access direction.

Then add clearance deliberately. A beginner-friendly case is a little larger
than necessary.

## 13. Suggested Milestones

Milestone 1: Paper CAD

- Complete 2D sketches for component envelope, airflow, panels, and assembly
  order.
- No wood cut yet.

Milestone 2: Cardboard Mockup

- Build the outer volume from cardboard or foam board.
- Place real parts inside without powering them.
- Check screw access, cables, and fan positions.

Milestone 3: Ugly Prototype

- Build from cheap plywood or MDF.
- Use the real metal tray and standoffs.
- Run the PC outside the prototype first, then inside it.
- Measure temperatures.

Milestone 4: Nice Wood Version

- Rebuild with better wood and finish.
- Keep the same proven hardware interfaces.
- Improve aesthetics only after the thermal and mechanical design works.

## 14. References Used

- Local lignumCAD manual: `doc/en/HTML/SOHTML/Introduction.html`,
  `Sketch.html`, `Part.html`, `Assembly.html`, `Drawing.html`, `Layout.html`,
  and `Render.html`.
- Local port notes: `docs/QT6-PORT-PLAN.md` and
  `docs/gl2ps-pdf-print-export-plan.md`.
- Local usage notes: `docs/LIGNUMCAD-SKETCH-HOWTO.md`.
- General form-factor dimensions checked against the current ATX summary:
  https://en.wikipedia.org/wiki/ATX
- BC-250 public hands-on reference:
  https://www.tomshardware.com/video-games/playstation/amds-rare-playstation-5-apu-based-bc-250-mining-board-resurfaces-for-usd120-and-can-actually-run-cyberpunk-2077

Always prefer your actual component datasheets and your own calipers over this
document.
