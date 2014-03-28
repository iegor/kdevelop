/*
*kate: space-indent on; tab-width 2; indent-width 2; indent-mode cstyle; encoding UTF-8;
*
*  C/C++ Implementation: vs_solution
*
* Description:
*
*
* Author: iegor <rmtdev@gmail.com>, (C) 2013
*
* Copyright: See COPYING file that comes with this distribution
*/
#include <string.h>
#include <qtextstream.h>
#include <kdebug.h>

#include "vs_solution.h"

UUID::UUID() {
    memset(&m1, 0, 32);
}

VSSolution::VSSolution(QString name, QString path)
: fl(path) {
}

VSSolution::~VSSolution() {
    _write_file();
}

bool VSSolution::Parse() {
    int sln_ver = 0;
    if(!fl.open(IO_ReadWrite|IO_Raw)) {
        kdDebug() << "can't open file for solution" << endl;
        return false;
    }

    if(!fl.isReadable()) {
        kdDebug() << "Solution file exist, but is not readable" << endl;
        return false;
    }

    kdDebug() << "<<<<< Parsing solution file >>>>>" << endl;

    QString ln;
    QTextStream str(&fl);

    //BEGIN // Read the MS guards and version info
    QString ms_guard, ms_sharp_guard, ms_ver;
    int sln_ver_expected = 0;

    ms_guard = str.readLine();
    if(ms_guard.isEmpty()) {
        kdDebug() << "empty line at begin of file detected" << endl;
        ms_guard = str.readLine();
    }

    ms_sharp_guard = str.readLine();

    if(ms_sharp_guard.compare(QString("# Visual Studio 2008")) == 0) {
        sln_ver_expected = 10;
    } else if(ms_sharp_guard.compare(QString("# Visual Studio 2005")) == 0) {
        sln_ver_expected = 9;
    } else {
        sln_ver_expected = 8;
    }

    printf("MS_GUARD: %s\n", ms_guard.ascii());
    printf("MS_#GUARD: %s\n", ms_sharp_guard.ascii());

    ms_ver = ms_guard.right(5);
    ms_ver.remove(ms_ver.find('.'), 3);
    printf("ms_ver: \"%s\"\n", ms_ver.ascii());
    bool b_ok = false;
    sln_ver = ms_ver.toInt(&b_ok, 10);

    if(!b_ok) {
        kdDebug() << "can't convert version string to int" << endl;
        //return false;
    }

    printf("Solution ver: %i\n", sln_ver);
    //END // Read the MS guards and version info

    while(!str.atEnd()) {
        ln = str.readLine();
        //BEGIN // Read project info
        if(0 == ln.left(7).compare(QString("Project"))) {
            while(0 != ln.left(10).compare(QString("EndProject"))) {

                //BEGIN // Read project section info, dependencies
                if(0 == ln.left(15).compare(QString("ProjectSection"))) {
                    while(0 != ln.left(17).compare(QString("EndProjectSection"))) {
                        printf("Project section data: %s\n", ln.ascii());
                        ln = str.readLine();
                    }
                }
                //END // Read project section info, dependencies

                // Read project data

                /**
                 * E.G. project info inside sln looks like this.
                 * Project("{8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942}") = "testing_stuf", "testing_stuf.vcproj", "{4B448DC1-8FF4-41AC-8734-A655187A84D7}"
                 * So we need to split it into these pieces:
                 *
                 * - "Project" string
                 *
                 * - ("{8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942}")
                 *   solution internal UUID of project
                 *
                 * - "project name" internal for solution
                 *
                 */
                printf("Project data: %s\n", ln.ascii());

                ln = str.readLine();
            }
        }
        //END // Read project info
        else if(0 == ln.compare("Global")) {
        }
    }

    kdDebug() << "<<<<< Parsing solution file : FINISHED >>>>>" << endl;
    return true;
}

bool VSSolution::_write_file() {
    QTextStream out(&fl);
    return true;
}