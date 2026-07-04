# lignumCAD Qt6 Port Plan

## Objective

Port lignumCAD to modern Qt6, OpenCascade, CMake, and a current GCC toolchain while preserving the behavior of the original application.

The original application behavior is the reference. Refactoring is deferred unless it directly enables a small, verifiable porting step.

## Branch Base

Create the port branch from:

```text
8712f9f657d04b39507b26a3b3d1f79d3ad0efa9
```

Recommended branch name:

```text
qt6-port
```

This commit is a good starting point because it is the last small commit before the Qt5 porting series begins. The later Qt5 port contains many incomplete or behavior-changing edits, so it should be used only as reference material.

Relevant preceding commits:

- `616ba28` imports the original lignumCAD 0.2 source.
- `724b3e1` applies PLD Linux patches.
- `7be3136` converts `.ui` files to Qt Designer 4 XML format.
- `8712f9f` adds `.gitignore`.
- `6730706` starts the Qt5 porting series and should not be included in the Qt6 branch baseline.

The caveat is that `8712f9f` already contains Qt Designer 4-format `.ui` files with `Q3*` classes. That is still a useful intermediate format compared with raw Qt3 Designer files, but the Qt6 port must replace those Qt3 support classes deliberately.

## Review Rules

Every commit should be small enough for human review.

Each step should do one kind of change only:

- Build-system translation.
- Dependency discovery.
- Mechanical compile fix.
- Qt API replacement.
- Runtime behavior restoration.
- Focused refactor after behavior is verified.

Do not mix broad cleanup with porting. Do not deduplicate copy/pasted code during the initial port unless the duplication blocks that exact step.

## Phase 0: Baseline

### 0.1 Create the Port Branch

Change:

- Create `qt6-port` from `8712f9f657d04b39507b26a3b3d1f79d3ad0efa9`.

Verify:

```sh
git status --short
git log --oneline --decorate -5
```

Review:

- No source changes.

### 0.2 Add This Plan

Change:

- Add `docs/QT6-PORT-PLAN.md`.

Verify:

```sh
git diff -- docs/QT6-PORT-PLAN.md
```

Review:

- Documentation-only change.

## Phase 1: CMake Skeleton

### 1.1 Add Minimal Top-Level CMake

Change:

- Add a minimal `CMakeLists.txt`.
- Define the project and C++ standard.
- Do not port source code yet.

Verify:

```sh
cmake -S . -B build-qt6
```

Review:

- Build-system-only diff.

### 1.2 Translate qmake File Lists

Change:

- Move the source, header, form, and resource lists from `lignumCAD.pro` into CMake variables.
- Preserve the existing file set.

Verify:

```sh
cmake -S . -B build-qt6
```

Review:

- No code edits.
- File lists should correspond to the old qmake project.
- Do not carry forward the legacy vendored Xft shim files `xftname.c` and `xftint.h` unless a later build proves they are required.

### 1.3 Add Qt6 Discovery

Change:

- Add `find_package(Qt6 REQUIRED COMPONENTS Widgets OpenGL OpenGLWidgets Xml PrintSupport)`.
- Enable `CMAKE_AUTOMOC`, `CMAKE_AUTOUIC`, and `CMAKE_AUTORCC`.
- Link Qt6 libraries to the executable target.

Verify:

```sh
cmake -S . -B build-qt6
```

Review:

- Qt dependency wiring only.

### 1.4 Add OpenCascade Discovery

Change:

- Prefer OpenCascade's CMake package config if available.
- Add a controlled fallback for systems where OCCT headers are under `/usr/include/opencascade` and libraries are under `/usr/lib`.
- Avoid the old hardcoded `/usr/lib/opencascade/lin64/gcc/lib` layout.

Verify:

```sh
cmake -S . -B build-qt6
```

Review:

- OpenCascade dependency wiring only.

### 1.5 Add Remaining System Dependencies

Change:

- Add FreeType, Fontconfig, OpenGL/GLU, and any other required system libraries.
- Avoid depending on Xft private APIs; keep the legacy vendored Xft shim quarantined unless proven necessary.
- Keep each dependency addition separate if possible.

Verify:

```sh
cmake -S . -B build-qt6
```

Review:

- Dependency wiring only.

## Phase 2: First Compile

### 2.1 Record the First Compile Failure

Change:

- No source fixes yet.
- Build and record the first reproducible compiler failure.

Verify:

```sh
cmake --build build-qt6
```

Review:

- Build log note only, if documented.

### 2.2 Fix Mechanical C++ Compile Blockers

Change:

- Make narrow mechanical fixes such as namespace qualification, standard headers, and removed include paths.
- Example: `ostream` should become `std::ostream` where required.

Verify:

```sh
cmake --build build-qt6
```

Review:

- Small compile-only commits.

### 2.3 Replace Removed Qt3 Signal Names

Change:

- Replace `QAction::activated()` connections with Qt6-compatible `triggered()` or `toggled()` connections.
- Keep behavior equivalent.

Verify:

```sh
rg "activated\\(\\)" .
cmake --build build-qt6
```

Review:

- Action/signal wiring only.

### 2.4 Remove Qt3 Support Classes Deliberately

Change:

- Replace `Q3*` classes with chosen Qt6 equivalents.
- Prefer the simplest faithful Qt6 widget first.

Verify:

```sh
cmake --build build-qt6
```

Review:

- One widget family per commit where practical.

### 2.5 Reach a Linkable Executable

Change:

- Continue small compile/link fixes until the executable links.

Verify:

```sh
cmake --build build-qt6
```

Review:

- No broad behavior refactors.

## Phase 3: Runtime Shell

### 3.1 Launch the Application

Change:

- Fix startup crashes only.

Verify:

- The main window opens.
- No model workflow needs to work yet.

Review:

- Startup-only changes.

### 3.2 Restore New Model Wizard

Change:

- Port the original wizard behavior to Qt6.
- Ensure the initial page choices are visible and selectable.

Verify:

- Create a new model.
- Select an initial page.
- Cancel path still works.

Review:

- Wizard-only changes.

### 3.3 Restore Page Tabs

Change:

- Restore page-to-tab mapping and tab switching behavior from the original application.

Verify:

- Create multiple pages.
- Switch tabs.
- Rename and delete pages.

Review:

- Page tab behavior only.

### 3.4 Restore Model Hierarchy

Change:

- Choose one Qt6 hierarchy implementation.
- Prefer `QTreeWidget` first if it gives the smallest faithful port.
- Preserve rename and selection behavior.

Verify:

- Model, pages, and child items appear.
- Renaming emits the expected model changes.
- Selection still drives the correct view behavior.

Review:

- Hierarchy-only changes.

## Phase 4: Core Workflows

### 4.1 Open and Save `.lcad` Files

Change:

- Fix file dialogs, XML read/write, `QUrl`, `DBURL`, and settings behavior.

Verify:

- Open a known `.lcad` file.
- Save it.
- Compare XML structure and object paths.

Review:

- File/model identity changes only.

### 4.2 Restore Sketch Workflow

Change:

- Port sketch tools: rectangle, centerline, reference line, annotation, dimensions, constraints.

Verify:

- Create, edit, delete, undo, and redo each sketch item.

Review:

- One tool family per commit where practical.

### 4.3 Restore Part Workflow

Change:

- Port part wizard, material dialog, and OpenCascade part creation.

Verify:

- Create a part.
- Assign material.
- Save and reopen.

Review:

- Part workflow only.

### 4.4 Restore Assembly Workflow

Change:

- Port add-model, delete-model, subassembly, and constraint behavior.

Verify:

- Add a model.
- Apply constraints.
- Save and reopen.

Review:

- Assembly workflow only.

### 4.5 Restore Drawing, Printing, and Export

Change:

- Port drawing views, print support, and export support.

Verify:

- Drawing page renders.
- Print/export path produces expected output or a documented remaining blocker.

Review:

- Drawing/printing/export only.

## Phase 5: Cleanup After Equivalence

### 5.1 Duplication Audit

Change:

- Identify repeated code blocks that appear safe to refactor.
- Do not refactor yet.

Verify:

- Produce a short list of candidates.

Review:

- Documentation-only or issue-list change.

### 5.2 Refactor One Duplicated Family at a Time

Change:

- Refactor only one repeated pattern per commit.
- Keep before/after behavior checks attached to the commit.

Verify:

- Repeat the relevant workflow checks before and after.

Review:

- Focused refactor only.

### 5.3 Qt6 Polish

Change:

- Move toward modern connect syntax.
- Reduce warnings.
- Add install rules and packaging support.

Verify:

```sh
cmake --build build-qt6
```

Review:

- No behavior changes unless explicitly called out.

## First Milestone

The first milestone is intentionally modest:

```text
The qt6-port branch configures with CMake and fails at a known, documented compile error.
```

That creates a clean review boundary before source porting begins.
