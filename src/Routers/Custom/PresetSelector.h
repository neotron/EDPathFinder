#ifndef PRESETSELECTOR_H
#define PRESETSELECTOR_H

#include <QDialog>
#include "PresetEntry.h"

namespace Ui {
class PresetSelector;
}

class PresetSelector : public QDialog
{
    Q_OBJECT

public:
    explicit PresetSelector(QWidget *parent, const PresetEntryList &presets);
    ~PresetSelector();

private:
    Ui::PresetSelector *_ui;
};

#endif // PRESETSELECTOR_H
