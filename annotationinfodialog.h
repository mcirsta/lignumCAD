#ifndef ANNOTATIONINFODIALOG
#define ANNOTATIONINFODIALOG

#include "ui_annotationinfodialog.h"

class AnnotationInfoDialog : public QDialog
{
    Q_OBJECT

public:
    AnnotationInfoDialog( QWidget* parent = 0 );
    Ui::AnnotationInfoDialog* getUi();


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

private:
    Ui::AnnotationInfoDialog ui;
};

#endif // ANNOTATIONINFODIALOG

