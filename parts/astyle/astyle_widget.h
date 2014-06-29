//kate: space-indent on; tab-width 2; indent-width 2; indent-mode cstyle; encoding UTF-8;
#ifndef __ASTYLE_WIDGET_H__
#define __ASTYLE_WIDGET_H__

#include "astyle.h"
#include "astyleconfig.h"

class AStylePart;
class KDevPart;

class AStyleWidget : public AStyleConfig
{
  Q_OBJECT
public:
  enum context{ GLOBAL, PROJECT };

  AStyleWidget(AStylePart * part, AStyleWidget::context widget_context, QWidget *parent=0, const char *name=0);
  ~AStyleWidget();

  int get_selected_style();

public slots:
  void accept();

private slots:
  void set_useglobal();
  void set_style();
  void set_fill();
  void set_indent();
  void set_blocks_and_brackets();
  void slotSetBrackets(int brackets_mode);
  void slotSetIndentMode(int mode);
  void set_oneliners();
  void set_padding();
  void slotToolsPage(int current);

private:
  void set_example();
  void fillcontrols(QMap<QString, QVariant> &options);
  void readcontrols(QMap<QString, QVariant> &options);
  void enable_controls();
  void disable_controls();

private:
  AStylePart * m_part;
  const context m_context;
  QString m_lastExt;
  bool m_bUseGlobalOpts;

  // Utils:
  QMap<QString, QVariant>& getContextOptions();

};

#endif
