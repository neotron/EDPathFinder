#include <QSettings>
#include <QFileDialog>
#include <QTextToSpeech>
#include <src/Notifications/MessageToaster.h>

#include "Preferences.h"
#include "ui_Preferences.h"
#include "Settings.h"
#include "Theme.h"

Preferences::Preferences(QWidget *parent)
        : QDialog(parent), _ui(new Ui::Preferences), _speech(new QTextToSpeech(this)) {
    _ui->setupUi(this);
    setModal(true);
    connect(this, SIGNAL(accepted()), SLOT(savePreferences()));
    connect(this, SIGNAL(rejected()), SLOT(revertTheme()));
    _buttonGroup.addButton(_ui->defaultTheme, Theme::Default);
    _buttonGroup.addButton(_ui->darkTheme, Theme::Dark);
    switch(Settings::restoreTheme()) {
        default:
            _ui->defaultTheme->setChecked(true);
            break;
        case Theme::Dark:
            _ui->darkTheme->setChecked(true);
            break;
    }
    _ui->journalPath->setText(Settings::restoreJournalPath());
    connect(&_buttonGroup, SIGNAL(buttonClicked(int)), this, SLOT(themeChanged()));
    connect(_ui->journalEdit, SIGNAL(clicked()), this, SLOT(selectJournalPath()));
    connect(_ui->testVoice, SIGNAL(clicked()), this, SLOT(testTTS()));
    _ui->voices->clear();

    // load voice settinghs
    auto defaultVoice = Settings::restore(MessageToaster::kVoiceKey, _speech->voice().name());
    RESTORE_CHECKED_DEFAULT(textNotifications, MessageToaster::kTextNotificationsEnabledKey, true);
    RESTORE_CHECKED_DEFAULT(voiceNotifications, MessageToaster::kVoiceNotificationsEnabledKey, true);
    auto rate = Settings::restore(MessageToaster::kVoiceRateKey, 0.0);
    auto pitch = Settings::restore(MessageToaster::kVoicePitchKey, 0.0);
    _ui->pitchSlider->setValue(static_cast<int>((pitch + 1) * 1000));
    _ui->rateSlider->setValue(static_cast<int>((rate + 1) * 1000));

    for(const auto &voice: _speech->availableVoices()) {
        auto voiceString = QString("%2, %3 %1").arg(voice.genderName(voice.gender()))
                .arg(voice.name()).arg(voice.ageName(voice.age()));
        _ui->voices->addItem(voiceString, voice.name());
        if(defaultVoice == voice.name()) {
            _ui->voices->setCurrentText(voiceString);
        }
    }
}

Preferences::~Preferences() {
    delete _ui;
}

void Preferences::savePreferences() {
    const auto oldPath = Settings::restoreJournalPath();
    const auto newPath = _ui->journalPath->text();

    if(QDir(oldPath) != QDir(newPath)) {
        Settings::saveJournalPath(newPath);
        emit journalPathUpdated(oldPath, newPath);
    }

    Settings::saveTheme(selectedThemeId());
    SAVE_CHECKED(voiceNotifications, MessageToaster::kVoiceNotificationsEnabledKey);
    SAVE_CHECKED(textNotifications, MessageToaster::kTextNotificationsEnabledKey);
    Settings::save(MessageToaster::kVoiceKey, _ui->voices->currentData().toString());
    Settings::save(MessageToaster::kVoiceRateKey, convertedSliderValue(_ui->rateSlider));
    Settings::save(MessageToaster::kVoicePitchKey, convertedSliderValue(_ui->pitchSlider));
    MessageToaster::instance().loadSettings();
}

void Preferences::themeChanged() {
    Theme::applyTheme(selectedThemeId());
}

void Preferences::revertTheme() {
    Theme::applyTheme();
}

Theme::Id Preferences::selectedThemeId() {
    return static_cast<Theme::Id>(_buttonGroup.checkedId());
}
void Preferences::selectJournalPath() {
    QString dir = QFileDialog::getExistingDirectory(this, "Select Journal Directory",
                                                    Settings::restoreJournalPath(),
                                                    QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if(!dir.isEmpty()) {
        _ui->journalPath->setText(dir);
    }
}


void Preferences::testTTS() {
    _speech->stop();

    auto pitch = convertedSliderValue(_ui->pitchSlider);
    auto rate = convertedSliderValue(_ui->rateSlider);
    auto voiceName = _ui->voices->currentData().toString();

    for(const auto &voice: _speech->availableVoices()) {
        if(voice.name() == voiceName) {
            _speech->setVoice(voice);
        }
    }

    _speech->setRate(rate);
    _speech->setPitch(pitch);
    _speech->say(QString("I am %1. Will you journey from Sol to Distant Worlds with me?").arg(voiceName));

}

double Preferences::convertedSliderValue(const QSlider *slider) const { return slider->value() / 1000.0 - 1.0; }
