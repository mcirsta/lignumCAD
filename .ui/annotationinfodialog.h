/****************************************************************************
** Form interface generated from reading ui file 'annotationinfodialog.ui'
**
** Created: Wed Dec 18 08:52:13 2002
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#ifndef ANNOTATIONINFODIALOG_H
#define ANNOTATIONINFODIALOG_H

#include <qvariant.h>
#include <qdialog.h>
class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
class QComboBox;
class QLabel;
class QLineEdit;
class QPushButton;
class QTextEdit;
class QToolButton;

class AnnotationInfoDialog : public QDialog
{ 
    Q_OBJECT

public:
    AnnotationInfoDialog( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~AnnotationInfoDialog();

    QLabel* nameTextLabel;
    QPushButton* colorButton;
    QTextEdit* annotationTextEdit;
    QPushButton* buttonHelp;
    QPushButton* buttonOk;
    QPushButton* buttonCancel;
    QLineEdit* nameEdit;
    QToolButton* boldButton;
    QToolButton* italicButton;
    QToolButton* underlineButton;
    QComboBox* sizesComboBox;


public slots:
    virtual void init();
    virtual void boldButton_toggled( bool bold );
    virtual void italicButton_toggled( bool italic );
    virtual void underlineButton_toggled( bool underline );
    virtual void pointSizeSpinBox_valueChanged( int value );
    virtual void colorButton_clicked();
    virtual void annotationTextEdit_currentColorChanged( const QColor & color );
    virtual void annotationTextEdit_currentFontChanged( const QFont & font );
    virtual void sizesComboBox_activated( const QString & text );
    virtual void buttonHelp_clicked();

protected:
    QGridLayout* AnnotationInfoDialogLayout;
    QHBoxLayout* Layout1;
    QHBoxLayout* Layout4;
};

#endif // ANNOTATIONINFODIALOG_H
