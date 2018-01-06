#pragma once

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

public slots:
    void slotSelectAll();
    void slotClearSelection();

    void accept() override;

signals:
    void didSelectEntries(const PresetEntryList &list);

private:
    Ui::PresetSelector *_ui;
    PresetEntryList _presets;
};

