/*!
    @file           GuiPreferencesDialog.cpp

    @brief          xxx.

    @author         L. J. Barman

    Copyright (c)   2008-2020, L. J. Barman and others, all rights reserved

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

#include <QtWidgets>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonValue>
#include <QStringBuilder>
#include <QRegularExpression>

#include "GuiPreferencesDialog.h"
#include "GlView.h"

GuiPreferencesDialog::GuiPreferencesDialog(QWidget *parent)
    : QDialog(parent)
{
    setupUi(this);
    m_song = nullptr;
    m_settings = nullptr;
    m_glView = nullptr;
    setWindowTitle(tr("Preferences"));
    followStopPointCombo->addItem(tr("Automatic (Recommended)"));
    followStopPointCombo->addItem(tr("On the Beat"));
    followStopPointCombo->addItem(tr("After the Beat"));
}

void GuiPreferencesDialog::initLanguageCombo(){
#ifndef NO_LANGS

    // read langs.json
    const auto localeDirectory = Util::dataDir(QStringLiteral("translations"));
    auto file = QFile(localeDirectory + QStringLiteral("/langs.json"));
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        ppLogError("Error while opening langs.json");
        return;
    }
    const auto document = QJsonDocument::fromJson(file.readAll());
    file.close();
    if (document.isEmpty()) {
        ppLogError("langs.json is not valid");
        return;
    }

    // load languages
    const auto rootLangs = document.object();
    languageCombo->clear();
    languageCombo->addItem(QString(QChar('<') % tr("System Language") % QChar('>')), QString());
    languageCombo->addItem(QStringLiteral("English"), QStringLiteral("en"));
    if (m_settings->value(QStringLiteral("General/lang"), QString()).toString() == QLatin1String("en")){
        languageCombo->setCurrentIndex(languageCombo->count() - 1);
    }

    auto dirLang = QDir(localeDirectory);
    dirLang.setFilter(QDir::Files | QDir::Hidden | QDir::NoDotAndDotDot);
    const auto listLang = QFileInfoList(dirLang.entryInfoList());
    static const auto rx = QRegularExpression(QStringLiteral("(pianobooster_)(.*)(.qm)"));
    for (const QFileInfo &fileInfo : listLang) {
        const auto match = rx.match(fileInfo.fileName());
        if (!match.hasMatch()) {
            continue;
        }
        const auto langCode = match.captured(2);
        if (langCode == QLatin1String("blank")) {
            continue;
        }
        auto langCodeLoc = langCode;
        if (langCodeLoc.indexOf(QChar('_')) == -1) {
            langCodeLoc += QChar('_') + langCodeLoc.toUpper();
        }

        auto languageName = QString();
        if (rootLangs.value(langCode).toObject().value(QLatin1String("nativeName")).isString()) {
            languageName = rootLangs.value(langCode).toObject().value(QLatin1String("nativeName")).toString();
        }
        if (languageName.isEmpty()){
            auto loc = QLocale(langCodeLoc);
            languageName = loc.nativeLanguageName();
            if (languageName.isEmpty()){
                languageName = QLocale::languageToString(loc.language());
            }
        }
        languageName[0] = languageName[0].toUpper();
        if (languageName.isEmpty() || languageName == QChar('C')) {
            languageName=langCode;
        }

        languageCombo->addItem(languageName,langCode);
        if (m_settings->value(QStringLiteral("General/lang"), QString()).toString() == langCode){
            languageCombo->setCurrentIndex(languageCombo->count() - 1);
        }
    }
#else
    // loading languages
    languageCombo->clear();
    languageCombo->addItem(QStringLiteral("English"), QStringLiteral("en"));
    languageCombo->setCurrentIndex(0);
#endif
}

void GuiPreferencesDialog::init(CSong* song, CSettings* settings, CGLView * glView)
{
    m_song = song;
    m_settings = settings;
    m_glView = glView;

    timingMarkersCheck->setChecked(m_song->cfg_timingMarkersFlag);
    showNoteNamesCheck->setChecked(m_settings->isNoteNamesEnabled());
    courtesyAccidentalsCheck->setChecked(m_settings->displayCourtesyAccidentals());
    showTutorPagesCheck->setChecked(m_settings->isTutorPagesEnabled());
    followThroughErrorsCheck->setChecked(m_settings->isFollowThroughErrorsEnabled());
    showColoredNotesCheck->setChecked(m_settings->isColoredNotesEnabled());

    followStopPointCombo->setCurrentIndex(m_song->cfg_stopPointMode);

    initLanguageCombo();
}

void GuiPreferencesDialog::accept()
{
    m_song->cfg_timingMarkersFlag = timingMarkersCheck->isChecked();
    m_settings->setValue("Score/TimingMarkers", m_song->cfg_timingMarkersFlag );
    m_settings->setNoteNamesEnabled( showNoteNamesCheck->isChecked());
    m_settings->setCourtesyAccidentals( courtesyAccidentalsCheck->isChecked());
    m_settings->setTutorPagesEnabled( showTutorPagesCheck->isChecked());
    m_settings->setFollowThroughErrorsEnabled( followThroughErrorsCheck->isChecked());
    m_settings->setColoredNotes( showColoredNotesCheck->isChecked());
    m_song->cfg_stopPointMode = static_cast<stopPointMode_t> (followStopPointCombo->currentIndex());
    m_settings->setValue("Score/StopPointMode", m_song->cfg_stopPointMode );

    m_settings->setValue("General/lang", languageCombo->currentData().toString());

    m_song->refreshScroll();

    this->QDialog::accept();
}
