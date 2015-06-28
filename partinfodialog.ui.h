/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you wish to add, delete or rename slots use Qt Designer which will
** update this file, preserving your code. Create an init() slot in place of
** a constructor, and a destroy() slot in place of a destructor.
*****************************************************************************/


void PartInfoDialog::buttonHelp_clicked()
{
    QWhatsThis::display( tr( "<p><b>Part Information</b></p>\
<p>This dialog lets you modify the overall attributes of a Part. Currently, \
the only attribute you can change is the name of the Part.</p>\
<p>Click the <b>OK</b> button \
(or press <b>Enter</b> or <b>Alt+O</b>) to apply the change. \
If you enter the same name as an existing Part, \
<i>lignumCAD</i> will reject the change.</p>\
<p>You can cancel these changes by clicking the <b>Cancel</b> \
button (or by pressing <b>ESC</b> or <b>Alt+C</b>).</p>" ) );
}
