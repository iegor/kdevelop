/*  C/C++ Implementation: vs_view_variables
*
* Description:
*
*
* Author: iegor <rmtdev@gmail.com>, (C) 2014
*
* Copyright: See COPYING file that comes with this distribution
*/
#ifndef __VS_VIEW_VARIABLES_WIDGET_H__
#define __VS_VIEW_VARIABLES_WIDGET_H__

/* Qt */
#include <qtable.h>

/* VStudio */
#include "vs_common.h"

namespace VStudio {
  class VSViewVars : public QTable {
    Q_OBJECT

    public:
      VSViewVars(VSPart *part, QWidget *parent=0, const char *name=0, WFlags fl=0);
      virtual ~VSViewVars();

    // QWidget's methods:
    protected:
      virtual void resizeEvent(QResizeEvent *event);

    public slots:
      void slotRefreshValues();

    private:
      VSPart *part;
  };
}; /* namespace VStudio */

#endif /* __VS_VIEW_VARIABLES_WIDGET_H__ */