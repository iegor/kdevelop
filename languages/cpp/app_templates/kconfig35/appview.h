%{H_TEMPLATE}

#ifndef _%{APPNAMEUC}VIEW_H_
#define _%{APPNAMEUC}VIEW_H_

#include <qwidget.h>

#include "%{APPNAMELC}view_base.h"

class QPainter;
class KURL;

/**
 * This is the main view class for %{APPNAME}.  Most of the non-menu,
 * non-toolbar, and non-statusbar (e.g., non frame) GUI code should go
 * here.
 *
 * @short Main view
 * @author %{AUTHOR} <%{EMAIL}>
 * @version %{VERSION}
 */
class %{APPNAMELC}View : public %{APPNAMELC}view_base
{
    Q_OBJECT
public:
	/**
	 * Default constructor
	 */
    %{APPNAMELC}View(QWidget *parent);

	/**
	 * Destructor
	 */
    virtual ~%{APPNAMELC}View();

signals:
    /**
     * Use this signal to change the content of the statusbar
     */
    void signalChangeStatusbar(const QString& text);

    /**
     * Use this signal to change the content of the caption
     */
    void signalChangeCaption(const QString& text);

private slots:
    void switchColors();
    void settingsChanged();
};

#endif // _%{APPNAMEUC}VIEW_H_
