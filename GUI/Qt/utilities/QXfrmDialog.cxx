#include <QtGui>

#include "ndXfrms.h"

#include "QXfrmDialog.h"

QXfrmDialog::QXfrmDialog(QWidget *parent) : QDialog(parent)
{
  m_Parent = parent;
    
  // set up the GUI.
  m_GUI.setupUi(this);

  this->setWindowTitle("Transformation Editor");

  this->CreateActions();

  ndSetXfrmMissing(&m_Xfrm);
  m_XfrmUpdated = false;
    
  // set the line edits to contain only floats.
  m_GUI.xLineEdit->setValidator(new QDoubleValidator((double)ND_MAX_NEGATIVE, (double)(-1*ND_MAX_NEGATIVE), 8, this));
  m_GUI.yLineEdit->setValidator(new QDoubleValidator((double)ND_MAX_NEGATIVE, (double)(-1*ND_MAX_NEGATIVE), 8, this));
  m_GUI.zLineEdit->setValidator(new QDoubleValidator((double)ND_MAX_NEGATIVE, (double)(-1*ND_MAX_NEGATIVE), 8, this));
  m_GUI.q0LineEdit->setValidator(new QDoubleValidator(-1.0, 1.0, 8, this));
  m_GUI.qXLineEdit->setValidator(new QDoubleValidator(-1.0, 1.0, 8, this));
  m_GUI.qYLineEdit->setValidator(new QDoubleValidator(-1.0, 1.0, 8, this));
  m_GUI.qZLineEdit->setValidator(new QDoubleValidator(-1.0, 1.0, 8, this));

  // update the line edits.
  this->UpdateXfrm();
  
}

QXfrmDialog::~QXfrmDialog()
{

}

void QXfrmDialog::CreateActions()
{
  // may need in the future.
}

void QXfrmDialog::accept()
{
  // update the transform first.
  m_Xfrm.translation.x = m_GUI.xLineEdit->text().toFloat();
  m_Xfrm.translation.y = m_GUI.yLineEdit->text().toFloat();
  m_Xfrm.translation.z = m_GUI.zLineEdit->text().toFloat();
  m_Xfrm.rotation.q0 = m_GUI.q0LineEdit->text().toFloat();
  m_Xfrm.rotation.qx = m_GUI.qXLineEdit->text().toFloat();
  m_Xfrm.rotation.qy = m_GUI.qYLineEdit->text().toFloat();
  m_Xfrm.rotation.qz = m_GUI.qZLineEdit->text().toFloat();

  if( !ndIsXfrmMissing(&m_Xfrm) )
  {
    m_XfrmUpdated = true;
  }
  else
  {
    QMessageBox::critical(this, "Transform Dialog Error", "The transform is not valid. Check your input.");
    m_XfrmUpdated = false;
    QDialog::reject();
    return;
  }

  QDialog::accept();

  emit this->XfrmUpdated(this->m_Xfrm);
}

void QXfrmDialog::reject()
{
  m_XfrmUpdated = false;
  QDialog::reject();
}

void QXfrmDialog::UpdateAndShow()
{
  this->UpdateXfrm();
  this->show();
}

void QXfrmDialog::UpdateAndShow(ndQuatTransformation* xfrm)
{
  this->SetXfrm(xfrm);
  this->UpdateXfrm();
  this->show();
}

void QXfrmDialog::ResetXfrm()
{
  ndSetXfrmMissing(&m_Xfrm);
  m_XfrmUpdated = false;
}

void QXfrmDialog::SetXfrm(ndQuatTransformation* xfrm)
{
  ndCopyTransform(xfrm, &m_Xfrm);
  //this->UpdateXfrm();
}

void QXfrmDialog::UpdateXfrm()
{
  // update the line edits.
  QString tmpStr;
  tmpStr.sprintf("%4.6f", m_Xfrm.translation.x);
  m_GUI.xLineEdit->setText(tmpStr);
  tmpStr.sprintf("%4.6f", m_Xfrm.translation.y);
  m_GUI.yLineEdit->setText(tmpStr);
  tmpStr.sprintf("%4.6f", m_Xfrm.translation.z);
  m_GUI.zLineEdit->setText(tmpStr);
  tmpStr.sprintf("%4.8f", m_Xfrm.rotation.q0);
  m_GUI.q0LineEdit->setText(tmpStr);
  tmpStr.sprintf("%4.8f", m_Xfrm.rotation.qx);
  m_GUI.qXLineEdit->setText(tmpStr);
  tmpStr.sprintf("%4.8f", m_Xfrm.rotation.qy);
  m_GUI.qYLineEdit->setText(tmpStr);
  tmpStr.sprintf("%4.8f", m_Xfrm.rotation.qz);
  m_GUI.qZLineEdit->setText(tmpStr);
}