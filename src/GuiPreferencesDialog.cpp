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

#include <QDir>
#include <QFontInfo>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonValue>
#include <QStringBuilder>
#include <QRegularExpression>

#include <functional>

#include "GuiPreferencesDialog.h"
#include "GlView.h"

#include "ui_GuiPreferencesDialog.h"

GuiPreferencesOptionPage::GuiPreferencesOptionPage(QObject *parent)
    : QObject(parent)
    , m_settings(nullptr)
    , m_song(nullptr)
    , m_glView(nullptr)
{
}

GuiPreferencesOptionPage::~GuiPreferencesOptionPage()
{
}

void GuiPreferencesOptionPage::initLanguageCombo() {
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
    ui()->languageCombo->clear();
    ui()->languageCombo->addItem(QString(QChar('<') % tr("System Language") % QChar('>')), QString());
    ui()->languageCombo->addItem(QStringLiteral("English"), QStringLiteral("en"));
    if (m_settings->value(QStringLiteral("General/lang"), QString()).toString() == QLatin1String("en")){
        ui()->languageCombo->setCurrentIndex(ui()->languageCombo->count() - 1);
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

        ui()->languageCombo->addItem(languageName, langCode);
        if (m_settings->value(QStringLiteral("General/lang"), QString()).toString() == langCode){
            ui()->languageCombo->setCurrentIndex(ui()->languageCombo->count() - 1);
        }
    }
#else
    // loading languages
    languageCombo->clear();
    languageCombo->addItem(QStringLiteral("English"), QStringLiteral("en"));
    languageCombo->setCurrentIndex(0);
#endif
}

void GuiPreferencesOptionPage::init(CSong* song, CSettings* settings, CGLView * glView)
{
    m_song = song;
    m_settings = settings;
    m_glView = glView;
}

bool GuiPreferencesOptionPage::apply()
{
    Cfg::experimentalNoteLength = ui()->showNoteLengthCheck->isChecked();
    m_settings->setValue("Score/ShowNoteLength", Cfg::experimentalNoteLength);
    m_song->cfg_timingMarkersFlag = ui()->timingMarkersCheck->isChecked();
    m_settings->setValue("Score/TimingMarkers", m_song->cfg_timingMarkersFlag );
    m_settings->setNoteNamesEnabled( ui()->showNoteNamesCheck->isChecked());
    m_settings->setCourtesyAccidentals( ui()->courtesyAccidentalsCheck->isChecked());
    m_settings->setTutorPagesEnabled( ui()->showTutorPagesCheck->isChecked());
    m_settings->setFollowThroughErrorsEnabled( ui()->followThroughErrorsCheck->isChecked());
    m_settings->setColoredNotes( ui()->showColoredNotesCheck->isChecked());
    m_song->cfg_stopPointMode = static_cast<stopPointMode_t>(ui()->followStopPointCombo->currentIndex());
    m_settings->setValue("Score/StopPointMode", m_song->cfg_stopPointMode );
    m_settings->setValue("General/lang", ui()->languageCombo->currentData().toString());
    m_song->refreshScroll();
    return true;
}

void GuiPreferencesOptionPage::reset()
{
    ui()->showNoteLengthCheck->setChecked(Cfg::experimentalNoteLength);
    ui()->timingMarkersCheck->setChecked(m_song->cfg_timingMarkersFlag);
    ui()->showNoteNamesCheck->setChecked(m_settings->isNoteNamesEnabled());
    ui()->courtesyAccidentalsCheck->setChecked(m_settings->displayCourtesyAccidentals());
    ui()->showTutorPagesCheck->setChecked(m_settings->isTutorPagesEnabled());
    ui()->followThroughErrorsCheck->setChecked(m_settings->isFollowThroughErrorsEnabled());
    ui()->showColoredNotesCheck->setChecked(m_settings->isColoredNotesEnabled());
    ui()->followStopPointCombo->setCurrentIndex(m_song->cfg_stopPointMode);
}

QWidget *GuiPreferencesOptionPage::setupWidget()
{
    auto *widget = static_cast<QtUtilities::OptionPageWidget *>(GuiPreferencesOptionPageBase::setupWidget());
    initFollowStopPointCombo();
    initLanguageCombo();
    QObject::connect(widget, &QtUtilities::OptionPageWidget::retranslationRequired, widget, std::bind(&GuiPreferencesOptionPage::initFollowStopPointCombo, this));
    return widget;
}

void GuiPreferencesOptionPage::initFollowStopPointCombo()
{
    auto *const combo = ui()->followStopPointCombo;
    const auto index = combo->currentIndex();
    combo->clear();
    combo->addItem(tr("Automatic (Recommended)"));
    combo->addItem(tr("On the Beat"));
    combo->addItem(tr("After the Beat"));
    combo->setCurrentIndex(index);

}
