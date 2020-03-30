#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QFile>

namespace Ui
{
class SettingsDialog;
class SettingsManager;
} // namespace Ui


/**
 * @brief   The SettingsManager class
 *          It is an interface with QSettings made
 *          just for not get lost with this app settings names
 */
class SettingsManager
{
private:
    /// mapping key -> key as string. access with keyStrings[enum Key key]
    static QString keyStrings[];

public:
    enum Key
    {
        IS_FIRST_START, /* first time you've started this app? */

        LAST_PST_PATH, /* absolute path of the last .pst opened */

        SPEEDS_BL_PATH_BIG,    /* path of speeds for big.LITTLE architecture*/
        SPEEDS_BL_PATH_LITTLE    /* path of speeds for big.LITTLE architecture*/
    };

    /// returns the key value as string, or "" if key does not exist
    static QString getString(SettingsManager::Key key, QString defaultVal = QString(""));

    /// returns the key value as int, or -1 if key does not exist
    static int getInt(SettingsManager::Key key, int defaultVal = -1);

    /// return the file or an invalid file
    static QFile* getFile(SettingsManager::Key key);

    /// stores key value inside standard settings
    static void save(SettingsManager::Key key, QString &value);

    /// stores key value for int values
    static void saveInt(SettingsManager::Key key, int value);

    /// stores key value as file inside standard settings. checks are performed on the filename
    static void saveFile(SettingsManager::Key key, QString &filename);

    /// remove the key and associated value
    static void removeKey(SettingsManager::Key key);

    /// reset all keys
    static void removeAllKeys();

    /// returns if key is contained
    static bool containsKey(SettingsManager::Key key);
};

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget *parent = 0);
    ~SettingsDialog();

private slots:
    void on_pushButton_cancel_clicked();

    void on_pushButton_ok_clicked();

private:
    Ui::SettingsDialog *ui;
};

#endif // SETTINGSDIALOG_H
