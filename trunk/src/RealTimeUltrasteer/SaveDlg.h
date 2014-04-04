#pragma once

#include "ui_save.h"

enum storageType
{
    storeAutoVolume = 0,
    storeManualVolume = 1,
    storeManualFrame
};

class porta;

class SaveDlg : public QDialog, private Ui::dPropelloStore
{
    Q_OBJECT

public:
    SaveDlg(storageType type);
    virtual ~SaveDlg();

private slots:
    void onSave();
    void onChangeDirectory();

private:
    void enableEdit(QTableWidgetItem* pItem);
    void disableEdit(QTableWidgetItem* pItem);

    void updateSaveOptions();
    bool saveAutoVolume();
    bool saveManualVolume();
    bool saveSingleFrame();

private:
    storageType m_storageType;

    QString m_defaultFile;
    QString m_filePath;

    int m_fileNumber;

    bool m_prescan;
    bool m_postscan;
    bool m_rf;
    bool m_colorvv;
    bool m_colorpost;
    bool m_header;
};
