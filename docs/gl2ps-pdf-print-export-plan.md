# gl2ps PDF Export/Print Plan

## Purpose

This document is the source of truth for integrating system `gl2ps` PDF output
with lignumCAD's Qt 6 print/export flow.

The plan is deliberately diagnostic-first. We do not continue adding export
features until we prove that `gl2ps` can capture known OpenGL primitives in the
same context used by `OpenGLPrinter`.

## Current Confidence

Status: Phase 2 passed (2026-07-08); awaiting in-app validation (Phase 3/4).

Root cause of `GL2PS_NO_FEEDBACK`: `OpenGLPrinter` was a hidden
`QOpenGLWidget` (`hide(); resize(1,1)`). Under Qt 6 a QOpenGLWidget that is
never shown never initializes its OpenGL context, so its `makeCurrent()` is a
documented no-op. gl2ps's `glRenderMode(GL_FEEDBACK)` therefore ran against no
context (or a stale one), which explains all three observed symptoms,
including the nondeterministic "captured some vector data, but not the page"
runs (those happened when the on-screen view's context was accidentally still
current).

Phase 2 evidence (standalone smoke test, this machine, AMD radeonsi /
Mesa 26.1.4, GL 4.6 Compatibility Profile):

- `QOffscreenSurface` + `QOpenGLContext` (default format), one hardcoded
  `GL_LINES` primitive: `gl2psBeginPage` and `gl2psEndPage` both return
  `GL2PS_SUCCESS`, with and without an FBO bound.
- `pdfinfo` reports a valid 612x792 PDF; `pdftoppm` renders exactly one
  diagonal line.
- Locale finding confirmed live: without `LC_NUMERIC=C` the PDF contains
  decimal-comma operators (`,199997`) and poppler reports syntax errors.
  `QGuiApplication` sets the locale from the environment, so the
  `NumericLocaleGuard` in `openglprinter.cpp` is required.

Resulting architecture change: `OpenGLPrinter` now owns a `QOffscreenSurface`
plus a `QOpenGLContext` shared with the on-screen view's context, and makes it
current inside `renderPagePdf()` for the whole begin/draw/end sequence. The
hidden widget remains only as the view-state object (fonts, scale, parent).

Second `GL2PS_NO_FEEDBACK` cause, found after the context fix (2026-07-08):
on the real Wayland session a `QOffscreenSurface` has no default framebuffer,
so every draw call fails with `GL_INVALID_FRAMEBUFFER_OPERATION` (0x506) and
is discarded before reaching the feedback stage. The first smoke test missed
this because the `offscreen` platform plugin does provide a default
framebuffer. Reproduced in a standalone harness (shown `QOpenGLWidget` as
share context + offscreen shared context, exactly mirroring
`makeRenderContextCurrent()`): without an FBO, `GL2PS_NO_FEEDBACK` + GL error
0x506; with a small `QOpenGLFramebufferObject` bound, `GL2PS_SUCCESS`.
`renderPagePdf()` now binds a 16x16 FBO for the duration of the capture.
Feedback capture never rasterizes, so the FBO size is irrelevant; it only
needs to make the framebuffer complete.

Third failure, found after the FBO fix (2026-07-08): at export time
`share_widget_->context()` was null ("The main view's OpenGL context is not
available"). Under Qt 6 a QOpenGLWidget's context is owned by the widget and
can be torn down on hide/reparent, so it is not a reliable share source.
Fix: `main()` now sets `Qt::AA_ShareOpenGLContexts`, which puts every
QOpenGLWidget context in one application-lifetime share group, and
`makeRenderContextCurrent()` shares with `QOpenGLContext::globalShareContext()`
first, the widget context second, and falls back to an unshared context (with
a qWarning) rather than failing — geometry export does not need sharing at
all; only display lists/textures cached by the on-screen view (3D part pages)
do. Verified in the standalone harness: global share context + offscreen
context + FBO returns `GL2PS_SUCCESS`.

## Working Rules

- One patch answers one question.
- Every patch has one expected observable result.
- Do not tune page layout while capture itself is unproven.
- Do not add printing until single-page PDF export is reliable.
- Do not add dimension/text export until geometry export is reliable.
- Do not rely on visual guessing. Inspect the PDF stream and render it to PNG.
- Preserve user work. Do not reset or revert unrelated files.

## Known Facts

Installed system header: `/usr/include/gl2ps.h`.

Relevant status codes from gl2ps 1.4.x:

```c
#define GL2PS_SUCCESS       0
#define GL2PS_INFO          1
#define GL2PS_WARNING       2
#define GL2PS_ERROR         3
#define GL2PS_NO_FEEDBACK   4
#define GL2PS_OVERFLOW      5
#define GL2PS_UNINITIALIZED 6
```

Important API shape:

```c
GLint gl2psBeginPage(const char *title, const char *producer,
                     GLint viewport[4], GLint format, GLint sort,
                     GLint options, GLint colormode,
                     GLint colorsize, GL2PSrgba *colormap,
                     GLint nr, GLint ng, GLint nb,
                     GLint buffersize, FILE *stream,
                     const char *filename);
```

Important compatibility notes:

- Use `#include <gl2ps.h>`, not a bundled local header.
- System gl2ps 1.4.x does not provide the old bundled `GL2PS_QT` backend.
- System gl2ps 1.4.x does not provide the old bundled `gl2psLineStipple(...)`
  helper.
- gl2ps PDF output is one complete PDF document per `gl2psBeginPage()` /
  `gl2psEndPage()` pair.
- Appending multiple gl2ps PDF pages to one `FILE*` does not create a valid
  multi-page PDF.
- `QPrinter` should be used as the settings/dialog object, not as the renderer.

## Target Architecture

Final intended design:

- `QPrinter` and `QPrintDialog` collect native print settings.
- `OpenGLPrinter` renders each page to a single-page PDF using system gl2ps.
- Export writes one PDF for the current page.
- Print generates one PDF per selected page and sends the files to CUPS.
- Multi-page print-to-file is handled by a real PDF merge tool or by a clear
  fallback, not by concatenating gl2ps output.

Non-goals for the first reliable export:

- EMF output.
- PostScript output.
- Windows printing.
- Persisted page setup in the document model.
- Perfect dimension/text output.

## Phase 0 - Freeze And Audit

Question: what exactly is in the tree before further work?

Actions:

- Record `git status --short`.
- Record current gl2ps-related diffs.
- Identify provisional changes made during debugging.
- Decide whether to keep them, amend them, or replace them after diagnostics.

Commands:

```sh
git status --short
git diff -- CMakeLists.txt openglprinter.cpp openglprinter.h designbookview.cpp
git diff -- rectangleview.cpp centerlineview.cpp referencelineview.cpp
```

Review gate:

- We can point to each gl2ps-related edit and state whether it is foundational,
  provisional, or suspect.

## Phase 1 - Build And Link Sanity

Question: are we definitely compiling and linking against one system gl2ps?

Actions:

- Remove bundled `gl2ps.c`, `gl2ps.h`, and `gl2ps_qt.cpp` from the build.
- Ensure repository-root `gl2ps.h` cannot shadow the system header.
- Link exactly one system gl2ps library.
- Keep compile-time guards for gl2ps 1.4.x and `GL2PS_PDF`.

Commands:

```sh
cmake -S . -B build-qt6
cmake --build build-qt6 -j2
ldd build-qt6/lignumCAD | rg -i 'gl2ps|not found'
rg 'GL2PS_QT|GL2PS_EMF|GL2PS_USE_QT|gl2psLineStipple'
```

Expected result:

- Build succeeds.
- `ldd` shows `/usr/lib/libgl2ps.so...`.
- No bundled-only gl2ps APIs remain in active code.

Review gate:

- No runtime export testing until this passes.

## Phase 2 - Minimal gl2ps Smoke Test

Question: can gl2ps capture one known primitive in the `OpenGLPrinter` context?

This phase must avoid `PageView::draw()`. It should draw one hardcoded primitive
inside the same `makeCurrent()` / `gl2psBeginPage()` / `gl2psEndPage()` path.

Suggested temporary diagnostic:

```cpp
glMatrixMode(GL_PROJECTION);
glLoadIdentity();
gluOrtho2D(0.0, 100.0, 0.0, 100.0);

glMatrixMode(GL_MODELVIEW);
glLoadIdentity();

glColor3d(0.0, 0.0, 0.0);
glBegin(GL_LINES);
glVertex2d(10.0, 10.0);
glVertex2d(90.0, 90.0);
glEnd();
```

Inspection commands:

```sh
pdfinfo /home/marius/temp/t.pdf
strings /home/marius/temp/t.pdf
pdftoppm -png /home/marius/temp/t.pdf /tmp/lignumcad-gl2ps-smoke
```

Expected result:

- `gl2psEndPage()` returns `GL2PS_SUCCESS`.
- PDF content stream contains drawing operators.
- Rendered PNG shows one diagonal line.

If it fails with `GL2PS_NO_FEEDBACK`:

- The issue is not lignumCAD geometry.
- Investigate OpenGL context currentness, OpenGL profile/compatibility, viewport
  passed to gl2ps, and whether gl2ps feedback works with `QOpenGLWidget`'s
  context.

Review gate:

- No `PageView` work until this passes.

## Phase 3 - PageView Isolation

Question: does `PageView::draw()` emit gl2ps-capturable primitives?

Actions:

- Keep the known smoke-test primitive available behind a local diagnostic flag.
- Run three variants:
  - smoke primitive only;
  - smoke primitive before `page_view_->draw()`;
  - smoke primitive after `page_view_->draw()`.

Expected results:

- If smoke-only succeeds but smoke-before-page fails, `PageView::draw()` or a
  child draw path corrupts GL state before gl2ps completes.
- If smoke-before-page succeeds but page geometry is absent, lignumCAD drawing
  emits primitives gl2ps does not capture or they are outside the projection.
- If smoke-after-page succeeds but smoke-before-page does not, GL state changes
  need to be localized.

Diagnostics to capture:

- current GL viewport;
- projection/modelview matrices;
- page view space (`SPACE2D` or `SPACE3D`);
- `source_view->llCorner()` and `source_view->urCorner()`;
- number of figure views on the page;
- gl2ps begin/end status names.

Review gate:

- We know whether the problem is context setup, projection/framing, or page
  drawing behavior.

## Phase 4 - Geometry Export Only

Question: can exported PDF show clean 2D geometry without dimensions/text?

Actions:

- Export current page geometry only.
- Suppress transient UI state: highlights, handles, active constraints, cursor
  helpers.
- Do not export dimension text yet.
- Do not add title block/frame yet.
- Use a stable page size from `QPrinter` defaults, but do not optimize layout.

Expected result:

- A simple sketch rectangle exports as visible rectangle geometry.
- PDF is valid under `pdfinfo`.
- Rendered PNG matches the basic geometry location and orientation.

Review gate:

- User confirms simple geometry is acceptable enough to continue.

## Phase 5 - Dimensions And Text

Question: how should text be represented in PDF?

Do not rely blindly on OGLFT polygon glyph rendering. Earlier testing produced
purple/scrambled glyph-like output, so text needs its own decision.

Options:

- Use gl2ps-native text calls where feasible.
- Keep vector outlines only after proving they render correctly.
- Temporarily omit dimensions from export if geometry export is the priority.

Expected result:

- Dimension values are legible.
- Text placement is stable at PDF point scale.
- No scrambled glyph polygons.

Review gate:

- A rectangle with dimensions exports with readable dimensions.

## Phase 6 - Page Layout And QPrinter Integration

Question: can export use QPrinter page settings without disturbing capture?

Actions:

- Build `GL2PSPageSettings` from `QPrinter::pageLayout().fullRectPoints()`.
- Honor orientation, page size, color mode, and a 1/2 inch default margin.
- Keep the gl2ps viewport and active GL viewport consistent.
- Apply margins through projection/framing only after capture is proven.

Expected result:

- Letter landscape default exports as Letter landscape.
- Geometry is upright, not mirrored, and not stretched.
- Margins are predictable.

Review gate:

- Current-page PDF export is good enough to be the base for printing.

## Phase 7 - Print Derived From Export

Question: can the reliable export renderer support printing?

Actions:

- Keep `QPrintDialog`.
- Generate one temporary PDF per selected page.
- Preserve document page numbers in title blocks.
- Send PDFs to CUPS with `lpr` using `QProcess`.
- For print-to-file:
  - one page: save/copy the generated PDF;
  - multiple pages: merge with a real PDF tool or offer a directory fallback.

Expected result:

- A selected page range prints as one CUPS job.
- Print-to-file creates valid PDFs.

Review gate:

- Printing is derived from the same renderer that export already proved.

## Validation Matrix

Minimum manual cases:

- Empty page.
- Sketch with one rectangle.
- Sketch with selected rectangle.
- Sketch with rectangle plus dimensions.
- Centerline/reference line dash patterns.
- Annotation/title text.
- Landscape and portrait page settings.
- Page range printing.
- Print-to-file one page.
- Print-to-file multiple pages.

Inspection tools:

```sh
pdfinfo output.pdf
strings output.pdf
pdftoppm -png output.pdf /tmp/output-check
```

Success criteria:

- No PDF parser errors.
- No empty content stream for non-empty pages.
- No `GL2PS_NO_FEEDBACK` for pages with known diagnostic primitives.
- Geometry appears in rendered PNG.
- Text is legible before it is declared supported.

## Phase 3/4 Results (2026-07-08)

Phase 3 root cause: with capture fully working, `GL2PS_NO_FEEDBACK` remained
because the export framed the *screen viewport*, not the page content. A
reloaded model views from its saved view point, so the geometry (rectangle at
x 23.1-34.1, y 11.75-15.1) sat entirely outside the ortho box
(-0.19,-1.36)-(5.4,2.9) and every primitive was clipped. Proven with the
in-app smoke line (`LIGNUMCAD_GL2PS_SMOKE=before`): smoke captured, page
contributed nothing.

Fixes, verified end-to-end against `build-qt6/1.lcad` via the
`LIGNUMCAD_AUTO_EXPORT` diagnostic hook (dialog-free export then exit):

- `OpenGLPrinter::measureContentBounds2D()`: a plain `glFeedbackBuffer`
  measuring pass over a +-1000 in projection runs `page_view_->draw()` once
  and reads back the true content bounds; export and print fit those bounds
  (5% padding) to the printable area. Falls back to the screen framing for
  empty pages. It reuses the exact draw path, so bounds always match output.
- `OpenGLPrinter` overrides `geometryColor()`/`annotationColor()` to black:
  screen schemes draw light-on-dark, which is invisible on paper.
- `RectangleView::draw()` skips solid/textured fills when printing
  (outline-only geometry export, Phase 4 scope).

Result: the test rectangle exports as a clean black outline, correctly
framed and proportioned on Letter landscape; `GL2PS_SUCCESS`, no GL errors,
valid PDF.

Temporary diagnostics still in the tree (remove after Phase 5/6 settle):

- stderr `gl2ps:` framing/status lines in `renderPagePdf()`.
- `LIGNUMCAD_GL2PS_SMOKE=before|only` smoke primitive.
- `LIGNUMCAD_AUTO_EXPORT=<path>` dialog-free export hook in
  `DesignBookView` (file-load constructor only).

Unrelated pre-existing bug observed: app teardown segfaults in
`~RectangleView` during `PageView::clearFigureViews()` (also on normal
exit paths); not printer-related.

## Current Stop Point

Phases 0-2 are done:

- Phase 1 verified: build links exactly `/usr/lib/libgl2ps.so.1`; no bundled
  gl2ps sources or bundled-only APIs (`GL2PS_QT`, `gl2psLineStipple`) remain.
- Phase 2 verified with the standalone offscreen smoke test described above.
- Phase 7 is provisionally wired ahead of order because it reuses the same
  renderer with no new capture logic: `print()` renders one PDF per selected
  page and sends the files to CUPS with `lp`; print-to-file copies a single
  page or merges multiple pages with `pdfunite`.
- The title block (`drawFrame`) stays disabled everywhere until Phase 5
  proves text output.

Phase 3/4 are validated for the single-rectangle case (see Phase 3/4
Results above).

Next action: user validates export/print against the rest of the matrix
(selected rectangle, centerline/reference line dash patterns, empty page,
page ranges), then Phase 5 decides text/dimension representation.
