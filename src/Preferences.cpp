#include <QSettings>
#include <QFileDialog>

#include "Preferences.h"
#include "ui_Preferences.h"
#include "Settings.h"
#include "Theme.h"

Preferences::Preferences(QWidget *parent)
        : QDialog(parent), _ui(new Ui::Preferences) {
    _ui->setupUi(this);
    connect(this, SIGNAL(accepted()), SLOT(savePreferences()));
    connect(this, SIGNAL(rejected()), SLOT(revertTheme()));
    _buttonGroup.addButton(_ui->defaultTheme, Theme::Default);
    _buttonGroup.addButton(_ui->darkTheme, Theme::Dark);
    switch(Settings::theme()) {
        default:
            _ui->defaultTheme->setChecked(true);
            break;
        case Theme::Dark:
            _ui->darkTheme->setChecked(true);
            break;
    }
    _ui->journalPath->setText(Settings::journalPath());
    connect(&_buttonGroup, SIGNAL(buttonClicked(int)), this, SLOT(themeChanged()));

    connect(_ui->journalEdit, SIGNAL(clicked()), this, SLOT(selectJournalPath()));
}

Preferences::~Preferences() {
    delete _ui;
}

void Preferences::savePreferences() {
    const auto oldPath = Settings::journalPath();
    const auto newPath = _ui->journalPath->text();

    if(QDir(oldPath) != QDir(newPath)) {
        Settings::setJournalPath(newPath);
        emit journalPathUpdated(oldPath, newPath);
    }

    Settings::setTheme(selectedThemeId());
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
                                                    Settings::journalPath(),
                                                    QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if(!dir.isEmpty()) {
        _ui->journalPath->setText(dir);
    }
}
