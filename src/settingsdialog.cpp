#include "settingsdialog.h"
#include "ui_settingsdialog.h"

#include <QSettings>
#include <QFileInfo>
#include <QString>
#include <QDebug>

QString SettingsManager::keyStrings[] = {
    "is_first_start",
    "last_path",
    "speeds_bl_path_little",
    "speeds_bl_path_big"
};

SettingsDialog::SettingsDialog(QWidget *parent) : QDialog(parent),
                                                  ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);

    QString pathLittle = SettingsManager::getString(SettingsManager::Key::SPEEDS_BL_PATH_LITTLE);
    QString pathBig = SettingsManager::getString(SettingsManager::Key::SPEEDS_BL_PATH_BIG);
    ui->lineEditLittle->setText(pathLittle);
    ui->lineEditBig->setText(pathBig);
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

void SettingsDialog::on_pushButton_cancel_clicked()
{
    close();
}

void SettingsDialog::on_pushButton_ok_clicked()
{
    QString speedsFileLittle = ui->lineEditLittle->text();
    QString speedsFileBig = ui->lineEditBig->text();
    SettingsManager::saveFile(SettingsManager::Key::SPEEDS_BL_PATH_LITTLE, speedsFileLittle);
    SettingsManager::saveFile(SettingsManager::Key::SPEEDS_BL_PATH_BIG, speedsFileBig);

    close();
}

// ------------------------------------------- SettingsManager

QString SettingsManager::getString(SettingsManager::Key key, QString defaultVal)
{
    QSettings settings;
    QString val = defaultVal;

    if (settings.contains(keyStrings[key]))
        val = settings.value(keyStrings[key], "").toString();

    return val;
}

int SettingsManager::getInt(SettingsManager::Key key, int defaultVal)
{
    QString defStr = QString::number(defaultVal);
    QString val = SettingsManager::getString(key, defStr);
    int i = val.toInt();

    return i;
}

QFile *SettingsManager::getFile(SettingsManager::Key key)
{
    QString filestr = SettingsManager::getString(key, "");
    QFile *f = new QFile(filestr);
    return f;
}

void SettingsManager::save(Key key, QString &value)
{
    QSettings settings;
    settings.setValue(keyStrings[key], value);
}

void SettingsManager::saveFile(Key key, QString &filename)
{
    QFileInfo speedsPath(filename);
    if (speedsPath.exists())
        SettingsManager::save(key, filename);
    else
        throw "not a valid filename: " + filename;
}

void SettingsManager::saveInt(Key key, int value)
{
    QString val = QString::number(value);
    SettingsManager::save(key, val);
}

void SettingsManager::removeKey(Key key)
{
    QSettings settings;
    settings.remove(keyStrings[key]);
}

void SettingsManager::removeAllKeys()
{
    QSettings settings;
    settings.clear();
}

bool SettingsManager::containsKey(SettingsManager::Key key)
{
    QSettings settings;
    return settings.contains(keyStrings[key]);
}
