/*********************************************************************************/
/*!
@file           GuiPreferencesDialog.h

@brief          xxxx.

@author         L. J. Barman

    Copyright (c)   2008-2013, L. J. Barman, all rights reserved

    This file is part of the PianoBooster application

    PianoBooster is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    PianoBooster is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with PianoBooster.  If not, see <http://www.gnu.org/licenses/>.

*/
/*********************************************************************************/

#ifndef __GUIPREFERENCESDIALOG_H__
#define __GUIPREFERENCESDIALOG_H__

#include <qtutilities/settingsdialog/optionpage.h>

#include "Song.h"
#include "Settings.h"

class CGLView;

class Ui_GuiPreferencesDialog;

using GuiPreferencesOptionPageBase = ::QtUtilities::UiFileBasedOptionPage<Ui_GuiPreferencesDialog>;
class GuiPreferencesOptionPage : public QObject, public GuiPreferencesOptionPageBase {
    Q_OBJECT

public:
    explicit GuiPreferencesOptionPage(QObject *parent = nullptr);
    ~GuiPreferencesOptionPage();
    void init(CSong* song, CSettings* settings, CGLView* glView);
    bool apply() override;
    void reset() override;

protected:                                                                                                                                           \
    QWidget *setupWidget() override;

private:
    void initFollowStopPointCombo();
    void initLanguageCombo();

    CSettings* m_settings;
    CSong* m_song;
    CGLView *m_glView;
};

#endif //__GUIPREFERENCESDIALOG_H__
