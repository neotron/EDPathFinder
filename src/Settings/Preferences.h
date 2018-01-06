#pragma once
#include <QDialog>
#include <QButtonGroup>
#include <QSlider>
#include "Theme.h"

namespace Ui {
    class Preferences;
}

class QTextToSpeech;

class Preferences : public QDialog
{
    Q_OBJECT

public:
    explicit Preferences(QWidget *parent = 0);
    ~Preferences() override;


signals:
    void journalPathUpdated(const QString &from, const QString &to);

public slots:
    void themeChanged();
private slots:
    void testTTS();
    void savePreferences();
    void revertTheme();
    void selectJournalPath();
private:
    Theme::Id selectedThemeId();
    QButtonGroup _buttonGroup;
    Ui::Preferences *_ui;
    QTextToSpeech *_speech;

    double convertedSliderValue(const QSlider *slider) const;
};
